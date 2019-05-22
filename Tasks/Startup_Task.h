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

/* Task function */
Void startupTaskFunc(UArg arg0, UArg arg1)
{
	/* Initialize ADC */
	ADC_init();

	/* Variable to hold ADC status */
	int_fast16_t res;

	while(1) {

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

			/* Check if cap is charged to less than 0.8V */
			if (adcValue0MicroVolt < 800000){
				/* If so, sleep the task for 1 minute to let the cap charge*/
				ADC_close(adc0);
				Task_sleep(6000000);
			}

			/* Check if cap is charged to more than 1.5 V */
			else if (adcValue0MicroVolt > 1500000){
				/* Turn on GPS and all other sensors */
				PIN_setOutputValue(pinHandle, IOID_21, 1);

				/* Burn charge until cap is at <1.5 V */
				while (adcValue0MicroVolt > 1500000) {
					/* Check capacitor charge */
					res = ADC_convert(adc0, &adcValue0);
					if (res == ADC_STATUS_SUCCESS) {
						adcValue0MicroVolt = ADC_convertRawToMicroVolts(adc0, adcValue0);
					}
					else {
//						Display_printf(display, 0, 0, "ADC channel 0 convert failed in loop\n");
					}

					/* Sleep for 1 second */
					Task_sleep(1000000);
				}

				/* Store capacitor state of charge */
				capacitor_charge = adcValue0;
				capacitor_charge_uv = adcValue0MicroVolt;

				/* Once at appropriate charge, close ADC channel 0 */
				ADC_close(adc0);

				/* Unlock GPS task */
				Semaphore_post(magLockSemaphoreHandle);


				/* Sleep for an hour (very long) */
				Task_sleep(360000000);
			}

			/* Otherwise, it has a charge between 0.8 and 1.5 V */
			else {

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
				Task_sleep(360000000);
			}


		}
		else {
//			Display_printf(display, 0, 0, "ADC channel 0 convert failed\n");
			ADC_close(adc0);
			Task_sleep(1000000);
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
