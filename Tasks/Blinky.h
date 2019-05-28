/*
 * Blinky.h
 *
 *  Created on: May 23, 2019
 *      Author: hunteradams
 */

#ifndef TASKS_BLINKY_H_
#define TASKS_BLINKY_H_

#include <ti/sysbios/knl/Task.h>
#include "Semaphore_Initialization.h"
#include "Shared_Resources.h"

Task_Struct blinkTask;

static uint8_t blinkTaskStack[300];

Void blinkTaskFunc(UArg arg0, UArg arg1)
{
	// Only runs once (initializations)




    while (1) {
    	PIN_setOutputValue(pinHandle, Board_PIN_LED1, 0);
    	Display_printf(display, 0, 0, "Hello world!");
    	Task_sleep(100000);
    	PIN_setOutputValue(pinHandle, Board_PIN_LED1, 1);
    	Task_sleep(100000);
    }
}

void createBlinkTask()
{
	Task_Params task_params;
	Task_Params_init(&task_params);
	task_params.stackSize = 300;
	task_params.priority = 1;
	task_params.stack = &blinkTaskStack;
	Task_construct(&blinkTask, blinkTaskFunc,
					   &task_params, NULL);
}



#endif /* TASKS_BLINKY_H_ */
