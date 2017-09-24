/*
 * Calibration_Task.h
 *
 *  Created on: Sep 24, 2017
 *      Author: hunteradams
 */

#ifndef TASKS_CALIBRATION_TASK_H_
#define TASKS_CALIBRATION_TASK_H_

#include "LSM9DS1.h"
#include "Shared_Resources.h"
#include <ti/sysbios/knl/Task.h>

Task_Struct calibrationTask;
static uint8_t calibrationTaskStack[450];

Void calibrationTaskFunc(UArg arg0, UArg arg1)
{
	while (1) {
		Semaphore_pend(calibSemaphoreHandle, BIOS_WAIT_FOREVER);
		calibrate(1);
//		calibrateMag(1);
		/* getMagInitial is only required if you're calibrating for the computer attitude */
		getMagInitial();
		goodToGo += 1;
		readGyro();
		readAccel();
		readMag();
	}
}

void createCalibrationTask()
{
    Task_Params task_params;
    Task_Params_init(&task_params);
    task_params.stackSize = 450;
    task_params.priority = 4;
    task_params.stack = &calibrationTaskStack;
    Task_construct(&calibrationTask, calibrationTaskFunc,
    		           &task_params, NULL);
}



#endif /* TASKS_CALIBRATION_TASK_H_ */
