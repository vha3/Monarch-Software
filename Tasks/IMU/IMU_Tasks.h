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
#include "../Humidity/Si7021.h"

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
    Task_sleep(10000);

	/* Initializate IMU*/
	uint16_t workpls = LSM9DS1begin();
	configInt(XG_INT1, INT_DRDY_G, INT_ACTIVE_LOW, INT_PUSH_PULL);
	configInt(XG_INT2, INT_DRDY_XL, INT_ACTIVE_LOW, INT_PUSH_PULL);

	Task_sleep(1000);

	/* Read from each sensor (improves reliability) */
	readGyro();
	readAccel();
	readMag();

	beginHumidity();
	Task_sleep(10000);
	heaterOff();
	Task_sleep(10000);


//	Semaphore_post(gpsLockSemaphoreHandle);


    while (1) {
    		Semaphore_pend(magSemaphoreHandle, BIOS_WAIT_FOREVER);
    		Semaphore_pend(batonSemaphoreHandle, BIOS_WAIT_FOREVER);
    		Semaphore_post(gyroLockSemaphoreHandle);

    		magN += 1;

			readMag();
			average_mx += (mx >> 3);
			average_my += (my >> 3);
			average_mz += (mz >> 3);

//			Display_printf(display, 0, 0, "MX: %x", mx);
//			Display_printf(display, 0, 0, "MY: %x", my);
//			Display_printf(display, 0, 0, "MZ: %x \n", mz);

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

    		if (magN >= 8){
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
    		Semaphore_post(accelLockSemaphoreHandle);

    		gyroN += 1;

			readGyro();
			average_gx += (gx >> 3);
			average_gy += (gy >> 3);
			average_gz += (gz >> 3);

//			Display_printf(display, 0, 0, "GX: %x", gx);
//			Display_printf(display, 0, 0, "GY: %x", gy);
//			Display_printf(display, 0, 0, "GZ: %x \n", gz);

//			Display_printf(display, 0, 0,
//									"Gyro X: %d \n", gx);
//			Display_printf(display, 0, 0,
//									"Gyro Y: %d \n", gy);
//			Display_printf(display, 0, 0,
//									"Gyro Z: %d \n", gz);
//			Display_printf(display, 0, 0,
//									"Gyro count: %d", gyroN);

    		Semaphore_post(batonSemaphoreHandle);

    		if (gyroN >= 8){
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
    		Semaphore_post(humidityLockSemaphoreHandle);

    		accelN += 1;

			readAccel();
			average_ax += (ax >> 3);
			average_ay += (ay >> 3);
			average_az += (az >> 3);

//			Display_printf(display, 0, 0, "AX: %x", ax);
//			Display_printf(display, 0, 0, "AY: %x", ay);
//			Display_printf(display, 0, 0, "AZ: %x \n", az);

//			Display_printf(display, 0, 0,
//								"Accel X: %d \n", ax);
//			Display_printf(display, 0, 0,
//									"Accel Y: %d \n", ay);
//			Display_printf(display, 0, 0,
//									"Accel Z: %d \n", az);
//			Display_printf(display, 0, 0,
//									"Accel count: %d", accelN);

			Semaphore_post(batonSemaphoreHandle);

    		if (accelN>=8){
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
	task_params.priority = 2;
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
	task_params.priority = 2;
	task_params.stack = &accelTaskStack;
	Task_construct(&accelTask, accelTaskFunc,
					   &task_params, NULL);
}

#endif /* TASKS_IMU_IMU_TASKS_H_ */
