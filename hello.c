/*
 * I2C communication with LSM9DS1 9-axis IMU
 * Hunter Adams
 * vha3@cornell.edu
 */

/* TI-RTOS Header files */
#include <xdc/std.h>
#include <unistd.h>
#include <stdlib.h>
#include <xdc/runtime/System.h>
#include <xdc/runtime/Error.h>

/* Driver header files */
#include <ti/drivers/I2C.h>
#include <ti/display/Display.h>
#include <ti/drivers/Power.h>
#include <ti/drivers/power/PowerCC26XX.h>
#include <ti/drivers/PIN.h>
#include <ti/drivers/pin/PINCC26XX.h>

/* BIOS Header files */
#include <ti/sysbios/knl/Clock.h>
#include <ti/sysbios/BIOS.h>
#include <ti/sysbios/knl/Task.h>
#include <ti/sysbios/knl/Semaphore.h>

/* Example/Board Header files */
#include "Board.h"

/* Other header files */
#include "LSM9DS1.h"
//#include "TRIAD.h"
#include "smartrf_settings/smartrf_settings.h"
#include "easylink/EasyLink.h"

/* TX quantities */
#define RFEASYLINKTX_BURST_SIZE         10
#define RFEASYLINKTXPAYLOAD_LENGTH      29

/* Display Handle */
static Display_Handle display;

/* Pin handles and states*/
static PIN_Handle pinHandle;
static PIN_State pinState;

/*
 * Application button pin configuration table:
 *   - Interrupts are configured to trigger on rising edge.
 */
PIN_Config pinTable[] = {
	CC1310_LAUNCHXL_DIO15  | PIN_INPUT_EN | PIN_PULLDOWN | PIN_IRQ_POSEDGE,
	CC1310_LAUNCHXL_DIO12  | PIN_INPUT_EN | PIN_PULLDOWN | PIN_IRQ_POSEDGE,
	CC1310_LAUNCHXL_DIO22  | PIN_INPUT_EN | PIN_PULLDOWN | PIN_IRQ_POSEDGE,
	CC1310_LAUNCHXL_PIN_RLED | PIN_GPIO_OUTPUT_EN | PIN_GPIO_LOW | PIN_PUSHPULL | PIN_DRVSTR_MAX,
	CC1310_LAUNCHXL_PIN_GLED | PIN_GPIO_OUTPUT_EN | PIN_GPIO_LOW | PIN_PUSHPULL | PIN_DRVSTR_MAX,
    PIN_TERMINATE
};

/* Task structs */
Task_Struct initializationTask;
Task_Struct calibrationTask;
Task_Struct magTask;
Task_Struct gyroTask;
Task_Struct accelTask;
Task_Struct attitudeTask;
Task_Struct txTask;

/* Attitude buffer */
//float attitudeBuffer[9];

/* Semaphore structs */
static Semaphore_Struct initSemaphore;
static Semaphore_Handle initSemaphoreHandle;

static Semaphore_Struct calibSemaphore;
static Semaphore_Handle calibSemaphoreHandle;

static Semaphore_Struct magSemaphore;
static Semaphore_Handle magSemaphoreHandle;

static Semaphore_Struct gyroSemaphore;
static Semaphore_Handle gyroSemaphoreHandle;

static Semaphore_Struct accelSemaphore;
static Semaphore_Handle accelSemaphoreHandle;

static Semaphore_Struct attitudeSemaphore;
static Semaphore_Handle attitudeSemaphoreHandle;

//static Semaphore_Struct txSemaphore;
//static Semaphore_Handle txSemaphoreHandle;

static Semaphore_Struct txGyroSemaphore;
static Semaphore_Handle txGyroSemaphoreHandle;

static Semaphore_Struct txAccelSemaphore;
static Semaphore_Handle txAccelSemaphoreHandle;

static Semaphore_Struct txMagSemaphore;
static Semaphore_Handle txMagSemaphoreHandle;

static Semaphore_Struct batonSemaphore;
static Semaphore_Handle batonSemaphoreHandle;

/* Make sure we have nice 8-byte alignment on the stack to avoid wasting memory */
#pragma DATA_ALIGN(txTaskStack, 8)
static uint8_t initializationTaskStack[400];
static uint8_t calibrationTaskStack[400];
static uint8_t magTaskStack[400];
static uint8_t gyroTaskStack[400];
static uint8_t accelTaskStack[400];
static uint8_t attitudeTaskStack[400];
static uint8_t txTaskStack[1024];

