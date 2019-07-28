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
#include "../I2C/LSM9DS1.h"
#include "../Semaphore_Initialization.h"
#include "../Shared_Resources.h"
#include <ti/devices/cc13x0/driverlib/sys_ctrl.h>

Task_Struct txDataTask;

static uint8_t txDataTaskStack[700];
#pragma DATA_ALIGN(txDataTaskStack, 8)

/*
 * Set to 1 if you want to attempt to retransmit a packet that couldn't be
 * transmitted after the CCA
 */
#define RFEASYLINKLBT_RETRANSMIT_PACKETS    1

#if RFEASYLINKLBT_RETRANSMIT_PACKETS
bool bAttemptRetransmission = false;
#endif // RFEASYLINKLBT_RETRANSMIT_PACKETS

uint8_t message[30] = {0x20, 0x53, 0x50, 0x41, 0x43, 0x45, 0x20, 0x53,
		0x59, 0x53, 0x54, 0x45, 0x4d, 0x53, 0x20, 0x44, 0x45, 0x53,
		0x49, 0x47, 0x4e, 0x20, 0x53, 0x54, 0x55, 0x44, 0x49, 0x4f,
		0x20, 0x20};

int done = 1;

void lbtDoneCb(EasyLink_Status status)
{
    if (status == EasyLink_Status_Success)
    {
        /* Toggle LED1 to indicate TX */
        PIN_setOutputValue(pinHandle, Board_PIN_LED1, !PIN_getOutputValue(Board_PIN_LED1));
#if RFEASYLINKLBT_RETRANSMIT_PACKETS
        bAttemptRetransmission = false;
        done = 0;
#endif // RFEASYLINKLBT_RETRANSMIT_PACKETS
    }
    else if (status == EasyLink_Status_Busy_Error)
    {
        /* Toggle LED2 to indicate maximum retries reached */
        PIN_setOutputValue(pinHandle, Board_PIN_LED2, !PIN_getOutputValue(Board_PIN_LED2));
        done = 1;

#if RFEASYLINKLBT_RETRANSMIT_PACKETS
        bAttemptRetransmission = true;
#endif // RFEASYLINKLBT_RETRANSMIT_PACKETS
    }
    else
    {
        /* Toggle LED1 and LED2 to indicate error */
        PIN_setOutputValue(pinHandle, Board_PIN_LED1, !PIN_getOutputValue(Board_PIN_LED1));
        PIN_setOutputValue(pinHandle, Board_PIN_LED2, !PIN_getOutputValue(Board_PIN_LED2));
    }

    Semaphore_post(lbtDoneSemaphoreHandle);
}

Void txDataTaskFunc(UArg arg0, UArg arg1)
{


	while(1) {

		Semaphore_pend(txDataSemaphoreHandle, BIOS_WAIT_FOREVER);

		uint32_t absTime;
		Power_setDependency(PowerCC26XX_PERIPH_TRNG);

		EasyLink_Params easyLink_params;
		EasyLink_Params_init(&easyLink_params);

	//	easyLink_params.ui32ModType = EasyLink_Phy_50kbps2gfsk;
		easyLink_params.ui32ModType = EasyLink_Phy_Custom;
		easyLink_params.pGrnFxn = (EasyLink_GetRandomNumber)HalTRNG_GetTRNG;

		/* Initialize EasyLink */
		if(EasyLink_init(&easyLink_params) != EasyLink_Status_Success)
		{
			System_abort("EasyLink_init failed");
		}

		EasyLink_setRfPower(14);
		EasyLink_enableRxAddrFilter((uint8_t*)&AddressList, 1, 2);
		EasyLink_setFrequency(915000000);

		EasyLink_TxPacket txPacket =  { {0}, 0, 0, {0} };

		if(bAttemptRetransmission == false){

			EasyLink_abort();

			txPacket.payload[0] = 0x01;

			txPacket.payload[1] = upperPart(ax);
			txPacket.payload[2] = lowerPart(ax);
			txPacket.payload[3] = upperPart(ay);
			txPacket.payload[4] = lowerPart(ay);
			txPacket.payload[5] = upperPart(az);
			txPacket.payload[6] = lowerPart(az);

			txPacket.payload[7] = upperPart(gx);
			txPacket.payload[8] = lowerPart(gx);
			txPacket.payload[9] = upperPart(gy);
			txPacket.payload[10] = lowerPart(gy);
			txPacket.payload[11] = upperPart(gz);
			txPacket.payload[12] = lowerPart(gz);

			txPacket.payload[13] = upperPart(mx);
			txPacket.payload[14] = lowerPart(mx);
			txPacket.payload[15] = upperPart(my);
			txPacket.payload[16] = lowerPart(my);
			txPacket.payload[17] = upperPart(mz);
			txPacket.payload[18] = lowerPart(mz);

			txPacket.payload[19] = upperPart(tx_temp);
			txPacket.payload[20] = lowerPart(tx_temp);

			txPacket.payload[21] = upperPart(tx_humidity);
			txPacket.payload[22] = lowerPart(tx_humidity);

			txPacket.payload[23] = upperPart(tx_light_top);
			txPacket.payload[24] = lowerPart(tx_light_top);
			txPacket.payload[25] = upperPart(tx_light_bottom);
			txPacket.payload[26] = lowerPart(tx_light_bottom);

			txPacket.payload[27] = upperPart(capacitor_charge);
			txPacket.payload[28] = lowerPart(capacitor_charge);

			int i=0;
			for (i = 0; i < bytes_read; i++)
			{
			  txPacket.payload[i+29] = input[i];
			}

			int packetlen = RFEASYLINKTXPAYLOAD_LENGTH + bytes_read;
			txPacket.len = packetlen;
			txPacket.dstAddr[0] = 0x00;
//				txPacket.absTime = 0;
			/* Set Tx absolute time to current time + 100ms */
			if(EasyLink_getAbsTime(&absTime) != EasyLink_Status_Success)
			{
				// Problem getting absolute time
			}
			txPacket.absTime = absTime + EasyLink_ms_To_RadioTime(0);
		}

		while (done){
			EasyLink_transmitCcaAsync(&txPacket, lbtDoneCb);
			Semaphore_pend(lbtDoneSemaphoreHandle, BIOS_WAIT_FOREVER);
		}
		done = 1;


		Task_sleep(10000);
		PIN_setOutputValue(pinHandle, Board_PIN_LED0,0);
		PIN_setOutputValue(pinHandle, Board_PIN_LED1,0);


		Watchdog_clear(watchdogHandle);
		Watchdog_close(watchdogHandle);


		// Sleep for 20 min
//		Task_sleep(30000000);
		Task_sleep(120000000);
//		Task_sleep(1000000);
		Semaphore_post(startSemaphoreHandle);

//		SysCtrlSystemReset();

//		Task_sleep(360000000);
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
