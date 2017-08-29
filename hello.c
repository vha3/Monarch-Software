/*
 * I2C communication with LSM9DS1 9-axis IMU
 * Hunter Adams
 * vha3@cornell.edu
 */

//#include "LSM9DS1_Registers.h"
#include "LSM9DS1.h"

/* TI-RTOS Header files */
#include <xdc/std.h>
#include <ti/sysbios/BIOS.h>
#include <ti/sysbios/knl/Task.h>
#include <ti/sysbios/knl/Semaphore.h>
//#include <math.h>

/* Driver header files */
#include <ti/drivers/GPIO.h>
#include <ti/sysbios/knl/Clock.h>
#include <ti/drivers/I2C.h>
#include <ti/display/Display.h>

/* Example/Board Header files */
#include "Board.h"

static Display_Handle display;


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
//		Task_sleep(500 * (1000 / Clock_tickPeriod));
	}
}

Void runningTaskFunc(UArg arg0, UArg arg1)
{
    while (1) {
    		Semaphore_pend(getDatasemaphoreHandle, BIOS_WAIT_FOREVER);
    		readMag();
    		Display_printf(display, 0, 0, "%f", calcMag(mx));
    		Display_printf(display, 0, 0, "%f", calcMag(my));
    		Display_printf(display, 0, 0, "%f", calcMag(mz));
    		Display_printf(display, 0, 0, "%02x", settings.mag.sampleRate);
    		Display_printf(display, 0, 0, "\n\n");
        /* Wait a while, because doWork should be a periodic thing, not continuous.*/
        Task_sleep(50 * (1000 / Clock_tickPeriod));
    }
}

/*
 *  ======== gpioButtonFxn0 ========
 *  Callback function for the GPIO interrupt on Board_GPIO_BUTTON0.
 */
void gpioButtonFxn0(uint_least8_t index)
{
    /* Clear the GPIO interrupt and toggle an LED */
    GPIO_toggle(Board_GPIO_LED0);
    Semaphore_post(getDatasemaphoreHandle);
}

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

    /* install Button callback */
    GPIO_setCallback(Board_GPIO_BUTTON0, gpioButtonFxn0);

    /* Enable interrupts */
    GPIO_enableInt(Board_GPIO_BUTTON0);

    /* Start kernel. */
    BIOS_start();

    return (0);
}
