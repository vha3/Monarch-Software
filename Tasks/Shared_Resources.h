/*
 * Shared_Resources.h
 *
 *  Created on: Sep 24, 2017
 *      Author: hunteradams
 */

#ifndef TASKS_SHARED_RESOURCES_H_
#define TASKS_SHARED_RESOURCES_H_

// Name
// Payload Size

/* Capacitor state of charge */
uint16_t capacitor_charge;
uint32_t capacitor_charge_uv;

/* Light sensors */
uint16_t average_light_top = 0;
uint32_t average_light_top_uv = 0;
uint16_t average_light_bottom = 0;
uint32_t average_light_bottom_uv = 0;

/* Temperature/Humidity sensor */
uint16_t average_temp = 0;
uint16_t average_humidity = 0;

/* IMU */
int16_t average_gx = 0;
int16_t average_gy = 0;
int16_t average_gz = 0;

int16_t average_ax = 0;
int16_t average_ay = 0;
int16_t average_az = 0;

int16_t average_mx = 0;
int16_t average_my = 0;
int16_t average_mz = 0;

// GPS Data
// Checksum


#endif /* TASKS_SHARED_RESOURCES_H_ */
