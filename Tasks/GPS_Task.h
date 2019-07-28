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
#include "Shared_Resources.h"

int bytesRead = 0;

Task_Struct gpsTask;
static uint8_t gpsTaskStack[2000];

void Serial_RxDataCallback(UART_Handle handle, void *buffer, size_t size)
{

    Semaphore_post(readSemaphoreHandle);
    bytesRead = size;
    bytes_read = size;
}


Void gpsFunc(UArg arg0, UArg arg1)
{

//	bool gotFix;

	while (1) {
		/* Check whether the GPS has a navigation fix */
//		gotFix = Semaphore_pend(gpsfixSemaphoreHandle, 100);
		Semaphore_pend(gpsLockSemaphoreHandle, BIOS_WAIT_FOREVER);

		uart = UART_open(Board_UART0, &uartParams);

		if (uart == NULL) {
			while (1);
		}
		UART_control(uart, UARTCC26XX_CMD_RETURN_PARTIAL_ENABLE, NULL);

//		uint8_t query[] = {0xa0, 0xa1, 0x00, 0x04, 0x64, 0x0a, 0x01, 0x01, 0x6e, 0x0d, 0x0a};
//		UART_write(uart, query, sizeof(query));
//		uint8_t nmea[] = {0xa0, 0xa1, 0x00, 0x09, 0x08, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x01, 0x08, 0x0d, 0x0a};
//		UART_write(uart, nmea, sizeof(nmea));

		bool gotData = 0;

		while (!(gotData & (bytesRead==72))) {
			/* Clear UART FIFO */
			UART_control(uart, UARTCC26XX_CMD_RX_FIFO_FLUSH, NULL);
			/* Read UART from GPS */
			int numBytes = UART_read(uart, &input, sizeof(input));
			/* Pend on a semaphore indicating data received */
			gotData = Semaphore_pend(readSemaphoreHandle, 200000);
		}

		/* Clear the watchdog timer */
//		Watchdog_clear(watchdogHandle);
		UART_write(uart, &input, bytesRead);
		UART_control(uart, UARTCC26XX_CMD_RX_FIFO_FLUSH, NULL);

		UART_readCancel(uart);
		UART_writeCancel(uart);
		UART_close(uart);

		PIN_setOutputValue(pinHandle, IOID_21, 0);
		PIN_setOutputValue(pinHandle, Board_PIN_LED0,0);
		PIN_setOutputValue(pinHandle, Board_PIN_LED1,0);

		Semaphore_post(txDataSemaphoreHandle);

		/* Sleep (1 hr) */
//		Task_sleep(360000000);

	}
}

void createGPSTask()
{
	Task_Params task_params;
	Task_Params_init(&task_params);
	task_params.stackSize = 2000;
	task_params.priority = 4;
	task_params.stack = &gpsTaskStack;
	Task_construct(&gpsTask, gpsFunc,
				   &task_params, NULL);
}

#endif /* TASKS_GPS_TASK_H_ */
