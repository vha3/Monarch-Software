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
#include "Watchdog_Initialization.h"
#include "Clock_Initialization.h"

/* Example/Board Header files */
#include "Board.h"

/* Pin handles and states*/
static PIN_Handle pinHandle;
static PIN_State pinState;

static PIN_Handle pinHandle2;
static PIN_State pinState2;

/*
 * Application button pin configuration table:
 *   - Interrupts are configured to trigger on rising edge.
 */
PIN_Config pinTable[] = {
	IOID_14  | PIN_INPUT_EN | PIN_PULLUP | PIN_IRQ_POSEDGE,
	CC1310_LAUNCHXL_DIO12  | PIN_INPUT_EN | PIN_PULLUP | PIN_IRQ_NEGEDGE,
	IOID_13  | PIN_INPUT_EN | PIN_PULLUP | PIN_IRQ_NEGEDGE,
	IOID_1 | PIN_INPUT_EN | PIN_PULLUP | PIN_IRQ_POSEDGE,
	IOID_21 | PIN_GPIO_OUTPUT_EN | PIN_GPIO_LOW | PIN_PUSHPULL | PIN_DRVSTR_MAX,
	CC1310_LAUNCHXL_PIN_RLED | PIN_GPIO_OUTPUT_EN | PIN_GPIO_LOW | PIN_PUSHPULL | PIN_DRVSTR_MAX,
	CC1310_LAUNCHXL_PIN_GLED | PIN_GPIO_OUTPUT_EN | PIN_GPIO_LOW | PIN_PUSHPULL | PIN_DRVSTR_MAX,
    PIN_TERMINATE
};

PIN_Config pinTable2[] = {
	IOID_14  | PIN_INPUT_EN | PIN_PULLDOWN | PIN_IRQ_DIS,
	CC1310_LAUNCHXL_DIO12  | PIN_INPUT_EN | PIN_PULLDOWN | PIN_IRQ_DIS,
	IOID_13  | PIN_INPUT_EN | PIN_PULLDOWN | PIN_IRQ_DIS,
	IOID_1 | PIN_INPUT_EN | PIN_PULLDOWN | PIN_IRQ_DIS,
	IOID_21 | PIN_GPIO_OUTPUT_EN | PIN_GPIO_LOW | PIN_PUSHPULL | PIN_DRVSTR_MAX,
	CC1310_LAUNCHXL_PIN_RLED | PIN_GPIO_OUTPUT_EN | PIN_GPIO_LOW | PIN_PUSHPULL | PIN_DRVSTR_MAX,
	CC1310_LAUNCHXL_PIN_GLED | PIN_GPIO_OUTPUT_EN | PIN_GPIO_LOW | PIN_PUSHPULL | PIN_DRVSTR_MAX,
//	CC1310_LAUNCHXL_I2C0_SCL0  | PIN_GPIO_OUTPUT_EN | PIN_GPIO_LOW | PIN_PUSHPULL | PIN_DRVSTR_MAX,
//	CC1310_LAUNCHXL_I2C0_SDA0  | PIN_GPIO_OUTPUT_EN | PIN_GPIO_LOW | PIN_PUSHPULL | PIN_DRVSTR_MAX,
//	CC1310_LAUNCHXL_I2C0_SCL0  | PIN_INPUT_EN | PIN_PULLDOWN | PIN_IRQ_DIS,
//	CC1310_LAUNCHXL_I2C0_SDA0  | PIN_INPUT_EN | PIN_PULLDOWN | PIN_IRQ_DIS,
    PIN_TERMINATE
};

void pinCallback(PIN_Handle handle, PIN_Id pinId) {
//    uint32_t currVal = 0;
	switch (pinId) {
		case CC1310_LAUNCHXL_DIO12:
//			currVal =  PIN_getOutputValue(Board_PIN_LED0);
//			PIN_setOutputValue(pinHandle, Board_PIN_LED0, !currVal);
			Semaphore_post(accelSemaphoreHandle);
			break;

		case IOID_14:
//			currVal =  PIN_getOutputValue(Board_PIN_LED1);
//			PIN_setOutputValue(pinHandle, Board_PIN_LED1, !currVal);
			Semaphore_post(magSemaphoreHandle);
			break;

		case IOID_13:
//			currVal =  PIN_getOutputValue(Board_PIN_LED0);
//			PIN_setOutputValue(pinHandle, Board_PIN_LED0, !currVal);
			Semaphore_post(gyroSemaphoreHandle);
			break;

		case IOID_1:
//			currVal =  PIN_getOutputValue(CC1310_LAUNCHXL_PIN_RLED);
//			PIN_setOutputValue(pinHandle, CC1310_LAUNCHXL_PIN_RLED, !currVal);
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

void pinSetup2()
{
    pinHandle2 = PIN_open(&pinState2, pinTable2);
	if(!pinHandle2) {
		/* Error initializing button pins */
		while(1);
	}

    /* Setup callback for button pins */
    if (PIN_registerIntCb(pinHandle2, &pinCallback) != 0) {
        /* Error registering button callback function */
        while(1);
    }
}



#endif /* PERIPHERALS_PIN_INITIALIZATION_H_ */
