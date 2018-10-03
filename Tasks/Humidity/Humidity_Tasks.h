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
	I2C_init();
//    LSM9DS1init();
    initI2C();
    uint16_t measurement;
    float degrees_C;
    float degrees_F;

    uint16_t rel_hum_measurement;
    float relative_humidity;

    beginHumidity();

    while (1) {

//    		Display_printf(display, 0, 0, "Bang\n");
    		PIN_setOutputValue(pinHandle, Board_PIN_LED0, !PIN_getOutputValue(Board_PIN_LED0));
    		measurement = I2Cwrite1read2HumByte(ADDRESS, 0xE3);
    		degrees_C = (175.72*measurement/65536)-46.85;
    		degrees_F = (degrees_C*1.8) + 32;
    		Display_printf(display, 0, 0, "Temperature: %f deg F \n", degrees_F);

    		rel_hum_measurement = I2Cwrite1read2HumByte(ADDRESS, 0xE5);
		relative_humidity = (125.0*rel_hum_measurement/65536)-6;
		Display_printf(display, 0, 0, "Relative Humidity: %f \n", relative_humidity);



    		Task_sleep(10000);
//    		beginHumidity();
//    		Semaphore_pend(magSemaphoreHandle, BIOS_WAIT_FOREVER);
//    		Semaphore_pend(batonSemaphoreHandle, BIOS_WAIT_FOREVER);
//    		if(goodToGo){
//    			readMag();
//    			Watchdog_clear(watchdogHandle);
//    			Display_printf(display, 0, 0,
//    									"Magnetometer X: %d \n", mx);
//    			Display_printf(display, 0, 0,
//    			    						"Magnetometer Y: %d \n", my);
//    			Display_printf(display, 0, 0,
//    			    						"Magnetometer Z: %d \n", mz);
//    		}
//    		Semaphore_post(batonSemaphoreHandle);
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
