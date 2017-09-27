/*
 * PWM_Tasks.h
 *
 *  Created on: Sep 26, 2017
 *      Author: hunteradams
 */

#ifndef TASKS_PWM_TASKS_H_
#define TASKS_PWM_TASKS_H_

#include <ti/sysbios/knl/Task.h>
#include "Semaphore_Initialization.h"
#include "Shared_Resources.h"
#include "Peripherals/PWM_Initialization.h"

Task_Struct pwmTask;

static uint8_t pwmTaskStack[300];

Void pwmTaskFunc(UArg arg0, UArg arg1)
{
	int polarity = 1;
    while (1) {
    		if(goodToGo){
    			if(polarity==1){
    				PWM_setDuty(pwm2, 0);
    				PWM_setDuty(pwm1, pwmduty);
    			}
    			else {
    				PWM_setDuty(pwm1, 0);
    				PWM_setDuty(pwm2, pwmduty);
    			}
    			pwmduty = (pwmduty + pwmdutyInc);

			if (pwmduty == pwmPeriod || (!pwmduty)) {
				pwmdutyInc = - pwmdutyInc;
			}

			if (pwmduty < 100){
				polarity = -polarity;
			}
    		}
    		Task_sleep(10000);
    }
}

void createPWMTask()
{
	Task_Params task_params;
	Task_Params_init(&task_params);
	task_params.stackSize = 300;
	task_params.priority = 1;
	task_params.stack = &pwmTaskStack;
	Task_construct(&pwmTask, pwmTaskFunc,
					   &task_params, NULL);
}



#endif /* TASKS_PWM_TASKS_H_ */