int goodToGo = 0;

/* ###########################################################
 * Some Bit Manipulation Functions
 */
static uint16_t seqNumber;

uint8_t sign(int16_t x)
{
	if(x>0){
		return 1;
	}
	else{
		return 0;
	}
}

uint8_t lowerPart(int16_t x){
	return (x & 0xff);
}

uint8_t upperPart(int16_t x){
	return (x >> 8);
}
/*
 * ###########################################################
 */

/* Message is SPACE SYSTEMS DESIGN STUDIO */
uint8_t message[30] = {0x20, 0x53, 0x50, 0x41, 0x43, 0x45, 0x20, 0x53,
		0x59, 0x53, 0x54, 0x45, 0x4d, 0x53, 0x20, 0x44, 0x45, 0x53,
		0x49, 0x47, 0x4e, 0x20, 0x53, 0x54, 0x55, 0x44, 0x49, 0x4f,
		0x20, 0x20};

/*
 * As far as I can tell, it's not strictly necessary to have
 * arguments for these functions. Having them prevents a bunch
 * of warnings, however, so I'm keeping them for now.
 */
Void initializationTaskFunc(UArg arg0, UArg arg1)
{
    while (1) {
    		Semaphore_pend(initSemaphoreHandle,BIOS_WAIT_FOREVER);
        uint16_t workpls = LSM9DS1begin();
        configInt(XG_INT1, INT_DRDY_G, INT_ACTIVE_HIGH, INT_PUSH_PULL);
        configInt(XG_INT2, INT_DRDY_XL, INT_ACTIVE_HIGH, INT_PUSH_PULL);
        Semaphore_post(calibSemaphoreHandle);
    }
}

Void calibrationTaskFunc(UArg arg0, UArg arg1)
{
	while (1) {
		Semaphore_pend(calibSemaphoreHandle, BIOS_WAIT_FOREVER);
		calibrate(1);
//		calibrateMag(1);
		/* getMagInitial is only required if you're calibrating for the computer attitude */
		getMagInitial();
		goodToGo += 1;
		readGyro();
		readAccel();
		readMag();
	}
}

Void magTaskFunc(UArg arg0, UArg arg1)
{
    while (1) {
    		Semaphore_pend(magSemaphoreHandle, BIOS_WAIT_FOREVER);
    		Semaphore_pend(batonSemaphoreHandle, BIOS_WAIT_FOREVER);
    		if(goodToGo){
    			readMag();
    		}
    		Semaphore_post(attitudeSemaphoreHandle);
    		Semaphore_post(txMagSemaphoreHandle);
    		Semaphore_post(batonSemaphoreHandle);
    }
}

Void gyroTaskFunc(UArg arg0, UArg arg1)
{
    while (1) {
    		Semaphore_pend(gyroSemaphoreHandle, BIOS_WAIT_FOREVER);
    		Semaphore_pend(batonSemaphoreHandle, BIOS_WAIT_FOREVER);
    		if(goodToGo){
    			readGyro();
    		}
    		Semaphore_post(attitudeSemaphoreHandle);
    		Semaphore_post(txGyroSemaphoreHandle);
    		Semaphore_post(batonSemaphoreHandle);
    }
}

Void accelTaskFunc(UArg arg0, UArg arg1)
{
    while (1) {
    		Semaphore_pend(accelSemaphoreHandle, BIOS_WAIT_FOREVER);
    		Semaphore_pend(batonSemaphoreHandle, BIOS_WAIT_FOREVER);
    		if(goodToGo){
    			readAccel();
    		}
    		Semaphore_post(attitudeSemaphoreHandle);
    		Semaphore_post(txAccelSemaphoreHandle);
    		Semaphore_post(batonSemaphoreHandle);
    }
}


Void attitudeTaskFunc(UArg arg0, UArg arg1)
{
	while(1) {
		Semaphore_pend(attitudeSemaphoreHandle, BIOS_WAIT_FOREVER);
		Semaphore_pend(batonSemaphoreHandle, BIOS_WAIT_FOREVER);
		if(goodToGo){

//			Display_printf(display, 0, 0, "attitude");

//			computeAttitude(mx, my, mz, ax, ay, az, attitudeBuffer);
//			float a11 = attitudeBuffer[0];
//			float a12 = attitudeBuffer[1];
//			float a13 = attitudeBuffer[2];
//			float a21 = attitudeBuffer[3];
//			float a22 = attitudeBuffer[4];
//			float a23 = attitudeBuffer[5];
//			float a31 = attitudeBuffer[6];
//			float a32 = attitudeBuffer[7];
//			float a33 = attitudeBuffer[8];
//			Display_printf(display, 0, 0, "%f, %f, %f, %f, %f, %f, %f, %f, %f", a11, a12, a13, a21, a22, a23, a31, a32, a33);
		}
		Semaphore_post(batonSemaphoreHandle);
//		Semaphore_post(txSemaphoreHandle);
	}
}

