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
uint16_t tx_light_top = 0;
uint32_t tx_light_top_uv = 0;
uint16_t tx_light_bottom = 0;
uint32_t tx_light_bottom_uv = 0;

/* Temperature/Humidity sensor */
uint16_t tx_temp = 0;
uint16_t tx_humidity = 0;

/* IMU */
int16_t tx_gx = 0;
int16_t tx_gy = 0;
int16_t tx_gz = 0;

int16_t tx_ax = 0;
int16_t tx_ay = 0;
int16_t tx_az = 0;

int16_t tx_mx = 0;
int16_t tx_my = 0;
int16_t tx_mz = 0;

// GPS Data
int bytes_read = 0;
// Checksum


#endif /* TASKS_SHARED_RESOURCES_H_ */
