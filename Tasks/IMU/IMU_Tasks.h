/*
 * IMU_Tasks.h
 *
 *  Created on: Sep 24, 2017
 *      Author: hunteradams
 */

#ifndef TASKS_IMU_IMU_TASKS_H_
#define TASKS_IMU_IMU_TASKS_H_

#include <ti/sysbios/knl/Task.h>
#include "../Semaphore_Initialization.h"
#include "../Shared_Resources.h"
#include "LSM9DS1.h"

Task_Struct magTask;
Task_Struct gyroTask;
Task_Struct accelTask;

static uint8_t magTaskStack[450];
static uint8_t gyroTaskStack[450];
static uint8_t accelTaskStack[450];

Void magTaskFunc(UArg arg0, UArg arg1)
{
    while (1) {
    		Semaphore_pend(magSemaphoreHandle, BIOS_WAIT_FOREVER);
    		Semaphore_pend(batonSemaphoreHandle, BIOS_WAIT_FOREVER);
    		if(goodToGo){
    			readMag();
    		}
    		Semaphore_post(batonSemaphoreHandle);
    }
}

Void gyroTaskFunc(UArg arg0, UArg arg1)
{
    while (1) {
    		Semaphore_pend(gyroSemaphoreHandle, BIOS_WAIT_FOREVER);
    		Semaphore_pend(batonSemaphoreHandle, BIOS_WAIT_FOREVER);
    		if(goodToGo){
    			readGyro();
    		}
    		Semaphore_post(batonSemaphoreHandle);
    }
}

Void accelTaskFunc(UArg arg0, UArg arg1)
{
    while (1) {
    		Semaphore_pend(accelSemaphoreHandle, BIOS_WAIT_FOREVER);
    		Semaphore_pend(batonSemaphoreHandle, BIOS_WAIT_FOREVER);
    		if(goodToGo){
    			readAccel();
    		}
    		Semaphore_post(batonSemaphoreHandle);
    }
}

void createIMUTasks()
{
	Task_Params task_params;
	Task_Params_init(&task_params);

	task_params.stackSize = 450;
	task_params.priority = 2;
	task_params.stack = &magTaskStack;
	Task_construct(&magTask, magTaskFunc,
					   &task_params, NULL);

	task_params.stackSize = 450;
	task_params.priority = 2;
	task_params.stack = &gyroTaskStack;
	Task_construct(&gyroTask, gyroTaskFunc,
					   &task_params, NULL);

	task_params.stackSize = 450;
	task_params.priority = 2;
	task_params.stack = &accelTaskStack;
	Task_construct(&accelTask, accelTaskFunc,
					   &task_params, NULL);
}




#endif /* TASKS_IMU_IMU_TASKS_H_ */
