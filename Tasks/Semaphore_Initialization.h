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

static Semaphore_Struct readSemaphore;
static Semaphore_Handle readSemaphoreHandle;

static Semaphore_Struct batonSemaphore;
static Semaphore_Handle batonSemaphoreHandle;

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

	Semaphore_construct(&readSemaphore, 0, &semparams);
	readSemaphoreHandle = Semaphore_handle(&readSemaphore);

    Semaphore_construct(&batonSemaphore, 1, &semparams);
    batonSemaphoreHandle = Semaphore_handle(&batonSemaphore);
}





#endif /* TASKS_SEMAPHORE_INITIALIZATION_H_ */
