/*
 * I2C communication with LSM9DS1 9-axis IMU
 * Hunter Adams
 * vha3@cornell.edu
 */

//#include "LSM9DS1_Registers.h"
#include "LSM9DS1.h"

/* TI-RTOS Header files */
#include <xdc/std.h>
#include <unistd.h>
#include <ti/sysbios/BIOS.h>
#include <ti/sysbios/knl/Task.h>
#include <ti/sysbios/knl/Semaphore.h>

/* Driver header files */
#include <ti/sysbios/knl/Clock.h>
#include <ti/sysbios/knl/Swi.h>
#include <ti/drivers/I2C.h>
#include <ti/display/Display.h>
#include <ti/drivers/Power.h>
#include <ti/drivers/power/PowerCC26XX.h>
#include <ti/drivers/PIN.h>
#include <ti/drivers/pin/PINCC26XX.h>

/* Example/Board Header files */
#include "Board.h"

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

static Semaphore_Struct batonSemaphore;
static Semaphore_Handle batonSemaphoreHandle;

/* Make sure we have nice 8-byte alignment on the stack to avoid wasting memory */
#pragma DATA_ALIGN(initializationTaskStack, 8)
#define STACKSIZE 1024
static uint8_t initializationTaskStack[STACKSIZE];
static uint8_t calibrationTaskStack[STACKSIZE];
static uint8_t magTaskStack[STACKSIZE];
static uint8_t gyroTaskStack[STACKSIZE];
static uint8_t accelTaskStack[STACKSIZE];
static uint8_t attitudeTaskStack[STACKSIZE];

int goodToGo = 0;

/*
 * As far as I can tell, it's not strictly necessary to have
 * arguments for these functions. Having them prevents a bunch
 * of warnings, however, so I'm keeping them for now.
 */
Void initializationTaskFunc(UArg arg0, UArg arg1)
{
    while (1) {
    		Semaphore_pend(initSemaphoreHandle,BIOS_WAIT_FOREVER);
//        	Display_printf(display, 0, 0, "Initializing");
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
//		Display_printf(display, 0, 0, "Calibrating");
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
    		Semaphore_post(batonSemaphoreHandle);
    }
}


float crossProductX(float u1, float u2, float u3,
		            float v1, float v2, float v3)
{
	return u2*v3-u3*v2;
}

float crossProductY(float u1, float u2, float u3,
					float v1, float v2, float v3)
{
	return u3*v1-u1*v3;
}

float crossProductZ(float u1, float u2, float u3,
					float v1, float v2, float v3)
{
	return u1*v2-u2*v1;
}

float vectorMagnitude(float u1, float u2, float u3)
{
	return sqrt(u1*u1 + u2*u2 + u3*u3);
}

