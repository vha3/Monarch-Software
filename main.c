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

/* Power header files */
#include <ti/drivers/Power.h>
#include <ti/drivers/power/PowerCC26XX.h>

/* BIOS Header files */
#include <ti/sysbios/BIOS.h>

/* Peripherals */
#include <Peripherals/Clock_Initialization.h>
#include <Peripherals/Pin_Initialization.h>
#include <Peripherals/Watchdog_Initialization.h>
#include <Peripherals/ADC_Initialization.h>
#include <Peripherals/Display_Initialization.h>
#include <Peripherals/PWM_Initialization.h>

/* Board Header file */
#include "Board.h"

/* Tasks */
#include "Tasks/Initialization_Task.h"
#include "Tasks/Calibration_Task.h"
#include "Tasks/GPS_Task.h"
#include "Tasks/Radio/RF_RX_Tasks.h"
#include "Tasks/Radio/RF_TX_Tasks.h"
#include "Tasks/IMU/IMU_Tasks.h"
#include <Tasks/Semaphore_Initialization.h>
#include <Tasks/Shared_Resources.h>
#include <Tasks/ADC_Tasks.h>
#include <Tasks/PWM_Tasks.h>


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
//    ADC_init();
    PWM_init();

    /* Use one or the other of the below. If using GPS, need UART */
//    UART_init();
    Display_init();

    /* Setup peripherals and semaphores */
    wdtSetup();
    LSM9DS1init();
    initI2C();
    clockSetup();
	semaphoreSetup();
	pinSetup();
	pwm2Setup();
	pwm1Setup();

//	adc1Setup();
//	adc0Setup();

	displaySetup();

	/* Construct tasks */
    createInitializationTask();
    createCalibrationTask();
    createMagTask();
    createGyroTask();
    createAccelTask();
//    createGPSTask();
//    createRFRXTasks();
    createRFTXTasks();
//    createADC1Task();
//    createADC0Task();
    createPWMTask();

    /* Start kernel. */
    BIOS_start();

    return (0);
}
