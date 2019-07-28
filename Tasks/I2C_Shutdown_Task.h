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

	Semaphore_pend(i2cDoneSemaphoreHandle, BIOS_WAIT_FOREVER);
	Semaphore_pend(lightsensorDoneSemaphoreHandle, BIOS_WAIT_FOREVER);

//	Watchdog_clear(watchdogHandle);
//	Watchdog_close(watchdogHandle);

//	PIN_setOutputValue(pinHandle, IOID_21, 0);
	PIN_setOutputValue(pinHandle, Board_PIN_LED0,0);
	PIN_setOutputValue(pinHandle, Board_PIN_LED1,0);

//	I2C_close(i2c);

	Display_printf(display, 0, 0, "AX: %x", tx_ax);
	Display_printf(display, 0, 0, "AY: %x", tx_ay);
	Display_printf(display, 0, 0, "AZ: %x", tx_az);
	Display_printf(display, 0, 0, "GX: %x", tx_gx);
	Display_printf(display, 0, 0, "GY: %x", tx_gy);
	Display_printf(display, 0, 0, "GZ: %x", tx_gz);
	Display_printf(display, 0, 0, "MX: %x", tx_mx);
	Display_printf(display, 0, 0, "MY: %x", tx_my);
	Display_printf(display, 0, 0, "MZ: %x", tx_mz);
	Display_printf(display, 0, 0, "T:  %x", tx_temp);
	Display_printf(display, 0, 0, "H:  %x", tx_humidity);
	Display_printf(display, 0, 0, "LU  %x", tx_light_top);
	Display_printf(display, 0, 0, "LD: %x", tx_light_bottom);
	Display_printf(display, 0, 0, "C:  %x \n", capacitor_charge);

	Display_close(display);

	Semaphore_post(gpsLockSemaphoreHandle);
//	Semaphore_post(txDataSemaphoreHandle);

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
