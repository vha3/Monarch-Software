/*
 * RF_Globals.h
 *
 *  Created on: Sep 24, 2017
 *      Author: hunteradams
 */

#ifndef TASKS_RADIO_RF_GLOBALS_H_
#define TASKS_RADIO_RF_GLOBALS_H_

#include "smartrf_settings/smartrf_settings.h"
#include "easylink/EasyLink.h"
#include "Network_Types.h"

/* TX quantities */
#define RFEASYLINKTX_BURST_SIZE         10
#define RFEASYLINKTXPAYLOAD_LENGTH      5

/* Addresses */
#define UNIVERSAL_ADDRESS 0xaa
#define PERSONAL_ADDRESS 0xbb

static uint8_t AddressList[0x10] =
{
		UNIVERSAL_ADDRESS, // First address to match
		PERSONAL_ADDRESS // Second address to match
};

/* List for holding neighbors */
int numConnections = 0;
uint8_t Connections[MAXNEIGHBORS] = {};

/* ###########################################################
 * Some Bit Manipulation Functions
 */
//static uint16_t seqNumber;

uint8_t sign(int16_t x)
{
	if(x>0){
		return 1;
	}
	else{
		return 0;
	}
}

uint8_t lowerPart(int16_t x){
	return (x & 0xff);
}

uint8_t upperPart(int16_t x){
	return (x >> 8) & 0xff;
}
/*
 * ###########################################################
 */



#endif /* TASKS_RADIO_RF_GLOBALS_H_ */
