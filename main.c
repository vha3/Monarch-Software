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
#include <ti/drivers/Power.h>
#include <ti/drivers/power/PowerCC26XX.h>

/* BIOS Header files */
#include <ti/sysbios/BIOS.h>

/* Example/Board Header files */
#include "Board.h"

/* Customized Initializations and Resources */
#include "Semaphore_Initialization.h"
#include "Clock_Initialization.h"
#include "Shared_Resources.h"
#include "Watchdog_Initialization.h"
#include "Pin_Initialization.h"

/* Task Inclusion */
#include "Tasks/Initialization_Task.h"
#include "Tasks/Calibration_Task.h"
#include "Tasks/IMU_Tasks.h"
#include "Tasks/GPS_Task.h"
#include "Tasks/Radio/RF_RX_Tasks.h"
#include "Tasks/Radio/RF_TX_Tasks.h"


/*
 *  ======== main ========
 *
 */
int main(void)
{
	/* Initialize TI drivers */
    Board_initGeneral();
    I2C_init();
    PIN_init(pinTable);
    UART_init();

    wdtSetup();
    LSM9DS1init();
    initI2C();
    clockSetup();
	semaphoreSetup();
	pinSetup();

    createInitializationTask();
    createCalibrationTask();
    createIMUTasks();
    createGPSTask();
    createRFRXTasks();
    createRFTXTasks();

    /* Start kernel. */
    BIOS_start();

    return (0);
}