Void txTaskFunc(UArg arg0, UArg arg1)
{
	EasyLink_init(EasyLink_Phy_Custom);
	EasyLink_setRfPwr(12);
	while(1) {
		Semaphore_pend(txGyroSemaphoreHandle, BIOS_WAIT_FOREVER);
		Semaphore_pend(txAccelSemaphoreHandle, BIOS_WAIT_FOREVER);
		Semaphore_pend(txMagSemaphoreHandle, BIOS_WAIT_FOREVER);
		Semaphore_pend(batonSemaphoreHandle, BIOS_WAIT_FOREVER);
		if(goodToGo){
			EasyLink_abort();
			EasyLink_TxPacket txPacket =  { {0}, 0, 0, {0} };
//			uint8_t i;
			txPacket.payload[0] = (uint8_t)(seqNumber >> 8);
			txPacket.payload[1] = (uint8_t)(seqNumber++);
			txPacket.payload[2] = sign(ax);
			txPacket.payload[3] = upperPart(ax);
			txPacket.payload[4] = lowerPart(ax);
			txPacket.payload[5] = sign(ay);
			txPacket.payload[6] = upperPart(ay);
			txPacket.payload[7] = lowerPart(ay);
			txPacket.payload[8] = sign(az);
			txPacket.payload[9] = upperPart(az);
			txPacket.payload[10] = lowerPart(az);

			txPacket.payload[11] = sign(gx);
			txPacket.payload[12] = upperPart(gx);
			txPacket.payload[13] = lowerPart(gx);
			txPacket.payload[14] = sign(gy);
			txPacket.payload[15] = upperPart(gy);
			txPacket.payload[16] = lowerPart(gy);
			txPacket.payload[17] = sign(gz);
			txPacket.payload[18] = upperPart(gz);
			txPacket.payload[19] = lowerPart(gz);

			txPacket.payload[20] = sign(mx);
			txPacket.payload[21] = upperPart(mx);
			txPacket.payload[22] = lowerPart(mx);
			txPacket.payload[23] = sign(my);
			txPacket.payload[24] = upperPart(my);
			txPacket.payload[25] = lowerPart(my);
			txPacket.payload[26] = sign(mz);
			txPacket.payload[27] = upperPart(mz);
			txPacket.payload[28] = lowerPart(mz);
//			for (i = 0; i < RFEASYLINKTXPAYLOAD_LENGTH; i++)
//			{
//			  txPacket.payload[i] = message[i];
//			}
	        txPacket.len = RFEASYLINKTXPAYLOAD_LENGTH;
	        txPacket.dstAddr[0] = 0xaa;
	        txPacket.absTime = 0;
	        EasyLink_Status result = EasyLink_transmit(&txPacket);

	        if (result == EasyLink_Status_Success)
			{
				/* Toggle LED1 to indicate TX */
				PIN_setOutputValue(pinHandle, Board_PIN_LED1,!PIN_getOutputValue(Board_PIN_LED1));
			}
			else
			{
				/* Toggle LED1 and LED2 to indicate error */
				PIN_setOutputValue(pinHandle, Board_PIN_LED1,!PIN_getOutputValue(Board_PIN_LED1));
				PIN_setOutputValue(pinHandle, Board_PIN_LED2,!PIN_getOutputValue(Board_PIN_LED2));
			}
		}
		Semaphore_post(batonSemaphoreHandle);
	}
}



void pinCallback(PIN_Handle handle, PIN_Id pinId) {
    uint32_t currVal = 0;
	switch (pinId) {
		case CC1310_LAUNCHXL_DIO12:
			currVal =  PIN_getOutputValue(Board_PIN_LED0);
			PIN_setOutputValue(pinHandle, Board_PIN_LED0, !currVal);
			Semaphore_post(gyroSemaphoreHandle);
			break;

		case CC1310_LAUNCHXL_DIO15:
			currVal =  PIN_getOutputValue(Board_PIN_LED1);
			PIN_setOutputValue(pinHandle, Board_PIN_LED1, !currVal);
			Semaphore_post(magSemaphoreHandle);
			break;

		case CC1310_LAUNCHXL_DIO22:
			Semaphore_post(accelSemaphoreHandle);
			break;

		default:
			/* Do nothing */
			break;
	}
}


