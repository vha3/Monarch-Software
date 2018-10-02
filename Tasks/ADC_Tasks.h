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

Task_Struct adcTask;

static uint8_t adcTaskStack[400];


/* ADC conversion result variables */
uint16_t adcValue1;
uint32_t adcValue1MicroVolt;

uint16_t adcValue0;
uint32_t adcValue0MicroVolt;

Void adcTaskFunc(UArg arg0, UArg arg1)
{
	ADC_init();
	adcSetup();

	int_fast16_t res;
    while (1) {
//    		if(goodToGo){
    			res = ADC_convert(adc1, &adcValue1);
    			PIN_setOutputValue(pinHandle, Board_PIN_LED1, !PIN_getOutputValue(Board_PIN_LED1));
    			if (res == ADC_STATUS_SUCCESS) {

				adcValue1MicroVolt = ADC_convertRawToMicroVolts(adc1, adcValue1);

//				Display_printf(display, 0, 0,
//						"ADC channel 1 convert result: %d uV\n", adcValue1MicroVolt);
			}
			else {
				Display_printf(display, 0, 0, "ADC channel 1 convert failed\n");
			}

    			res = ADC_convert(adc0, &adcValue0);
    			if (res == ADC_STATUS_SUCCESS) {

				adcValue0MicroVolt = ADC_convertRawToMicroVolts(adc0, adcValue0);

//				Display_printf(display, 0, 0,
//						"ADC channel 0 convert result: %d uV\n", adcValue0MicroVolt);
			}
			else {
				Display_printf(display, 0, 0, "ADC channel 0 convert failed\n");
			}
//    		}
    		/* 10 Hz */
    		Task_sleep(10000);
    }
}

void createADCTask()
{
	Task_Params task_params;
	Task_Params_init(&task_params);
	task_params.stackSize = 400;
	task_params.priority = 1;
	task_params.stack = &adcTaskStack;
	Task_construct(&adcTask, adcTaskFunc,
					   &task_params, NULL);
}



#endif /* TASKS_ADC_TASKS_H_ */
