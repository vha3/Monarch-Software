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

UART_Handle uart;
UART_Params uartParams;

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
	Semaphore_pend(gpsLockSemaphoreHandle, BIOS_WAIT_FOREVER);

	UART_init();

    UART_Params_init(&uartParams);
    uartParams.writeDataMode = UART_DATA_BINARY;
    uartParams.readDataMode = UART_DATA_BINARY;
    uartParams.readReturnMode = UART_RETURN_FULL;
    uartParams.readEcho = UART_ECHO_OFF;
    uartParams.baudRate = 9600;

    uartParams.readMode       = UART_MODE_CALLBACK;
    uartParams.readCallback   = &Serial_RxDataCallback;

    uart = UART_open(Board_UART0, &uartParams);

	if (uart == NULL) {
		while (1);
	}
	UART_control(uart, UARTCC26XX_CMD_RETURN_PARTIAL_ENABLE, NULL);
//	char        input[512] = {0};
//	const char 	newlinePrompt[] = "\r\n";
//	const char  startind[] = "Starting now\r\n";
//	const char  endind[] = "Ending now\r\n";

//	uint8_t query[] = {0xa0, 0xa1, 0x00, 0x04, 0x64, 0x0a, 0x01, 0x01, 0x6e, 0x0d, 0x0a};
//	uint8_t factory_defaults[] = {0xa0, 0xa1, 0x00, 0x02, 0x04, 0x00, 0x04, 0x0d, 0x0a};
//	UART_write(uart, factory_defaults, sizeof(factory_defaults));
//	Task_sleep(1000000);
	uint8_t nmea[] = {0xa0, 0xa1, 0x00, 0x09, 0x08, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x01, 0x08, 0x0d, 0x0a};
	UART_write(uart, nmea, sizeof(nmea));
//	uint8_t rate[] = {0xa0, 0xa1, 0x00, 0x03, 0x0e, 0x02, 0x00, 0x0f, 0x0d, 0x0a};
//	UART_write(uart, rate, sizeof(rate));
//	int i=0;
//	while(i<sizeof(query)){
//		UART_write(uart, &query[i], 1);
//		i+=1;
//	}

	bool gotData;
	bool gotFix;

	/* ADC conversion result variables */
	uint16_t adcValue0;
	uint32_t adcValue0MicroVolt;
	/* Variable to hold ADC status */
	int_fast16_t res;

	/* Start adc channel 0 (connected to supercap)*/
	adc0Setup();

	while (1) {
		/* Check whether the GPS has a navigation fix */
		gotFix = Semaphore_pend(gpsfixSemaphoreHandle, 100);

		/* Clear UART FIFO */
		UART_control(uart, UARTCC26XX_CMD_RX_FIFO_FLUSH, NULL);
		/* Read UART from GPS */
		int numBytes = UART_read(uart, &input, sizeof(input));
		/* Pend on a semaphore indicating data received */
		gotData = Semaphore_pend(readSemaphoreHandle, 200000);
		/* If data was received, write the data to the terminal and flush */
		if (gotData & (bytesRead==72)) {
			/* Clear the watchdog timer */
			Watchdog_clear(watchdogHandle);
			UART_write(uart, &input, bytesRead);
			UART_control(uart, UARTCC26XX_CMD_RX_FIFO_FLUSH, NULL);
		}
		/* Otherwise, just flush */
		else {
			UART_control(uart, UARTCC26XX_CMD_RX_FIFO_FLUSH, NULL);
		}

		/* If GPS has a fix, close everything and transmit */
		/* bytesRead==72?*/
		if (gotFix & (bytesRead==72)) {
			UART_readCancel(uart);
			UART_writeCancel(uart);
			UART_close(uart);

			ADC_close(adc0);

			PIN_setOutputValue(pinHandle, IOID_21, 0);
			PIN_setOutputValue(pinHandle, Board_PIN_LED0,0);
			PIN_setOutputValue(pinHandle, Board_PIN_LED1,0);

			Semaphore_post(txDataSemaphoreHandle);

			/* Sleep (1 hr) */
			Task_sleep(360000000);
		}

		/* Otherwise ... */
		else{
			/* Clear the watchdog */
//			Watchdog_clear(watchdogHandle);

			/* Retrieve ADC measurement from channel 0, store in adcValue0 */
			res = ADC_convert(adc0, &adcValue0);

			if (res == ADC_STATUS_SUCCESS) {
				/* Convert measured value to microvolts, store in adcValue0MicroVolt */
				adcValue0MicroVolt = ADC_convertRawToMicroVolts(adc0, adcValue0);

				/* If cap is below 0.7V, close everything and transmit */
				if (adcValue0MicroVolt < 700000) {
					UART_readCancel(uart);
					UART_writeCancel(uart);
					UART_close(uart);

					ADC_close(adc0);

					PIN_setOutputValue(pinHandle, IOID_21, 0);
					PIN_setOutputValue(pinHandle, Board_PIN_LED0,0);
					PIN_setOutputValue(pinHandle, Board_PIN_LED1,0);

					Semaphore_post(txDataSemaphoreHandle);

					/* Sleep (1 hr) */
					Task_sleep(360000000);
				}
				/* Otherwise, do nothing */
				else {
				}
			}
			else {
			}
			Task_sleep(100);
		}
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
