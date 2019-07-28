/*
 * Software for Monarch chipsat
 * V. Hunter Adams
 * vha3@cornell.edu
 */

/* TI-RTOS Header files */
#include <xdc/std.h>
#include <unistd.h>
#include <stdlib.h>
#include <xdc/runtime/System.h>
#include <xdc/runtime/Error.h>

/* Power header files */
#include <ti/drivers/Power.h>
#include <ti/drivers/power/PowerCC26XX.h>


/* BIOS Header files */
#include <ti/sysbios/BIOS.h>

/* Peripherals */
//#include <Peripherals/Clock_Initialization.h>
#include <Peripherals/Pin_Initialization.h>
#include <Peripherals/Watchdog_Initialization.h>
#include <Peripherals/ADC_Initialization.h>
#include <Peripherals/Display_Initialization.h>
//#include <Peripherals/PWM_Initialization.h>

/* Board Header file */
#include "Board.h"

/* Tasks */
#include "Tasks/GPS_Task.h"
//#include "Tasks/Radio/RF_RX_Tasks.h"
#include "Tasks/Radio/RF_TX_Tasks.h"
#include <Tasks/Semaphore_Initialization.h>
#include <Tasks/Shared_Resources.h>
#include <Tasks/ADC_Tasks.h>
#include <Tasks/I2C/I2C_Tasks.h>
#include <Tasks/Startup_Task.h>
//#include <Tasks/I2C_Shutdown_Task.h>
//#include <Tasks/PWM_Tasks.h>


/*
 *  ======== main ========
 *
 */
int main(void)
{
	/* Initialize TI drivers */
    Board_initGeneral();
    PIN_init(pinTable);

    /* Setup watchdog */
//	wdtSetup();
//	Watchdog_clear(watchdogHandle);

//	PWM_init();
//	pwmSetup();

    /* Use one or the other of the below. If using GPS, need UART */
//    Display_init();
//    displaySetup();

    /* Setup peripherals and semaphores */
//    clockSetup();
	semaphoreSetup();
	pinSetup();

	CC1310_LAUNCHXL_shutDownExtFlash();

	PIN_setOutputValue(pinHandle, IOID_21, 0);



	/* Construct tasks */
	createStartupTask();
    createi2cTask();
    createADCTask();
    createGPSTask();
    createRFTXTasks();

//    createRFRXTasks();
//    createPWMTask();
//    createi2cShutdownTask();

    /* Start kernel. */
    BIOS_start();

    return (0);
}
