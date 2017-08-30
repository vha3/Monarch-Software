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
 *   - Buttons interrupts are configured to trigger on rising edge.
 */
PIN_Config pinTable[] = {
	CC1310_LAUNCHXL_DIO15  | PIN_INPUT_EN | PIN_PULLDOWN | PIN_IRQ_POSEDGE,
	Board_PIN_BUTTON0  | PIN_INPUT_EN | PIN_PULLUP | PIN_IRQ_NEGEDGE,
	CC1310_LAUNCHXL_PIN_RLED | PIN_GPIO_OUTPUT_EN | PIN_GPIO_LOW | PIN_PUSHPULL | PIN_DRVSTR_MAX,
	CC1310_LAUNCHXL_PIN_GLED | PIN_GPIO_OUTPUT_EN | PIN_GPIO_LOW | PIN_PUSHPULL | PIN_DRVSTR_MAX,
    PIN_TERMINATE
};

/* Task structs */
Task_Struct initializationTask;
Task_Struct calibrationTask;
Task_Struct magTask;

/* Semaphore structs */
static Semaphore_Struct initSemaphore;
static Semaphore_Handle initSemaphoreHandle;

static Semaphore_Struct calibSemaphore;
static Semaphore_Handle calibSemaphoreHandle;

static Semaphore_Struct magSemaphore;
static Semaphore_Handle magSemaphoreHandle;

/* Make sure we have nice 8-byte alignment on the stack to avoid wasting memory */
#pragma DATA_ALIGN(initializationTaskStack, 8)
#define STACKSIZE 2048
static uint8_t initializationTaskStack[STACKSIZE];
static uint8_t calibrationTaskStack[STACKSIZE];
static uint8_t magTaskStack[STACKSIZE];

int goodToGo = 0;

Void initializationTaskFunc(UArg arg0, UArg arg1)
{
    while (1) {
    		Semaphore_pend(initSemaphoreHandle,BIOS_WAIT_FOREVER);
        	Display_printf(display, 0, 0, "Initializing");
        uint16_t workpls = LSM9DS1begin();
        configInt(XG_INT1, INT_DRDY_G, INT_ACTIVE_HIGH, INT_PUSH_PULL);
        configInt(XG_INT2, INT_DRDY_XL, INT_ACTIVE_HIGH, INT_PUSH_PULL);
        Semaphore_post(calibSemaphoreHandle);
//        Task_sleep(500 * (1000 / Clock_tickPeriod));
    }
}

Void calibrationTaskFunc(UArg arg0, UArg arg1)
{
	while (1) {
		Semaphore_pend(calibSemaphoreHandle, BIOS_WAIT_FOREVER);
		Display_printf(display, 0, 0, "Calibrating");
		calibrate(1);
		calibrateMag(1);
		goodToGo += 1;
	}
}

Void magTaskFunc(UArg arg0, UArg arg1)
{
    while (1) {
    		Semaphore_pend(magSemaphoreHandle, BIOS_WAIT_FOREVER);
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


void pinCallback(PIN_Handle handle, PIN_Id pinId) {
    uint32_t currVal = 0;
	switch (pinId) {
		case Board_PIN_BUTTON0:
			currVal =  PIN_getOutputValue(Board_PIN_LED0);
			PIN_setOutputValue(pinHandle, Board_PIN_LED0, !currVal);
			break;

		case CC1310_LAUNCHXL_DIO15:
			currVal =  PIN_getOutputValue(Board_PIN_LED1);
			PIN_setOutputValue(pinHandle, Board_PIN_LED1, !currVal);
			Semaphore_post(magSemaphoreHandle);
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
//    GPIO_init();
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


    /* Create Semaphore */
    Semaphore_construct(&initSemaphore, 1, NULL);
    initSemaphoreHandle = Semaphore_handle(&initSemaphore);

    Semaphore_construct(&calibSemaphore, 0, NULL);
    calibSemaphoreHandle = Semaphore_handle(&calibSemaphore);

    Semaphore_construct(&magSemaphore, 0, NULL);
    magSemaphoreHandle = Semaphore_handle(&magSemaphore);

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
