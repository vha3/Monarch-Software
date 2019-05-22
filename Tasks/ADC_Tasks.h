/*
 * ADC_Tasks.h
 *
 *  Created on: Sep 25, 2017
 *      Author: hunteradams
 */

#ifndef TASKS_ADC_TASKS_H_
#define TASKS_ADC_TASKS_H_

#include <ti/sysbios/knl/Task.h>
#include "Semaphore_Initialization.h"
#include "Shared_Resources.h"

/* Task struct */
Task_Struct adcTask;

/* Task memory allocation */
static uint8_t adcTaskStack[400];

/* Variables for holding measurements */
uint16_t adcValue1;
uint32_t adcValue1MicroVolt;
uint16_t adcValue3;
uint32_t adcValue3MicroVolt;

int lightsensorN = 0;

Void adcTaskFunc(UArg arg0, UArg arg1)
{
	Semaphore_pend(lightsensorLockSemaphoreHandle, BIOS_WAIT_FOREVER);

	adc1Setup();
	adc3Setup();

	Task_sleep(1000);

	int_fast16_t res;

    while (1) {

    		Semaphore_pend(batonSemaphoreHandle, BIOS_WAIT_FOREVER);
    		lightsensorN += 1;

    		PIN_setOutputValue(pinHandle, Board_PIN_LED1,
    				!PIN_getOutputValue(Board_PIN_LED1));

    		res = ADC_convert(adc1, &adcValue1);
    		if (res == ADC_STATUS_SUCCESS) {

				adcValue1MicroVolt = ADC_convertRawToMicroVolts(adc1, adcValue1);
				average_light_top += (adcValue1 >> 3);
				average_light_top_uv += (adcValue1MicroVolt >> 3);

//				Display_printf(display, 0, 0,
//						"ADC channel 1 convert result: %d uV\n", adcValue1MicroVolt);
			}
			else {
//				Display_printf(display, 0, 0, "ADC channel 1 convert failed\n");
			}

    		res = ADC_convert(adc3, &adcValue3);
    		if (res == ADC_STATUS_SUCCESS) {

				adcValue3MicroVolt = ADC_convertRawToMicroVolts(adc3, adcValue3);
				average_light_bottom += (adcValue3 >> 3);
				average_light_bottom_uv += (adcValue3MicroVolt >> 3);

//				Display_printf(display, 0, 0,
//						"ADC channel 3 convert result: %d uV\n", adcValue3MicroVolt);
			}
			else {
//				Display_printf(display, 0, 0, "ADC channel 1 convert failed\n");
			}

    		Display_printf(display, 0, 0, "LU: %x", adcValue1);
    		Display_printf(display, 0, 0, "LD: %x \n", adcValue3);

    		if (lightsensorN >= 8) {
    			ADC_close(adc1);
    			ADC_close(adc3);
				Semaphore_post(lightsensorDoneSemaphoreHandle);
				Task_sleep(360000000);
    		}

    		/* 10 Hz */
    		Semaphore_post(batonSemaphoreHandle);
    		Task_sleep(10000);
    }
}

void createADCTask()
{
	Task_Params task_params;
	Task_Params_init(&task_params);
	task_params.stackSize = 400;
	task_params.priority = 2;
	task_params.stack = &adcTaskStack;
	Task_construct(&adcTask, adcTaskFunc,
					   &task_params, NULL);
}



#endif /* TASKS_ADC_TASKS_H_ */
