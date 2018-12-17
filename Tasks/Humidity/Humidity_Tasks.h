/*
 * Humidity_Tasks.h
 *
 *  Created on: Oct 2, 2018
 *      Author: hunteradams
 */

#ifndef TASKS_HUMIDITY_HUMIDITY_TASKS_H_
#define TASKS_HUMIDITY_HUMIDITY_TASKS_H_


#include <ti/sysbios/knl/Task.h>
#include "../Semaphore_Initialization.h"
#include "../Shared_Resources.h"
#include "Si7021.h"

Task_Struct humidityTask;

static uint8_t humidityTaskStack[450];

Void humidityTaskFunc(UArg arg0, UArg arg1)
{
//	I2C_init();
//	Task_sleep(10000);
////    LSM9DS1init();
//    initI2C();
//    Task_sleep(10000);

    float degrees_F;
    float relative_humidity;
    int stopper = 0;

    while (1) {
    		Semaphore_pend(batonSemaphoreHandle, BIOS_WAIT_FOREVER);
    		if (goodToGo){
    			if (stopper == 0){
    				stopper += 1;
    				beginHumidity();
    				Task_sleep(1000);
				heaterOff();
				Task_sleep(1000);
    			}
    			else {
				degrees_F = getTempFarenheit();
//				Display_printf(display, 0, 0, "Temperature: %f deg F \n", degrees_F);

				relative_humidity = getRelativeHumidity();
//				Display_printf(display, 0, 0, "Relative Humidity: %f \n", relative_humidity);
    			}
    		}
    		Semaphore_post(batonSemaphoreHandle);
    		Task_sleep(10000);
    }
}


void createHumidityTask()
{
	Task_Params task_params;
	Task_Params_init(&task_params);
	task_params.stackSize = 450;
	task_params.priority = 2;
	task_params.stack = &humidityTaskStack;
	Task_construct(&humidityTask, humidityTaskFunc,
					   &task_params, NULL);
}


#endif /* TASKS_HUMIDITY_HUMIDITY_TASKS_H_ */
