/*
 * IMU_Tasks.h
 *
 *  Created on: Sep 24, 2017
 *      Author: hunteradams
 */

#ifndef TASKS_IMU_IMU_TASKS_H_
#define TASKS_IMU_IMU_TASKS_H_

#include <ti/sysbios/knl/Task.h>
#include "../Semaphore_Initialization.h"
#include "../Shared_Resources.h"
#include "LSM9DS1.h"

/* Task structs */
Task_Struct magTask;
Task_Struct gyroTask;
Task_Struct accelTask;

/* Task memory allocation */
static uint8_t magTaskStack[450];
static uint8_t gyroTaskStack[450];
static uint8_t accelTaskStack[450];

int magN = 0;
int gyroN = 0;
int accelN = 0;

Void magTaskFunc(UArg arg0, UArg arg1)
{
	/* Wait for startup task to unlock */
	Semaphore_pend(magLockSemaphoreHandle, BIOS_WAIT_FOREVER);

	/* Open i2c channel, setup IMU, initialize I2C channel */
	I2C_init();
	LSM9DS1init();
    initI2C();

    /* Wait briefly to let things settle */
    Task_sleep(1000);

	/* Initializate IMU*/
	uint16_t workpls = LSM9DS1begin();
	configInt(XG_INT1, INT_DRDY_G, INT_ACTIVE_LOW, INT_PUSH_PULL);
	configInt(XG_INT2, INT_DRDY_XL, INT_ACTIVE_LOW, INT_PUSH_PULL);

	/* Read from each sensor (improves reliability) */
	readGyro();
	readAccel();
	readMag();

	Semaphore_post(gyroLockSemaphoreHandle);
	Semaphore_post(accelLockSemaphoreHandle);
//	Semaphore_post(humidityLockSemaphoreHandle);
//	Semaphore_post(gpsLockSemaphoreHandle);


    while (1) {
    		Semaphore_pend(magSemaphoreHandle, BIOS_WAIT_FOREVER);
    		Semaphore_pend(batonSemaphoreHandle, BIOS_WAIT_FOREVER);

    		magN += 1;

			readMag();
	//    			Watchdog_clear(watchdogHandle);
//			Display_printf(display, 0, 0,
//										"Magnetometer X: %d \n", mx);
//			Display_printf(display, 0, 0,
//										"Magnetometer Y: %d \n", my);
//			Display_printf(display, 0, 0,
//										"Magnetometer Z: %d \n", mz);
//			Display_printf(display, 0, 0,
//										"Magnetometer count: %d", magN);
    		Semaphore_post(batonSemaphoreHandle);

    		if (magN > 20){
    			Semaphore_post(magDoneSemaphoreHandle);
    			Task_sleep(360000000);
    		}
    }
}

Void gyroTaskFunc(UArg arg0, UArg arg1)
{
	Semaphore_pend(gyroLockSemaphoreHandle, BIOS_WAIT_FOREVER);
    while (1) {
    		Semaphore_pend(gyroSemaphoreHandle, BIOS_WAIT_FOREVER);
    		Semaphore_pend(batonSemaphoreHandle, BIOS_WAIT_FOREVER);

    		gyroN += 1;

			readGyro();
//			Display_printf(display, 0, 0,
//									"Gyro X: %d \n", gx);
//			Display_printf(display, 0, 0,
//									"Gyro Y: %d \n", gy);
//			Display_printf(display, 0, 0,
//									"Gyro Z: %d \n", gz);
//			Display_printf(display, 0, 0,
//									"Gyro count: %d", gyroN);

    		Semaphore_post(batonSemaphoreHandle);

    		if (gyroN > 20){
    			Semaphore_post(gyroDoneSemaphoreHandle);
    			Task_sleep(360000000);
    		}
    }
}

Void accelTaskFunc(UArg arg0, UArg arg1)
{
	Semaphore_pend(accelLockSemaphoreHandle, BIOS_WAIT_FOREVER);
    while (1) {
    		Semaphore_pend(accelSemaphoreHandle, BIOS_WAIT_FOREVER);
    		Semaphore_pend(batonSemaphoreHandle, BIOS_WAIT_FOREVER);

    		accelN += 1;

			readAccel();

//			Display_printf(display, 0, 0,
//								"Accel X: %d \n", ax);
//			Display_printf(display, 0, 0,
//									"Accel Y: %d \n", ay);
//			Display_printf(display, 0, 0,
//									"Accel Z: %d \n", az);
//			Display_printf(display, 0, 0,
//									"Accel count: %d", accelN);

    		Semaphore_post(batonSemaphoreHandle);

    		if (accelN>20){
    			Semaphore_post(accelDoneSemaphoreHandle);
    			Task_sleep(360000000);
    		}
    }
}

void createMagTask()
{
	Task_Params task_params;
	Task_Params_init(&task_params);
	task_params.stackSize = 450;
	task_params.priority = 1;
	task_params.stack = &magTaskStack;
	Task_construct(&magTask, magTaskFunc,
					   &task_params, NULL);
}

void createGyroTask()
{
	Task_Params task_params;
	Task_Params_init(&task_params);
	task_params.stackSize = 450;
	task_params.priority = 2;
	task_params.stack = &gyroTaskStack;
	Task_construct(&gyroTask, gyroTaskFunc,
					   &task_params, NULL);
}

void createAccelTask()
{
	Task_Params task_params;
	Task_Params_init(&task_params);
	task_params.stackSize = 450;
	task_params.priority = 3;
	task_params.stack = &accelTaskStack;
	Task_construct(&accelTask, accelTaskFunc,
					   &task_params, NULL);
}

#endif /* TASKS_IMU_IMU_TASKS_H_ */
