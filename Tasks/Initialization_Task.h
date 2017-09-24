/*
 * Initialization_Task.h
 *
 *  Created on: Sep 24, 2017
 *      Author: hunteradams
 */

#ifndef TASKS_INITIALIZATION_TASK_H_
#define TASKS_INITIALIZATION_TASK_H_

#include <ti/sysbios/knl/Task.h>
#include "IMU/LSM9DS1.h"

Task_Struct initializationTask;
static uint8_t initializationTaskStack[400];

Void initializationTaskFunc(UArg arg0, UArg arg1)
{
    while (1) {
    		Semaphore_pend(initSemaphoreHandle,BIOS_WAIT_FOREVER);

        uint16_t workpls = LSM9DS1begin();
        configInt(XG_INT1, INT_DRDY_G, INT_ACTIVE_HIGH, INT_PUSH_PULL);
        configInt(XG_INT2, INT_DRDY_XL, INT_ACTIVE_HIGH, INT_PUSH_PULL);

        Semaphore_post(calibSemaphoreHandle);
    }
}

void createInitializationTask()
{
    Task_Params task_params;
    Task_Params_init(&task_params);
    task_params.stackSize = 400;
    task_params.priority = 4;
    task_params.stack = &initializationTaskStack;
    Task_construct(&initializationTask, initializationTaskFunc,
    		           &task_params, NULL);
}



#endif /* TASKS_INITIALIZATION_TASK_H_ */
