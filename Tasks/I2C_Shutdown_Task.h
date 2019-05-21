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

	I2C_close(i2c);
	Display_printf(display, 0, 0,"Done! \n");
	Display_close(display);
	PIN_setOutputValue(pinHandle, IOID_21, 0);
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
