/*
 * RF_TX_Tasks.h
 *
 *  Created on: Sep 24, 2017
 *      Author: hunteradams
 */

#ifndef TASKS_RADIO_RF_TX_TASKS_H_
#define TASKS_RADIO_RF_TX_TASKS_H_

#include "RF_Globals.h"
#include <ti/sysbios/knl/Task.h>
#include "../../Peripherals/Pin_Initialization.h"
#include "../Semaphore_Initialization.h"
//#include "TRIAD.h"

Task_Struct txDataTask;

static uint8_t txDataTaskStack[700];
#pragma DATA_ALIGN(txDataTaskStack, 8)

uint8_t message[30] = {0x20, 0x53, 0x50, 0x41, 0x43, 0x45, 0x20, 0x53,
		0x59, 0x53, 0x54, 0x45, 0x4d, 0x53, 0x20, 0x44, 0x45, 0x53,
		0x49, 0x47, 0x4e, 0x20, 0x53, 0x54, 0x55, 0x44, 0x49, 0x4f,
		0x20, 0x20};

Void txDataTaskFunc(UArg arg0, UArg arg1)
{
	//EasyLink_init(EasyLink_Phy_Custom);
	EasyLink_init(EasyLink_Phy_50kbps2gfsk);
	EasyLink_setRfPwr(12);
	EasyLink_enableRxAddrFilter((uint8_t*)&AddressList, 1, 2);
	EasyLink_setFrequency(915000000);

	uint8_t counter = 0x00;
	while(1) {
		Semaphore_pend(txDataSemaphoreHandle, BIOS_WAIT_FOREVER);
		Semaphore_pend(batonSemaphoreHandle, BIOS_WAIT_FOREVER);

		if(goodToGo){

			EasyLink_abort();
			EasyLink_TxPacket txPacket =  { {0}, 0, 0, {0} };

			//txPacket.payload[0] = BEACON;
			//txPacket.payload[1] = PERSONAL_ADDRESS;

//			txPacket.payload[0] = (uint8_t)(seqNumber >> 8);
//			txPacket.payload[1] = (uint8_t)(seqNumber++);
//			txPacket.payload[2] = sign(ax);
//			txPacket.payload[3] = upperPart(ax);
//			txPacket.payload[4] = lowerPart(ax);
//			txPacket.payload[5] = sign(ay);
//			txPacket.payload[6] = upperPart(ay);
//			txPacket.payload[7] = lowerPart(ay);
//			txPacket.payload[8] = sign(az);
//			txPacket.payload[9] = upperPart(az);
//			txPacket.payload[10] = lowerPart(az);
//
//			txPacket.payload[11] = sign(gx);
//			txPacket.payload[12] = upperPart(gx);
//			txPacket.payload[13] = lowerPart(gx);
//			txPacket.payload[14] = sign(gy);
//			txPacket.payload[15] = upperPart(gy);
//			txPacket.payload[16] = lowerPart(gy);
//			txPacket.payload[17] = sign(gz);
//			txPacket.payload[18] = upperPart(gz);
//			txPacket.payload[19] = lowerPart(gz);
//
//			txPacket.payload[20] = sign(mx);
//			txPacket.payload[21] = upperPart(mx);
//			txPacket.payload[22] = lowerPart(mx);
//			txPacket.payload[23] = sign(my);
//			txPacket.payload[24] = upperPart(my);
//			txPacket.payload[25] = lowerPart(my);
//			txPacket.payload[26] = sign(mz);
//			txPacket.payload[27] = upperPart(mz);
//			txPacket.payload[28] = lowerPart(mz);
			if (counter > 0xfe){
				counter = 0;
			}
			else{
				counter = counter + 0x01;
			}
			int i=0;
			for (i = 0; i < sizeof(message); i++)
			{
			  txPacket.payload[i] = message[i];
			}
			txPacket.payload[30] = counter;

			txPacket.len = RFEASYLINKTXPAYLOAD_LENGTH;
			txPacket.dstAddr[0] = 0xaa;
			txPacket.absTime = 0;
			EasyLink_Status result = EasyLink_transmit(&txPacket);

			if (result == EasyLink_Status_Success)
			{
				/* Toggle LED1 to indicate TX */
//				PIN_setOutputValue(pinHandle, Board_PIN_LED1,!PIN_getOutputValue(Board_PIN_LED1));
			}
			else
			{
				/* Toggle LED1 and LED2 to indicate error */
				PIN_setOutputValue(pinHandle, Board_PIN_LED1,!PIN_getOutputValue(Board_PIN_LED1));
				PIN_setOutputValue(pinHandle, Board_PIN_LED2,!PIN_getOutputValue(Board_PIN_LED2));
			}
			Semaphore_post(rxRestartSemaphoreHandle);
		}
		Semaphore_post(batonSemaphoreHandle);
	}
}

void createRFTXTasks()
{
    Task_Params task_params;
    Task_Params_init(&task_params);
    task_params.stackSize = 700;
    task_params.priority = 2;
	task_params.stack = &txDataTaskStack;
	Task_construct(&txDataTask, txDataTaskFunc,
				   &task_params, NULL);
}


#endif /* TASKS_RADIO_RF_TX_TASKS_H_ */
