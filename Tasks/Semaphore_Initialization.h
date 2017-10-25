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

//static Semaphore_Struct magLockSemaphore;
//static Semaphore_Handle magLockSemaphoreHandle;
//
//static Semaphore_Struct gyroLockSemaphore;
//static Semaphore_Handle gyroLockSemaphoreHandle;
//
//static Semaphore_Struct accelLockSemaphore;
//static Semaphore_Handle accelLockSemaphoreHandle;
//
//static Semaphore_Struct gpsLockSemaphore;
//static Semaphore_Handle gpsLockSemaphoreHandle;
//
//static Semaphore_Struct adcLockSemaphore;
//static Semaphore_Handle adcLockSemaphoreHandle;
//
//static Semaphore_Struct pwmLockSemaphore;
//static Semaphore_Handle pwmLockSemaphoreHandle;
//
//static Semaphore_Struct rxRestartLockSemaphore;
//static Semaphore_Handle rxRestartLockSemaphoreHandle;
//
//static Semaphore_Struct rxBeaconLockSemaphore;
//static Semaphore_Handle rxBeaconLockSemaphoreHandle;
//
//static Semaphore_Struct txDataLockSemaphore;
//static Semaphore_Handle txDataLockSemaphoreHandle;

void semaphoreSetup()
{
    /* Create Semaphores */

	Semaphore_Params semparams;
	Semaphore_Params_init(&semparams);

	semparams.mode = Semaphore_Mode_BINARY_PRIORITY;

    Semaphore_construct(&magSemaphore, 0, &semparams);
    magSemaphoreHandle = Semaphore_handle(&magSemaphore);

    Semaphore_construct(&gyroSemaphore, 0, &semparams);
    gyroSemaphoreHandle = Semaphore_handle(&gyroSemaphore);

    Semaphore_construct(&accelSemaphore, 0, &semparams);
    accelSemaphoreHandle = Semaphore_handle(&accelSemaphore);

	Semaphore_construct(&txDataSemaphore, 0, &semparams);
	txDataSemaphoreHandle = Semaphore_handle(&txDataSemaphore);

	Semaphore_construct(&rxRestartSemaphore, 0, &semparams);
	rxRestartSemaphoreHandle = Semaphore_handle(&rxRestartSemaphore);

	Semaphore_construct(&rxBeaconSemaphore, 0, &semparams);
	rxBeaconSemaphoreHandle = Semaphore_handle(&rxBeaconSemaphore);

	Semaphore_construct(&readSemaphore, 0, &semparams);
	readSemaphoreHandle = Semaphore_handle(&readSemaphore);

    Semaphore_construct(&batonSemaphore, 1, &semparams);
    batonSemaphoreHandle = Semaphore_handle(&batonSemaphore);

//    Semaphore_construct(&magLockSemaphore, 0, &semparams);
//    magLockSemaphoreHandle = Semaphore_handle(&magLockSemaphore);
//
//    Semaphore_construct(&gyroLockSemaphore, 0, &semparams);
//    gyroLockSemaphoreHandle = Semaphore_handle(&gyroLockSemaphore);
//
//    Semaphore_construct(&accelLockSemaphore, 0, &semparams);
//    accelLockSemaphoreHandle = Semaphore_handle(&accelLockSemaphore);
//
//	Semaphore_construct(&gpsLockSemaphore, 0, &semparams);
//	gpsLockSemaphoreHandle = Semaphore_handle(&gpsLockSemaphore);
//
//	Semaphore_construct(&adcLockSemaphore, 0, &semparams);
//	adcLockSemaphoreHandle = Semaphore_handle(&adcLockSemaphore);
//
//	Semaphore_construct(&pwmLockSemaphore, 0, &semparams);
//	pwmLockSemaphoreHandle = Semaphore_handle(&pwmLockSemaphore);
//
//	Semaphore_construct(&rxRestartLockSemaphore, 0, &semparams);
//	rxRestartLockSemaphoreHandle = Semaphore_handle(&rxRestartLockSemaphore);
//
//	Semaphore_construct(&rxBeaconLockSemaphore, 0, &semparams);
//	rxBeaconLockSemaphoreHandle = Semaphore_handle(&rxBeaconLockSemaphore);
//
//	Semaphore_construct(&txDataLockSemaphore, 0, &semparams);
//	txDataLockSemaphoreHandle = Semaphore_handle(&txDataLockSemaphore);
}





#endif /* TASKS_SEMAPHORE_INITIALIZATION_H_ */
