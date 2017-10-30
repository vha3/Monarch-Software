/*
 * Clock_Initialization.h
 *
 *  Created on: Sep 24, 2017
 *      Author: hunteradams
 */

#ifndef PERIPHERALS_CLOCK_INITIALIZATION_H_
#define PERIPHERALS_CLOCK_INITIALIZATION_H_

#include <ti/sysbios/knl/Clock.h>
#include "../Tasks/Semaphore_Initialization.h"
#include "../Tasks/Shared_Resources.h"
#include "Peripherals/Watchdog_Initialization.h"
#include "Pin_Initialization.h"

#define Clock_convertSecondsToTicks(seconds) (((seconds) * 1000000) / (Clock_tickPeriod))


/* Stuff related to clock */
Void clk0Fxn(UArg arg0);
Clock_Struct clk0Struct;
Clock_Handle clk0Handle;

Void clk1Fxn(UArg arg0);
Clock_Struct clk1Struct;
Clock_Handle clk1Handle;


/* Some stuff for simulating nav indicator */
Void clk0Fxn(UArg arg0)
{
	if(goodToGo){
		Watchdog_clear(watchdogHandle);
		Semaphore_post(txDataSemaphoreHandle);
	}
}

Void clk1Fxn(UArg arg0)
{
	if(GPSorRX) {
		GPSorRX = 0;
		PIN_setOutputValue(pinHandle, IOID_15, 1);
	}
	else {
		GPSorRX = 1;
		PIN_setOutputValue(pinHandle, IOID_15, 0);
	}
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

	clkParams.period = Clock_convertSecondsToTicks(60);
	clkParams.startFlag = TRUE;
	Clock_construct(&clk1Struct, (Clock_FuncPtr)clk1Fxn,
			Clock_convertSecondsToTicks(60), &clkParams);
}





#endif /* PERIPHERALS_CLOCK_INITIALIZATION_H_ */