/*
 *  ======== main ========
 *
 */
int main(void)
{
	/* Initialize TI drivers */
    Board_initGeneral();
    Display_init();
    I2C_init();
    PIN_init(pinTable);

    /* Open Display */
    display = Display_open(Display_Type_UART, NULL);
    if (display == NULL){
    		while(1);
    }


    	/* Open I2C connection to LSM9DS1 */
    LSM9DS1init();
    initI2C();

    /* Setup/create tasks */
    Task_Params task_params;
    Task_Params_init(&task_params);
    task_params.stackSize = 400;
    task_params.priority = 3;
    task_params.stack = &initializationTaskStack;
    Task_construct(&initializationTask, initializationTaskFunc,
    		           &task_params, NULL);

    task_params.stackSize = 400;
    task_params.priority = 2;
    task_params.stack = &calibrationTaskStack;
    Task_construct(&calibrationTask, calibrationTaskFunc,
    				   &task_params, NULL);

    	task_params.stackSize = 400;
    task_params.priority = 1;
    task_params.stack = &magTaskStack;
    Task_construct(&magTask, magTaskFunc,
    		           &task_params, NULL);

    task_params.stackSize = 400;
    task_params.priority = 1;
    task_params.stack = &gyroTaskStack;
    Task_construct(&gyroTask, gyroTaskFunc,
    		           &task_params, NULL);

    task_params.stackSize = 400;
    task_params.priority = 1;
    task_params.stack = &accelTaskStack;
    Task_construct(&accelTask, accelTaskFunc,
    		           &task_params, NULL);

    task_params.stackSize = 400;
    task_params.priority = 1;
    task_params.stack = &attitudeTaskStack;
    Task_construct(&attitudeTask, attitudeTaskFunc,
    		           &task_params, NULL);

    task_params.stackSize = 1024;
    task_params.priority = 1;
	task_params.stack = &txTaskStack;
	Task_construct(&txTask, txTaskFunc,
				   &task_params, NULL);


    /* Create Semaphores */
    Semaphore_construct(&initSemaphore, 1, NULL);
    initSemaphoreHandle = Semaphore_handle(&initSemaphore);

    Semaphore_construct(&calibSemaphore, 0, NULL);
    calibSemaphoreHandle = Semaphore_handle(&calibSemaphore);

    Semaphore_construct(&magSemaphore, 0, NULL);
    magSemaphoreHandle = Semaphore_handle(&magSemaphore);

    Semaphore_construct(&gyroSemaphore, 0, NULL);
    gyroSemaphoreHandle = Semaphore_handle(&gyroSemaphore);

    Semaphore_construct(&accelSemaphore, 0, NULL);
    accelSemaphoreHandle = Semaphore_handle(&accelSemaphore);

    Semaphore_construct(&attitudeSemaphore, 0, NULL);
    attitudeSemaphoreHandle = Semaphore_handle(&attitudeSemaphore);

//    Semaphore_construct(&txSemaphore, 0, NULL);
//    txSemaphoreHandle = Semaphore_handle(&txSemaphore);

	Semaphore_construct(&txGyroSemaphore, 0, NULL);
	txGyroSemaphoreHandle = Semaphore_handle(&txGyroSemaphore);

	Semaphore_construct(&txAccelSemaphore, 0, NULL);
	txAccelSemaphoreHandle = Semaphore_handle(&txAccelSemaphore);

	Semaphore_construct(&txMagSemaphore, 0, NULL);
	txMagSemaphoreHandle = Semaphore_handle(&txMagSemaphore);

    Semaphore_construct(&batonSemaphore, 1, NULL);
    batonSemaphoreHandle = Semaphore_handle(&batonSemaphore);


    pinHandle = PIN_open(&pinState, pinTable);
	if(!pinHandle) {
		/* Error initializing button pins */
		while(1);
	}

    /* Setup callback for button pins */
    if (PIN_registerIntCb(pinHandle, &pinCallback) != 0) {
        /* Error registering button callback function */
        while(1);
    }

    /* Start kernel. */
    BIOS_start();

    return (0);
}