Void attitudeTaskFunc(UArg arg0, UArg arg1)
{
	while(1) {
		Semaphore_pend(attitudeSemaphoreHandle, BIOS_WAIT_FOREVER);
		Semaphore_pend(batonSemaphoreHandle, BIOS_WAIT_FOREVER);
		if(goodToGo){

			float mxf = calcMag(mx);
			float myf = calcMag(my);
			float mzf = calcMag(mz);
			float axf = calcAccel(ax);
			float ayf = calcAccel(ay);
			float azf = calcAccel(az);

			float asq = vectorMagnitude(axf, ayf, azf);

			float axn = axf/asq;
			float ayn = ayf/asq;
			float azn = azf/asq;


			float axm_x = crossProductX(axf, ayf, azf,
									    mxf, myf, mzf);
			float axm_y = crossProductY(axf, ayf, azf,
				    						mxf, myf, mzf);
			float axm_z = crossProductZ(axf, ayf, azf,
							    			mxf, myf, mzf);

			float axm_sq = vectorMagnitude(axm_x, axm_y, axm_z);
			float axm_xn = axm_x/axm_sq;
			float axm_yn = axm_y/axm_sq;
			float axm_zn = axm_z/axm_sq;


			float anxaxm_n_x = crossProductX(axn, ayn, azn,
										    axm_xn, axm_yn, axm_zn);
			float anxaxm_n_y = crossProductY(axn, ayn, azn,
										    axm_xn, axm_yn, axm_zn);
			float anxaxm_n_z = crossProductZ(axn, ayn, azn,
										    axm_xn, axm_yn, axm_zn);

			float anxaxm_sq = vectorMagnitude(anxaxm_n_x, anxaxm_n_y, anxaxm_n_z);

			float anxaxm_nxn = anxaxm_n_x/anxaxm_sq;
			float anxaxm_nyn = anxaxm_n_y/anxaxm_sq;
			float anxaxm_nzn = anxaxm_n_z/anxaxm_sq;

			float AXM_X = crossProductX(AXN, AYN, AZN,
									    MXN, MYN, MZN);
			float AXM_Y = crossProductY(AXN, AYN, AZN,
										MXN, MYN, MZN);
			float AXM_Z = crossProductZ(AXN, AYN, AZN,
										MXN, MYN, MZN);

			float AXM_SQ = vectorMagnitude(AXM_X, AXM_Y, AXM_Z);
			float AXM_XN = AXM_X/AXM_SQ;
			float AXM_YN = AXM_Y/AXM_SQ;
			float AXM_ZN = AXM_Z/AXM_SQ;

			float ANXAXM_N_X = crossProductX(AXN, AYN, AZN,
											AXM_XN, AXM_YN, AXM_ZN);
			float ANXAXM_N_Y = crossProductY(AXN, AYN, AZN,
												AXM_XN, AXM_YN, AXM_ZN);
			float ANXAXM_N_Z = crossProductZ(AXN, AYN, AZN,
												AXM_XN, AXM_YN, AXM_ZN);

			float ANXAXM_SQ = vectorMagnitude(ANXAXM_N_X, ANXAXM_N_Y, ANXAXM_N_Z);

			float ANXAXM_NXN = ANXAXM_N_X/ANXAXM_SQ;
			float ANXAXM_NYN = ANXAXM_N_Y/ANXAXM_SQ;
			float ANXAXM_NZN = ANXAXM_N_Z/ANXAXM_SQ;


			float a11 = axn*AXN + anxaxm_nxn*ANXAXM_NXN + axm_xn*AXM_XN;
			float a12 = axn*AYN + anxaxm_nxn*ANXAXM_NYN + axm_xn*AXM_YN;
			float a13 = axn*AZN + anxaxm_nxn*ANXAXM_NZN + axm_xn*AXM_ZN;

			float a21 = AXN*ayn + anxaxm_nyn*ANXAXM_NXN + axm_yn*AXM_XN;
			float a22 = ayn*AYN + anxaxm_nyn*ANXAXM_NYN + axm_yn*AXM_YN;
			float a23 = ayn*AZN + anxaxm_nyn*ANXAXM_NZN + axm_yn*AXM_ZN;

			float a31 = AXN*azn + anxaxm_nzn*ANXAXM_NXN + axm_zn*AXM_XN;
			float a32 = AYN*azn + anxaxm_nzn*ANXAXM_NYN + axm_zn*AXM_YN;
			float a33 = azn*AZN + anxaxm_nzn*ANXAXM_NZN + axm_zn*AXM_ZN;

			Display_printf(display, 0, 0, "%f, %f, %f, %f, %f, %f, %f, %f, %f", a11, a12, a13, a21, a22, a23, a31, a32, a33);
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

    /* Set up the led task */
    Task_Params task_params;
    Task_Params_init(&task_params);
    task_params.stackSize = STACKSIZE;
    task_params.priority = 3;
    task_params.stack = &initializationTaskStack;
    Task_construct(&initializationTask, initializationTaskFunc,
    		           &task_params, NULL);

    task_params.priority = 2;
    task_params.stack = &calibrationTaskStack;
    Task_construct(&calibrationTask, calibrationTaskFunc,
    				   &task_params, NULL);

    task_params.priority = 1;
    task_params.stack = &magTaskStack;
    Task_construct(&magTask, magTaskFunc,
    		           &task_params, NULL);

    task_params.priority = 1;
    task_params.stack = &gyroTaskStack;
    Task_construct(&gyroTask, gyroTaskFunc,
    		           &task_params, NULL);

    task_params.priority = 1;
    task_params.stack = &accelTaskStack;
    Task_construct(&accelTask, accelTaskFunc,
    		           &task_params, NULL);

    task_params.priority = 1;
    task_params.stack = &attitudeTaskStack;
    Task_construct(&attitudeTask, attitudeTaskFunc,
    		           &task_params, NULL);


    /* Create Semaphore */
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
