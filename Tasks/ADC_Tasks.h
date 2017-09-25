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

Task_Struct adc1Task;
Task_Struct adc0Task;

static uint8_t adc1TaskStack[450];
static uint8_t adc0TaskStack[450];


/* ADC conversion result variables */
uint16_t adcValue1;
uint32_t adcValue1MicroVolt;

uint16_t adcValue0;
uint32_t adcValue0MicroVolt;

Void adc1TaskFunc(UArg arg0, UArg arg1)
{
	int_fast16_t res;
    while (1) {
    		if(goodToGo){
    			res = ADC_convert(adc1, &adcValue1);
    			if (res == ADC_STATUS_SUCCESS) {

				adcValue1MicroVolt = ADC_convertRawToMicroVolts(adc1, adcValue1);

				Display_printf(display, 0, 0,
						"ADC channel 1 convert result: %d uV\n", adcValue1MicroVolt);
			}
			else {
				Display_printf(display, 0, 0, "ADC channel 1 convert failed\n");
			}
    		}
    		Task_yield();
    }
}

Void adc0TaskFunc(UArg arg0, UArg arg1)
{
	int_fast16_t res;
    while (1) {
    		if(goodToGo){
    			res = ADC_convert(adc0, &adcValue0);
    			if (res == ADC_STATUS_SUCCESS) {

				adcValue0MicroVolt = ADC_convertRawToMicroVolts(adc0, adcValue0);

				Display_printf(display, 0, 0,
						"ADC channel 0 convert result: %d uV\n", adcValue0MicroVolt);
			}
			else {
				Display_printf(display, 0, 0, "ADC channel 0 convert failed\n");
			}
    		}
    		Task_yield();
    }
}

void createADC1Task()
{
	Task_Params task_params;
	Task_Params_init(&task_params);
	task_params.stackSize = 450;
	task_params.priority = 1;
	task_params.stack = &adc1TaskStack;
	Task_construct(&adc1Task, adc1TaskFunc,
					   &task_params, NULL);
}

void createADC0Task()
{
	Task_Params task_params;
	Task_Params_init(&task_params);
	task_params.stackSize = 450;
	task_params.priority = 1;
	task_params.stack = &adc0TaskStack;
	Task_construct(&adc0Task, adc0TaskFunc,
					   &task_params, NULL);
}



#endif /* TASKS_ADC_TASKS_H_ */
