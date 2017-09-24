/*
 * GPS_Task.h
 *
 *  Created on: Sep 24, 2017
 *      Author: hunteradams
 */

#ifndef TASKS_GPS_TASK_H_
#define TASKS_GPS_TASK_H_

#include <ti/drivers/UART.h>
#include <ti/drivers/uart/UARTCC26XX.h>
#include <ti/sysbios/knl/Task.h>
#include "Semaphore_Initialization.h"

UART_Handle uart;
UART_Params uartParams;

int bytesRead = 0;

Task_Struct gpsTask;
static uint8_t gpsTaskStack[1024];

void Serial_RxDataCallback(UART_Handle handle, void *buffer, size_t size)
{

    Semaphore_post(readSemaphoreHandle);
    bytesRead = size;
}


Void gpsFunc(UArg arg0, UArg arg1)
{
    UART_Params_init(&uartParams);
    uartParams.writeDataMode = UART_DATA_BINARY;
    uartParams.readDataMode = UART_DATA_TEXT;
    uartParams.readReturnMode = UART_RETURN_FULL;
    uartParams.readEcho = UART_ECHO_OFF;
    uartParams.baudRate = 9600;

    uartParams.readMode       = UART_MODE_CALLBACK;
    uartParams.readCallback   = &Serial_RxDataCallback;

    uart = UART_open(Board_UART0, &uartParams);

	if (uart == NULL) {
		/* UART_open() failed */
		while (1);
	}
	UART_control(uart, UARTCC26XX_CMD_RETURN_PARTIAL_ENABLE, NULL);
	char        input[512] = {0};
	const char 	newlinePrompt[] = "\r\n";
	while (1) {
		if(goodToGo){
			int numBytes = UART_read(uart, &input, sizeof(input));
			Semaphore_pend(readSemaphoreHandle, BIOS_WAIT_FOREVER);
			UART_write(uart, &input, bytesRead);
			UART_write(uart, newlinePrompt, sizeof(newlinePrompt));
			UART_control(uart, UARTCC26XX_CMD_RX_FIFO_FLUSH, NULL);
		}
	}
}

void createGPSTask()
{
	Task_Params task_params;
	Task_Params_init(&task_params);
	task_params.stackSize = 1024;
	task_params.priority = 1;
	task_params.stack = &gpsTaskStack;
	Task_construct(&gpsTask, gpsFunc,
				   &task_params, NULL);
}



#endif /* TASKS_GPS_TASK_H_ */
