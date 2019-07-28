/*
 * Startup.h
 *
 *  Created on: May 21, 2019
 *      Author: hunteradams
 */

#ifndef TASKS_STARTUP_TASK_H_
#define TASKS_STARTUP_TASK_H_

#include <ti/sysbios/knl/Task.h>
#include "Semaphore_Initialization.h"
#include "Shared_Resources.h"

/* Task struct */
Task_Struct startupTask;

/* Task memory allocation */
static uint8_t startupTaskStack[400];

/* ADC conversion result variables */
uint16_t adcValue0;
uint32_t adcValue0MicroVolt;

uint16_t adcValue3_2;
uint32_t adcValue3MicroVolt_2;

/* Task function */
Void startupTaskFunc(UArg arg0, UArg arg1)
{

	/* Initialize ADC */
	ADC_init();

	/* Initialize I2C */
	I2C_init();

	/* Setup IMU */
	LSM9DS1init();

	/* Initialize UART */
	UART_init();

	/* Setup UART */
    UART_Params_init(&uartParams);
    uartParams.writeDataMode = UART_DATA_BINARY;
    uartParams.readDataMode = UART_DATA_BINARY;
    uartParams.readReturnMode = UART_RETURN_FULL;
    uartParams.readEcho = UART_ECHO_OFF;
    uartParams.baudRate = 9600;

    uartParams.readMode       = UART_MODE_CALLBACK;
    uartParams.readCallback   = &Serial_RxDataCallback;


    PIN_setOutputValue(pinHandle, IOID_21, 1);


    int_fast16_t res1;
    adc3Setup();
    res1=ADC_convert(adc3, &adcValue3_2);
    while (res1 != ADC_STATUS_SUCCESS){
    	res1=ADC_convert(adc3, &adcValue3_2);
    }
    adcValue3MicroVolt_2 = ADC_convertRawToMicroVolts(adc3, adcValue3_2);
    ADC_close(adc3);
    if (adcValue3MicroVolt_2 > 3100000){
    	PIN_setOutputValue(pinHandle, Board_PIN_LED0, 1);
    	Semaphore_pend(gpsfixSemaphoreHandle, BIOS_WAIT_FOREVER);
    }
    else{

    }


	while(1) {

		Semaphore_pend(startSemaphoreHandle, BIOS_WAIT_FOREVER);
		wdtSetup();
		Watchdog_clear(watchdogHandle);
		Task_sleep(10000);

		/* Variable to hold ADC status */
		int_fast16_t res;

		/* Start adc channel 0 (connected to supercap)*/
		adc0Setup();

		/* Retrieve ADC measurement from channel 0, store in adcValue0 */
		res = ADC_convert(adc0, &adcValue0);

		/* Check ADC status */
		if (res == ADC_STATUS_SUCCESS) {

			/* Convert measured value to microvolts, store in adcValue0MicroVolt */
			adcValue0MicroVolt = ADC_convertRawToMicroVolts(adc0, adcValue0);

//			Display_printf(display, 0, 0,
//				"ADC channel 0 convert result: %d uV\n", adcValue0MicroVolt);

			/* Store capacitor state of charge */
			capacitor_charge = adcValue0;
			capacitor_charge_uv = adcValue0MicroVolt;

			/* Turn on GPS and all other sensors */
			PIN_setOutputValue(pinHandle, IOID_21, 1);

			/* Use a short software delay to give things time to power up (100 ms)*/
			Task_sleep(100000);

			/* Close ADC channel 0 */
			ADC_close(adc0);

			/* Unlock i2c task */
			Semaphore_post(magLockSemaphoreHandle);

			/* Sleep for an hour (very long) */
//			Task_sleep(360000000);

		}
		else {
//			Display_printf(display, 0, 0, "ADC channel 0 convert failed\n");
			ADC_close(adc0);
			Task_sleep(100000);
		}
	}
}

void createStartupTask()
{
	Task_Params task_params;
	Task_Params_init(&task_params);
	task_params.stackSize = 400;
	task_params.priority = 1;
	task_params.stack = &startupTaskStack;
	Task_construct(&startupTask, startupTaskFunc,
					   &task_params, NULL);
}





#endif /* TASKS_STARTUP_TASK_H_ */
