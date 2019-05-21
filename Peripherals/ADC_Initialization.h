/*
 * ADC_Initialization.h
 *
 *  Created on: Sep 25, 2017
 *      Author: hunteradams
 */

#ifndef PERIPHERALS_ADC_INITIALIZATION_H_
#define PERIPHERALS_ADC_INITIALIZATION_H_

#include "Board.h"
#include <ti/drivers/ADC.h>
#include "Pin_Initialization.h"

ADC_Handle   adc1;
ADC_Handle   adc0;
ADC_Handle	 adc3;
ADC_Params   params;

void adc0Setup()
{
	ADC_Params_init(&params);
	adc0 = ADC_open(Board_ADC0, &params);

	if (adc0 == NULL) {
		while (1);
	}
}

void adc1Setup()
{
	ADC_Params_init(&params);
	adc1 = ADC_open(Board_ADC1, &params);

	if (adc1 == NULL) {
		while (1);
	}
}

void adc3Setup()
{
	ADC_Params_init(&params);
	adc3 = ADC_open(Board_ADC3, &params);

	if (adc3 == NULL) {
		while (1);
	}
}

#endif /* PERIPHERALS_ADC_INITIALIZATION_H_ */
