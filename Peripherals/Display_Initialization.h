/*
 * Display_Initialization.h
 *
 *  Created on: Sep 25, 2017
 *      Author: hunteradams
 */

#ifndef PERIPHERALS_DISPLAY_INITIALIZATION_H_
#define PERIPHERALS_DISPLAY_INITIALIZATION_H_

#include <ti/display/Display.h>
#include "Board.h"

static Display_Handle display;

void displaySetup()
{
    display = Display_open(Display_Type_UART, NULL);
    if (display == NULL) {
        /* Failed to open display driver */
        while (1);
    }
}


#endif /* PERIPHERALS_DISPLAY_INITIALIZATION_H_ */
