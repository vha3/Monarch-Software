/*
 * PWM_Initialization.h
 *
 *  Created on: Sep 26, 2017
 *      Author: hunteradams
 */

#ifndef PERIPHERALS_PWM_INITIALIZATION_H_
#define PERIPHERALS_PWM_INITIALIZATION_H_

/* Driver Header files */
#include <ti/drivers/PWM.h>

/* Example/Board Header files */
#include "Board.h"

/* Period and duty in microseconds */
uint16_t   pwmPeriod = 488;
uint16_t   pwmduty = 0;
uint16_t   pwmdutyInc = 1;
//int polarity = 1;

/* Sleep time in microseconds */
uint32_t   pwmtime = 50000;
PWM_Handle pwm1 = NULL;
//PWM_Handle pwm2 = NULL;
PWM_Params pwmparams;

void pwmSetup()
{
    PWM_Params_init(&pwmparams);
    pwmparams.dutyUnits = PWM_DUTY_US;
    pwmparams.dutyValue = 0;
    pwmparams.periodUnits = PWM_PERIOD_US;
    pwmparams.periodValue = pwmPeriod;

    pwm1 = PWM_open(Board_PWM2, &pwmparams);
    if (pwm1 == NULL) {
        /* Board_PWM0 did not open */
        while (1);
    }

//    pwm2 = PWM_open(Board_PWM3, &pwmparams);
//    if (pwm2 == NULL) {
//        /* Board_PWM0 did not open */
//        while (1);
//    }

    PWM_start(pwm1);
//    PWM_start(pwm2);
}



#endif /* PERIPHERALS_PWM_INITIALIZATION_H_ */
