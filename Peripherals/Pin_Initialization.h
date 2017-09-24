/*
 * Pin_Initialization.h
 *
 *  Created on: Sep 24, 2017
 *      Author: hunteradams
 */

#ifndef PERIPHERALS_PIN_INITIALIZATION_H_
#define PERIPHERALS_PIN_INITIALIZATION_H_

#include <ti/drivers/PIN.h>
#include <ti/drivers/pin/PINCC26XX.h>

/* Example/Board Header files */
#include "Board.h"

/* Pin handles and states*/
static PIN_Handle pinHandle;
static PIN_State pinState;

/*
 * Application button pin configuration table:
 *   - Interrupts are configured to trigger on rising edge.
 */
PIN_Config pinTable[] = {
	CC1310_LAUNCHXL_DIO15  | PIN_INPUT_EN | PIN_PULLDOWN | PIN_IRQ_POSEDGE,
	CC1310_LAUNCHXL_DIO12  | PIN_INPUT_EN | PIN_PULLDOWN | PIN_IRQ_POSEDGE,
	CC1310_LAUNCHXL_DIO22  | PIN_INPUT_EN | PIN_PULLDOWN | PIN_IRQ_POSEDGE,
	CC1310_LAUNCHXL_PIN_RLED | PIN_GPIO_OUTPUT_EN | PIN_GPIO_LOW | PIN_PUSHPULL | PIN_DRVSTR_MAX,
	CC1310_LAUNCHXL_PIN_GLED | PIN_GPIO_OUTPUT_EN | PIN_GPIO_LOW | PIN_PUSHPULL | PIN_DRVSTR_MAX,
    PIN_TERMINATE
};

void pinCallback(PIN_Handle handle, PIN_Id pinId) {
//    uint32_t currVal = 0;
	switch (pinId) {
		case CC1310_LAUNCHXL_DIO12:
//			currVal =  PIN_getOutputValue(Board_PIN_LED0);
//			PIN_setOutputValue(pinHandle, Board_PIN_LED0, !currVal);
			Semaphore_post(gyroSemaphoreHandle);
			break;

		case CC1310_LAUNCHXL_DIO15:
//			currVal =  PIN_getOutputValue(Board_PIN_LED1);
//			PIN_setOutputValue(pinHandle, Board_PIN_LED1, !currVal);
			Semaphore_post(magSemaphoreHandle);
			break;

		case CC1310_LAUNCHXL_DIO22:
//			currVal =  PIN_getOutputValue(Board_PIN_LED1);
//			PIN_setOutputValue(pinHandle, Board_PIN_LED1, !currVal);
			Semaphore_post(accelSemaphoreHandle);
			break;

		default:
			/* Do nothing */
			break;
	}
}

void pinSetup()
{
    pinHandle = PIN_open(&pinState, pinTable);
	if(!pinHandle) {
		/* Error initializing button pins */
		while(1);
	}

    /* Setup callback for button pins */
    if (PIN_registerIntCb(pinHandle, &pinCallback) != 0) {
        /* Error registering button callback function */
        while(1);
    }
}



#endif /* PERIPHERALS_PIN_INITIALIZATION_H_ */
