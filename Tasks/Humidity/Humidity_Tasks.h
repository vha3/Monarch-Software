/*
 * Humidity_Tasks.h
 *
 *  Created on: Oct 2, 2018
 *      Author: hunteradams
 */

#ifndef TASKS_HUMIDITY_HUMIDITY_TASKS_H_
#define TASKS_HUMIDITY_HUMIDITY_TASKS_H_

#include <ti/devices/cc13x0/driverlib/sys_ctrl.h>


#include <ti/sysbios/knl/Task.h>
#include "../Semaphore_Initialization.h"
#include "../Shared_Resources.h"
#include "Si7021.h"

Task_Struct humidityTask;

static uint8_t humidityTaskStack[450];

uint16_t raw_temp;
uint16_t raw_humidity;
//float degrees_F;
//float relative_humidity;

int temphumidityN = 0;

Void humidityTaskFunc(UArg arg0, UArg arg1)
{

	Semaphore_pend(humidityLockSemaphoreHandle, BIOS_WAIT_FOREVER);

	Semaphore_pend(batonSemaphoreHandle, BIOS_WAIT_FOREVER);
	beginHumidity();
	Task_sleep(1000);
	heaterOff();
	Task_sleep(1000);
	Semaphore_post(batonSemaphoreHandle);

    while (1) {
    		Semaphore_pend(batonSemaphoreHandle, BIOS_WAIT_FOREVER);

    		temphumidityN += 1;

			raw_temp = getRawTemp();
			raw_humidity = getRawHumidity();
//			degrees_F = getTempFarenheit();
//			relative_humidity = getRelativeHumidity();

//			Display_printf(display, 0, 0,
//							"Temp: %d \n", raw_temp);
//
//			Display_printf(display, 0, 0,
//							"Humidity: %d \n", raw_humidity);

			Semaphore_post(batonSemaphoreHandle);

			if (temphumidityN > 20) {
				Semaphore_post(temphumidityDoneSemaphoreHandle);
				Task_sleep(360000000);
    		}
//    		SysCtrlSystemReset();
    		Task_sleep(10000);
    }
}


void createHumidityTask()
{
	Task_Params task_params;
	Task_Params_init(&task_params);
	task_params.stackSize = 450;
	task_params.priority = 1;
	task_params.stack = &humidityTaskStack;
	Task_construct(&humidityTask, humidityTaskFunc,
					   &task_params, NULL);
}


#endif /* TASKS_HUMIDITY_HUMIDITY_TASKS_H_ */
