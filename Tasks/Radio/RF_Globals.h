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

/* Driverlib Header files */
#include <ti/devices/DeviceFamily.h>
#include DeviceFamily_constructPath(driverlib/trng.h)

/* TX quantities */
#define RFEASYLINKTX_BURST_SIZE         10
#define RFEASYLINKTXPAYLOAD_LENGTH      29

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

static uint32_t lastTrngVal;

void HalTRNG_InitTRNG( void )
{
    // configure TRNG
    // Note: Min=4x64, Max=1x256, ClkDiv=1+1 gives the same startup and refill
    //       time, and takes about 11us (~14us with overhead).
    TRNGConfigure( (1 << 8), (1 << 8), 0x01 );

    // enable TRNG
    TRNGEnable();

    // init variable to hold the last value read
    lastTrngVal = 0;
}

void HalTRNG_WaitForReady( void )
{
    // poll status
    while(!(TRNGStatusGet() & TRNG_NUMBER_READY));

    return;
}

uint32_t HalTRNG_GetTRNG( void )
{
    uint32_t trngVal;

    // initialize and enable TRNG if TRNG is not enabled
    if (0 == (HWREG(TRNG_BASE + TRNG_O_CTL) & TRNG_CTL_TRNG_EN))
    {
      HalTRNG_InitTRNG();
    }

    // check that a valid value is ready
    while(!(TRNGStatusGet() & TRNG_NUMBER_READY));

    // check to be sure we're not getting the same value repeatedly
    if ( (trngVal = TRNGNumberGet(TRNG_LOW_WORD)) == lastTrngVal )
    {
      return( 0xDEADBEEF );
    }
    else // value changed!
    {
      // so save last TRNG value
      lastTrngVal = trngVal;

      return( trngVal );
    }
}




#endif /* TASKS_RADIO_RF_GLOBALS_H_ */
