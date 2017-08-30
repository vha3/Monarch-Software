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
#include <ti/drivers/GPIO.h>
#include <ti/sysbios/knl/Clock.h>
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
static PIN_Handle buttonPinHandle;
static PIN_State buttonPinState;

/*
 * Application button pin configuration table:
 *   - Buttons interrupts are configured to trigger on rising edge.
 */
PIN_Config buttonPinTable[] = {
	CC1310_LAUNCHXL_DIO15  | PIN_INPUT_EN | PIN_PULLDOWN | PIN_IRQ_POSEDGE,
	Board_PIN_BUTTON0  | PIN_INPUT_EN | PIN_PULLUP | PIN_IRQ_NEGEDGE,
	CC1310_LAUNCHXL_PIN_RLED | PIN_GPIO_OUTPUT_EN | PIN_GPIO_LOW | PIN_PUSHPULL | PIN_DRVSTR_MAX,
	CC1310_LAUNCHXL_PIN_GLED | PIN_GPIO_OUTPUT_EN | PIN_GPIO_LOW | PIN_PUSHPULL | PIN_DRVSTR_MAX,
    PIN_TERMINATE
};



/* Task structs */
Task_Struct initializationTask;
Task_Struct runningTask;
Task_Struct calibrationTask;

/* Semaphore structs */
static Semaphore_Struct IMUStartsemaphore;
static Semaphore_Handle IMUStartsemaphoreHandle;

static Semaphore_Struct CalibrationStartsemaphore;
static Semaphore_Handle CalibrationStartsemaphoreHandle;

static Semaphore_Struct getDatasemaphore;
static Semaphore_Handle getDatasemaphoreHandle;

/* Make sure we have nice 8-byte alignment on the stack to avoid wasting memory */
#pragma DATA_ALIGN(initializationTaskStack, 8)
#define STACKSIZE 2048
static uint8_t initializationTaskStack[STACKSIZE];
static uint8_t runningTaskStack[STACKSIZE];
static uint8_t calibrationTaskStack[STACKSIZE];

int goodToGo = 0;

Void initializationTaskFunc(UArg arg0, UArg arg1)
{
    while (1) {
    		Semaphore_pend(IMUStartsemaphoreHandle,BIOS_WAIT_FOREVER);
        	Display_printf(display, 0, 0, "Initializing");
        uint16_t workpls = LSM9DS1begin();
        configInt(XG_INT1, INT_DRDY_G, INT_ACTIVE_HIGH, INT_PUSH_PULL);
        configInt(XG_INT2, INT_DRDY_XL, INT_ACTIVE_HIGH, INT_PUSH_PULL);
        Semaphore_post(CalibrationStartsemaphoreHandle);
//        Task_sleep(500 * (1000 / Clock_tickPeriod));
    }
}

Void calibrationTaskFunc(UArg arg0, UArg arg1)
{
	while (1) {
		Semaphore_pend(CalibrationStartsemaphoreHandle, BIOS_WAIT_FOREVER);
		Display_printf(display, 0, 0, "Calibrating");
		calibrate(1);
		calibrateMag(1);
		goodToGo += 1;
	}
}

Void runningTaskFunc(UArg arg0, UArg arg1)
{
    while (1) {
    		Semaphore_pend(getDatasemaphoreHandle, BIOS_WAIT_FOREVER);
    		if(goodToGo){
    		readMag();
				Display_printf(display, 0, 0, "%f", calcMag(mx));
				Display_printf(display, 0, 0, "%f", calcMag(my));
				Display_printf(display, 0, 0, "%f", calcMag(mz));
				Display_printf(display, 0, 0, "\n\n");
    		}
        /* Wait a while, because doWork should be a periodic thing, not continuous.*/
//        Task_sleep(50 * (1000 / Clock_tickPeriod));
    }
}

void buttonCallbackFxn(PIN_Handle handle, PIN_Id pinId) {
    uint32_t currVal = 0;
	switch (pinId) {
		case Board_PIN_BUTTON0:
			currVal =  PIN_getOutputValue(Board_PIN_LED0);
			PIN_setOutputValue(buttonPinHandle, Board_PIN_LED0, !currVal);
			break;

		case CC1310_LAUNCHXL_DIO15:
			currVal =  PIN_getOutputValue(Board_PIN_LED1);
			PIN_setOutputValue(buttonPinHandle, Board_PIN_LED1, !currVal);
			Semaphore_post(getDatasemaphoreHandle);
			break;

		default:
			/* Do nothing */
			break;
	}
}

///*
// *  ======== gpioButtonFxn0 ========
// *  Callback function for the GPIO interrupt on Board_GPIO_BUTTON0.
// */
//void buttonCallbackFxn(PIN_Handle handle, PIN_Id pinId)
//{
//    /* Clear the GPIO interrupt and toggle an LED */
//	uint32_t currVal = 0;
//	currVal =  PIN_getOutputValue(Board_PIN_LED0);
//	PIN_setOutputValue(buttonPinHandle, Board_PIN_LED0, !currVal);
//    Semaphore_post(getDatasemaphoreHandle);
//}

/*
 *  ======== main ========
 *
 */
int main(void)
{
	/* Initialize TI drivers */
    Board_initGeneral();
    GPIO_init();
    Display_init();
    I2C_init();
    PIN_init(buttonPinTable);

    /* Open Display */
    display = Display_open(Display_Type_UART, NULL);
    if (display == NULL){
    		while(1);
    }


    	/* Open I2C connection to LSM9DS1 */
    LSM9DS1init();
    initI2C();

    /* Set up the led task */
    Task_Params initializationTaskParams;
    Task_Params_init(&initializationTaskParams);
    initializationTaskParams.stackSize = STACKSIZE;
    initializationTaskParams.priority = 3;
    initializationTaskParams.stack = &initializationTaskStack;
    Task_construct(&initializationTask, initializationTaskFunc,
    		           &initializationTaskParams, NULL);

    initializationTaskParams.priority = 2;
    initializationTaskParams.stack = &runningTaskStack;
    Task_construct(&runningTask, runningTaskFunc,
    		           &initializationTaskParams, NULL);

    initializationTaskParams.priority = 1;
    initializationTaskParams.stack = &calibrationTaskStack;
    Task_construct(&calibrationTask, calibrationTaskFunc,
    				   &initializationTaskParams, NULL);

    /* Create Semaphore */
    Semaphore_construct(&IMUStartsemaphore, 1, NULL);
    IMUStartsemaphoreHandle = Semaphore_handle(&IMUStartsemaphore);

    Semaphore_construct(&CalibrationStartsemaphore, 0, NULL);
    CalibrationStartsemaphoreHandle = Semaphore_handle(&CalibrationStartsemaphore);

    Semaphore_construct(&getDatasemaphore, 0, NULL);
    getDatasemaphoreHandle = Semaphore_handle(&getDatasemaphore);

    buttonPinHandle = PIN_open(&buttonPinState, buttonPinTable);
	if(!buttonPinHandle) {
		/* Error initializing button pins */
		while(1);
	}

    /* Setup callback for button pins */
    if (PIN_registerIntCb(buttonPinHandle, &buttonCallbackFxn) != 0) {
        /* Error registering button callback function */
        while(1);
    }

//    /* install Button callback */
//    GPIO_setCallback(Board_GPIO_BUTTON0, gpioButtonFxn0);
//
//    /* Enable interrupts */
//    GPIO_enableInt(Board_GPIO_BUTTON0);

    /* Start kernel. */
    BIOS_start();

    return (0);
}
