/*
 * Watchdog_Initialization.h
 *
 *  Created on: Sep 24, 2017
 *      Author: hunteradams
 */

#ifndef PERIPHERALS_WATCHDOG_INITIALIZATION_H_
#define PERIPHERALS_WATCHDOG_INITIALIZATION_H_

#include <ti/drivers/Watchdog.h>
#include "Board.h"

Watchdog_Handle watchdogHandle;
void wdtSetup()
{
	Watchdog_Params params;
	/* Call board init functions */
	Watchdog_init();
	/* Create and enable a Watchdog with resets disabled */
	Watchdog_Params_init(&params);
	params.resetMode = Watchdog_RESET_ON; //generate Reboot interrupt
	watchdogHandle = Watchdog_open(Board_WATCHDOG0, &params);
	Watchdog_setReload(watchdogHandle, Watchdog_convertMsToTicks(watchdogHandle, 2500)); //set WDT period
}


#endif /* PERIPHERALS_WATCHDOG_INITIALIZATION_H_ */
