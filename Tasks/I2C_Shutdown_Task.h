/*
 * I2C_Shutdown_Task.h
 *
 *  Created on: May 21, 2019
 *      Author: hunteradams
 */

#ifndef TASKS_I2C_SHUTDOWN_TASK_H_
#define TASKS_I2C_SHUTDOWN_TASK_H_

#include <ti/sysbios/knl/Task.h>
#include "Semaphore_Initialization.h"
#include "Shared_Resources.h"

Task_Struct i2cShutdownTask;

static uint8_t i2cShutdownTaskStack[800];

Void i2cShutdownTaskFunc(UArg arg0, UArg arg1)
{

	Semaphore_pend(magDoneSemaphoreHandle, BIOS_WAIT_FOREVER);
	Semaphore_pend(gyroDoneSemaphoreHandle, BIOS_WAIT_FOREVER);
	Semaphore_pend(accelDoneSemaphoreHandle, BIOS_WAIT_FOREVER);
	Semaphore_pend(temphumidityDoneSemaphoreHandle, BIOS_WAIT_FOREVER);
	Semaphore_pend(lightsensorDoneSemaphoreHandle, BIOS_WAIT_FOREVER);

	Watchdog_clear(watchdogHandle);
	Watchdog_close(watchdogHandle);

	I2C_close(i2c);
//	Display_printf(display, 0, 0,
//						"Avg. light 1: %d \n", average_light_top_uv);
//	Display_printf(display, 0, 0,
//						"Avg. light 2: %d \n", average_light_bottom_uv);
	Display_printf(display, 0, 0, "AX: %x", average_ax);
	Display_printf(display, 0, 0, "AY: %x", average_ay);
	Display_printf(display, 0, 0, "AZ: %x", average_az);
	Display_printf(display, 0, 0, "GX: %x", average_gx);
	Display_printf(display, 0, 0, "GY: %x", average_gy);
	Display_printf(display, 0, 0, "GZ: %x", average_gz);
	Display_printf(display, 0, 0, "MX: %x", average_mx);
	Display_printf(display, 0, 0, "MY: %x", average_my);
	Display_printf(display, 0, 0, "MZ: %x", average_mz);
	Display_printf(display, 0, 0, "T:  %x", average_temp);
	Display_printf(display, 0, 0, "H:  %x", average_humidity);
	Display_printf(display, 0, 0, "LU  %x", average_light_top);
	Display_printf(display, 0, 0, "LD: %x", average_light_bottom);
	Display_printf(display, 0, 0, "C:  %x", capacitor_charge);

	Display_close(display);
	PIN_setOutputValue(pinHandle, IOID_21, 0);
	PIN_setOutputValue(pinHandle, Board_PIN_LED0,0);
	PIN_setOutputValue(pinHandle, Board_PIN_LED1,0);
	Task_sleep(360000000);

}

void createi2cShutdownTask()
{
	Task_Params task_params;
	Task_Params_init(&task_params);
	task_params.stackSize = 800;
	task_params.priority = 1;
	task_params.stack = &i2cShutdownTaskStack;
	Task_construct(&i2cShutdownTask, i2cShutdownTaskFunc,
					   &task_params, NULL);
}



#endif /* TASKS_I2C_SHUTDOWN_TASK_H_ */
