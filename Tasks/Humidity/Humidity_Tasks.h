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

Void humidityTaskFunc(UArg arg0, UArg arg1)
{

	Semaphore_pend(humidityLockSemaphoreHandle, BIOS_WAIT_FOREVER);

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
    				Watchdog_clear(watchdogHandle);
    				Watchdog_close(watchdogHandle);
    				PIN_setOutputValue(pinHandle, Board_PIN_LED0,!PIN_getOutputValue(Board_PIN_LED0));
    				stopper += 1;
					degrees_F = getTempFarenheit();
//					Display_printf(display, 0, 0, "Temperature: %f deg F \n", degrees_F);

					relative_humidity = getRelativeHumidity();
//					Display_printf(display, 0, 0, "Relative Humidity: %f \n", relative_humidity);
//
//					Display_printf(display, 0, 0, "Cound: %d \n", stopper);
    			}
    		}

    		if (stopper > 100){

    			PIN_setOutputValue(pinHandle, IOID_21,0);
    			PIN_setOutputValue(pinHandle, Board_PIN_LED0,0);
    			PIN_setOutputValue(pinHandle, Board_PIN_LED1,0);
    			UART_readCancel(uart);
    			UART_writeCancel(uart);
    			I2C_close(i2c);
    			halt += 1;
    			PIN_close(&pinState);

//    			Task_sleep(600000);
//    			PIN_init(pinTable2);
//    			pinSetup2();
//    			pinHandle = PIN_open(&pinState, pinTable2);
//    			if(!pinHandle) {
//					/* Error initializing button pins */
//    				PIN_setOutputValue(pinHandle, Board_PIN_LED0,1);
//    				PIN_setOutputValue(pinHandle, Board_PIN_LED1,1);
//					while(1);
//				}
//    			Display_close(display);
//    			halt += 1;
//    			PIN_setOutputValue(pinHandle2, CC1310_LAUNCHXL_I2C0_SCL0,0);
//    			PIN_setOutputValue(pinHandle2, CC1310_LAUNCHXL_I2C0_SDA0,0);
//    			GPIO_write(IOID_4, 0);
//    			GPIO_write(IOID_5, 0);
//    			PIN_close(&pinState2);
    			Semaphore_post(readSemaphoreHandle);
    			Semaphore_post(batonSemaphoreHandle);
    			Task_sleep(6000000);
    			SysCtrlSystemReset();
    		}
    		Semaphore_post(batonSemaphoreHandle);
    		Semaphore_post(txDataSemaphoreHandle);
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
