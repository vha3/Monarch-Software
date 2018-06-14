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

Task_Struct magTask;
Task_Struct gyroTask;
Task_Struct accelTask;

static uint8_t magTaskStack[450];
static uint8_t gyroTaskStack[450];
static uint8_t accelTaskStack[450];

Void magTaskFunc(UArg arg0, UArg arg1)
{
	I2C_init();
    LSM9DS1init();
    initI2C();

	/* Initialization and Calibration */
    uint16_t workpls = LSM9DS1begin();
    configInt(XG_INT1, INT_DRDY_G, INT_ACTIVE_HIGH, INT_PUSH_PULL);
    configInt(XG_INT2, INT_DRDY_XL, INT_ACTIVE_HIGH, INT_PUSH_PULL);
    //		calibrate(1);
    //		calibrateMag(1);

    	/* getMagInitial is only required if you're calibrating for the computer attitude */
    //		getMagInitial();

    /* Unlock other tasks */
	goodToGo += 1;

	/* Read from each sensor (improves reliability) */
	readGyro();
	readAccel();
	readMag();
    while (1) {
    		Semaphore_pend(magSemaphoreHandle, BIOS_WAIT_FOREVER);
    		Semaphore_pend(batonSemaphoreHandle, BIOS_WAIT_FOREVER);
    		if(goodToGo){
    			readMag();
//    			Watchdog_clear(watchdogHandle);
//    			Display_printf(display, 0, 0,
//    									"Magnetometer X: %d \n", mx);
//    			Display_printf(display, 0, 0,
//    			    						"Magnetometer Y: %d \n", my);
//    			Display_printf(display, 0, 0,
//    			    						"Magnetometer Z: %d \n", mz);
    		}
    		Semaphore_post(batonSemaphoreHandle);
    }
}

Void gyroTaskFunc(UArg arg0, UArg arg1)
{
    while (1) {
    		Semaphore_pend(gyroSemaphoreHandle, BIOS_WAIT_FOREVER);
    		Semaphore_pend(batonSemaphoreHandle, BIOS_WAIT_FOREVER);
    		if(goodToGo){
    			readGyro();
//    			Display_printf(display, 0, 0,
//									"Gyro X: %d \n", gx);
//			Display_printf(display, 0, 0,
//									"Gyro Y: %d \n", gy);
//			Display_printf(display, 0, 0,
//									"Gyro Z: %d \n", gz);

    		}
    		Semaphore_post(batonSemaphoreHandle);
    }
}

Void accelTaskFunc(UArg arg0, UArg arg1)
{
    while (1) {
    		Semaphore_pend(accelSemaphoreHandle, BIOS_WAIT_FOREVER);
    		Semaphore_pend(batonSemaphoreHandle, BIOS_WAIT_FOREVER);
    		if(goodToGo){
    			readAccel();
//    			Display_printf(display, 0, 0,
//									"Accel X: %d \n", ax);
//			Display_printf(display, 0, 0,
//									"Accel Y: %d \n", ay);
//			Display_printf(display, 0, 0,
//									"Accel Z: %d \n", az);
    		}
    		Semaphore_post(batonSemaphoreHandle);
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
