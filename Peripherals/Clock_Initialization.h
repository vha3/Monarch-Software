/*
 * Clock_Initialization.h
 *
 *  Created on: Sep 24, 2017
 *      Author: hunteradams
 */

#ifndef PERIPHERALS_CLOCK_INITIALIZATION_H_
#define PERIPHERALS_CLOCK_INITIALIZATION_H_

#include <ti/sysbios/knl/Clock.h>
//#include "../Tasks/Semaphore_Initialization.h"
//#include "../Tasks/Shared_Resources.h"
//#include "Peripherals/Watchdog_Initialization.h"
#include "Pin_Initialization.h"
//#include "PWM_Initialization.h"

#define Clock_convertSecondsToTicks(seconds) (((seconds) * 1000000) / (Clock_tickPeriod))


/* Stuff related to clock */
Void clk0Fxn(UArg arg0);
Clock_Struct clk0Struct;
Clock_Handle clk0Handle;


/* Some stuff for simulating nav indicator */
Void clk0Fxn(UArg arg0)
{
//		Watchdog_clear(watchdogHandle);
		PIN_setOutputValue(pinHandle, Board_PIN_LED1, !PIN_getOutputValue(Board_PIN_LED1));
//		Semaphore_post(txDataSemaphoreHandle);
}


void clockSetup()
{
	Clock_Params clkParams;
	Clock_Params_init(&clkParams);

	clkParams.period = 500000/Clock_tickPeriod;
	clkParams.startFlag = TRUE;
	/* Construct a periodic Clock Instance */
	Clock_construct(&clk0Struct, (Clock_FuncPtr)clk0Fxn,
					5000/Clock_tickPeriod, &clkParams);
}





#endif /* PERIPHERALS_CLOCK_INITIALIZATION_H_ */
