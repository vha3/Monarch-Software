/*
 * IMU_Tasks.h
 *
 *  Created on: Sep 24, 2017
 *      Author: hunteradams
 */

#ifndef TASKS_I2C_I2C_TASKS_H_
#define TASKS_I2C_I2C_TASKS_H_

#include <ti/sysbios/knl/Task.h>
#include "../Semaphore_Initialization.h"
#include "../Shared_Resources.h"
#include "Si7021.h"
#include "LSM9DS1.h"

/* Task structs */
Task_Struct i2cTask;

/* Task memory allocation */
static uint8_t i2cTaskStack[1000];

uint16_t raw_temp;
uint16_t raw_humidity;

Void i2cTaskFunc(UArg arg0, UArg arg1)
{

    while (1) {
			/* Wait for startup task to unlock */
			Semaphore_pend(magLockSemaphoreHandle, BIOS_WAIT_FOREVER);

			/* Open i2c channel, setup IMU, initialize I2C channel */
			initI2C();

			/* Wait briefly to let things settle */
			Task_sleep(10000);

			/* Initializate IMU*/
			uint16_t workpls = LSM9DS1begin();
			configInt(XG_INT1, INT_DRDY_G, INT_ACTIVE_LOW, INT_PUSH_PULL);
			configInt(XG_INT2, INT_DRDY_XL, INT_ACTIVE_LOW, INT_PUSH_PULL);
			Task_sleep(1000);

			/* Initialize the temp/humidity sensor */
			beginHumidity();
			Task_sleep(10000);
			heaterOff();
			Task_sleep(1000);
			heaterOff();

			/* Read once from each sensor (improves reliability) */
			readGyro();
			readAccel();
			readMag();
			raw_temp = getRawTemp();
			raw_humidity = getRawHumidity();

    		/* Wait for data to be available from gyro, accel, and mag */
    		Semaphore_pend(magSemaphoreHandle, BIOS_WAIT_FOREVER);
    		Semaphore_pend(gyroSemaphoreHandle, BIOS_WAIT_FOREVER);
    		Semaphore_pend(accelSemaphoreHandle, BIOS_WAIT_FOREVER);

    		/* Read magnetometer, store data */
    		readMag();
    		tx_mx = mx;
    		tx_my = my;
    		tx_mz = mz;
    		Task_sleep(1000);

    		/* Read gyro, store data */
    		readGyro();
    		tx_gx = gx;
    		tx_gy = gy;
    		tx_gz = gz;
    		Task_sleep(1000);

    		/* Read accel, store data */
    		readAccel();
    		tx_ax = ax;
    		tx_ay = ay;
    		tx_az = az;
    		Task_sleep(1000);

    		/* Read temperature, store data */
    		raw_temp = getRawTemp();
    		tx_temp = raw_temp;
    		Task_sleep(1000);

//    		float tempf = getTempFarenheit();
//    		float humid = getRelativeHumidity();
//    		Display_printf(display, 0, 0, "temp: %f", tempf);
//    		Display_printf(display, 0, 0, "humidity: %f", humid);

    		/* Read humidity, store data */
    		raw_humidity = getRawHumidity();
    		tx_humidity = raw_humidity;

    		/* Close i2c channel */
    		I2C_close(i2c);

    		/* Start the light sensor task */
			Semaphore_post(lightsensorLockSemaphoreHandle);

			/* Signal that we're finished with the i2c interface */
			Semaphore_post(i2cDoneSemaphoreHandle);


			/* Sleep (1 hr) */
//			Task_sleep(360000000);
    }
}


void createi2cTask()
{
	Task_Params task_params;
	Task_Params_init(&task_params);
	task_params.stackSize = 1000;
	task_params.priority = 2;
	task_params.stack = &i2cTaskStack;
	Task_construct(&i2cTask, i2cTaskFunc,
					   &task_params, NULL);
}

#endif /* TASKS_I2C_I2C_TASKS_H_ */
