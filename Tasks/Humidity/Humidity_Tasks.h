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

    while (1) {
    		Semaphore_pend(batonSemaphoreHandle, BIOS_WAIT_FOREVER);
    		Semaphore_post(lightsensorLockSemaphoreHandle);

    		temphumidityN += 1;
			PIN_setOutputValue(pinHandle, Board_PIN_LED0,
					!PIN_getOutputValue(Board_PIN_LED0));

    		Task_sleep(6000);
			raw_temp = getRawTemp();
			Display_printf(display, 0, 0, "T:  %x", raw_temp);
			Task_sleep(6000);
			raw_humidity = getRawHumidity();
			Display_printf(display, 0, 0, "H:  %x \n", raw_humidity);

//			degrees_F = getTempFarenheit();
//			relative_humidity = getRelativeHumidity();

			average_temp += (raw_temp >> 3);
			average_humidity += (raw_humidity >> 3);



//
//			Display_printf(display, 0, 0,
//							"Humidity: %f \n", relative_humidity);

			Semaphore_post(batonSemaphoreHandle);

			if (temphumidityN >= 8) {
				Semaphore_post(temphumidityDoneSemaphoreHandle);
				Task_sleep(360000000);
    		}
//    		SysCtrlSystemReset();

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
