/*
 * RF_RX_Tasks.h
 *
 *  Created on: Sep 24, 2017
 *      Author: hunteradams
 */

#ifndef TASKS_RADIO_RF_RX_TASKS_H_
#define TASKS_RADIO_RF_RX_TASKS_H_

#include "RF_Globals.h"
#include <ti/sysbios/knl/Task.h>
#include "../../Peripherals/Pin_Initialization.h"
#include "../Semaphore_Initialization.h"

Task_Struct rxRestartTask;
Task_Struct rxBeaconTask;

static uint8_t rxRestartTaskStack[300];
static uint8_t rxBeaconTaskStack[300];

EasyLink_RxPacket globalPacket = {0};

void rxDoneCb(EasyLink_RxPacket * rxPacket, EasyLink_Status status)
{
    if (status == EasyLink_Status_Success)
    {
        /* Toggle LED2 to indicate RX */
//        PIN_setOutputValue(pinHandle, CC1310_LAUNCHXL_PIN_RLED,
//        		!PIN_getOutputValue(CC1310_LAUNCHXL_PIN_RLED));
        globalPacket.dstAddr[0] = rxPacket->dstAddr[0];
        globalPacket.rssi = rxPacket->rssi;
        globalPacket.absTime = rxPacket->absTime;
        globalPacket.rxTimeout = rxPacket->rxTimeout;
        globalPacket.len = rxPacket->len;
        int i=0;
        while(i < globalPacket.len){
        		globalPacket.payload[i] = rxPacket->payload[i];
        		i += 1;
        }
		Semaphore_post(rxRestartSemaphoreHandle);

    }
    else if(status == EasyLink_Status_Aborted)
    {
        /* Toggle LED1 to indicate command aborted */
//        PIN_setOutputValue(pinHandle, CC1310_LAUNCHXL_PIN_GLED,
//        		!PIN_getOutputValue(CC1310_LAUNCHXL_PIN_GLED));
        Semaphore_post(rxRestartSemaphoreHandle);
    }
    else
    {
        /* Toggle LED1 and LED2 to indicate error */
        PIN_setOutputValue(pinHandle, CC1310_LAUNCHXL_PIN_GLED,
        		!PIN_getOutputValue(CC1310_LAUNCHXL_PIN_GLED));
        PIN_setOutputValue(pinHandle, CC1310_LAUNCHXL_PIN_RLED,
        		!PIN_getOutputValue(CC1310_LAUNCHXL_PIN_RLED));
    }
}

Void rxRestartFunc(UArg arg0, UArg arg1)
{
    while(1) {
    		if(halt){
    			EasyLink_abort();
    			PIN_setOutputValue(pinHandle, Board_PIN_LED0,0);
    			PIN_setOutputValue(pinHandle, Board_PIN_LED1,0);
    			Task_sleep(600000000);
		}
    		Semaphore_pend(rxRestartSemaphoreHandle, BIOS_WAIT_FOREVER);
    		EasyLink_receiveAsync(rxDoneCb, 0);
    }
}

void createRFRXTasks()
{
	Task_Params task_params;
	Task_Params_init(&task_params);

    task_params.stackSize = 300;
    task_params.priority = 2;
	task_params.stack = &rxRestartTaskStack;
	Task_construct(&rxRestartTask, rxRestartFunc,
				   &task_params, NULL);
}




#endif /* TASKS_RADIO_RF_RX_TASKS_H_ */
