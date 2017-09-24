/*
 * Semaphore_Initialization.h
 *
 *  Created on: Sep 24, 2017
 *      Author: hunteradams
 */

#ifndef TASKS_SEMAPHORE_INITIALIZATION_H_
#define TASKS_SEMAPHORE_INITIALIZATION_H_

#include <ti/sysbios/knl/Semaphore.h>

/* Semaphore structs */
static Semaphore_Struct initSemaphore;
static Semaphore_Handle initSemaphoreHandle;

static Semaphore_Struct calibSemaphore;
static Semaphore_Handle calibSemaphoreHandle;

static Semaphore_Struct magSemaphore;
static Semaphore_Handle magSemaphoreHandle;

static Semaphore_Struct gyroSemaphore;
static Semaphore_Handle gyroSemaphoreHandle;

static Semaphore_Struct accelSemaphore;
static Semaphore_Handle accelSemaphoreHandle;

static Semaphore_Struct txDataSemaphore;
static Semaphore_Handle txDataSemaphoreHandle;

static Semaphore_Struct rxRestartSemaphore;
static Semaphore_Handle rxRestartSemaphoreHandle;

static Semaphore_Struct rxBeaconSemaphore;
static Semaphore_Handle rxBeaconSemaphoreHandle;

static Semaphore_Struct readSemaphore;
static Semaphore_Handle readSemaphoreHandle;

static Semaphore_Struct batonSemaphore;
static Semaphore_Handle batonSemaphoreHandle;

void semaphoreSetup()
{
    /* Create Semaphores */
    Semaphore_construct(&initSemaphore, 1, NULL);
    initSemaphoreHandle = Semaphore_handle(&initSemaphore);

    Semaphore_construct(&calibSemaphore, 0, NULL);
    calibSemaphoreHandle = Semaphore_handle(&calibSemaphore);

    Semaphore_construct(&magSemaphore, 0, NULL);
    magSemaphoreHandle = Semaphore_handle(&magSemaphore);

    Semaphore_construct(&gyroSemaphore, 0, NULL);
    gyroSemaphoreHandle = Semaphore_handle(&gyroSemaphore);

    Semaphore_construct(&accelSemaphore, 0, NULL);
    accelSemaphoreHandle = Semaphore_handle(&accelSemaphore);

	Semaphore_construct(&txDataSemaphore, 0, NULL);
	txDataSemaphoreHandle = Semaphore_handle(&txDataSemaphore);

	Semaphore_construct(&rxRestartSemaphore, 0, NULL);
	rxRestartSemaphoreHandle = Semaphore_handle(&rxRestartSemaphore);

	Semaphore_construct(&rxBeaconSemaphore, 0, NULL);
	rxBeaconSemaphoreHandle = Semaphore_handle(&rxBeaconSemaphore);

	Semaphore_construct(&readSemaphore, 0, NULL);
	readSemaphoreHandle = Semaphore_handle(&readSemaphore);

    Semaphore_construct(&batonSemaphore, 1, NULL);
    batonSemaphoreHandle = Semaphore_handle(&batonSemaphore);
}





#endif /* TASKS_SEMAPHORE_INITIALIZATION_H_ */
