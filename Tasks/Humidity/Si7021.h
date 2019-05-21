/*
 * Si7021.h
 *
 *  Created on: Oct 2, 2018
 *      Author: hunteradams
 */

#ifndef TASKS_HUMIDITY_SI7021_H_
#define TASKS_HUMIDITY_SI7021_H_

/* ===============================================================
 * =================== Includes ==================================
 * ===============================================================
 */
/* Include IMU registers and datatypes */
#include <math.h>

/* Include necessary TI drivers */
//#include <ti/drivers/GPIO.h>
#include <ti/sysbios/knl/Clock.h>
#include <ti/drivers/I2C.h>

/* Include board file for access to pin definitions */
#include "Board.h"


#define ADDRESS      0x40

#define TEMP_MEASURE_HOLD  0xE3
#define HUMD_MEASURE_HOLD  0xE5
#define TEMP_MEASURE_NOHOLD  0xF3
#define HUMD_MEASURE_NOHOLD  0xF5
#define TEMP_PREV   0xE0

#define WRITE_USER_REG  0xE6
#define READ_USER_REG  0xE7
#define SOFT_RESET  0xFE

#define HTRE        0x02
#define _BV(bit) (1 << (bit))

#define CRC_POLY 0x988000 // Shifted Polynomial for CRC check

// Error codes
#define I2C_TIMEOUT 	998
#define BAD_CRC		999

/* Create global i2c handle and parameters structs */
I2C_Handle      i2c;
I2C_Params      i2cParams;
I2C_Transaction i2cTransaction;

/* Function that opens I2C channel, turns on red LED if failure */
//void initI2C(void){
//	I2C_Params_init(&i2cParams);
//	i2cParams.bitRate = I2C_400kHz;
//	i2c = I2C_open(Board_I2C0, &i2cParams);
//	if (i2c == NULL) {
////		GPIO_write(Board_GPIO_LED0, Board_GPIO_LED_ON);
//		while (1);
//	}
//	else {
//	}
//}

/* Function that sends a 2-part command code and returns the response byte */
uint8_t I2Cwrite2read1HumByte(uint8_t address, uint8_t command_code_1, uint8_t command_code_2){

	uint8_t data;

    uint8_t txBuffer[1];
    uint8_t rxBuffer[1];

    txBuffer[0] = command_code_1;
    txBuffer[1] = command_code_2;
    i2cTransaction.slaveAddress = address;// 0x1e;
    i2cTransaction.writeBuf = txBuffer;
    i2cTransaction.writeCount = 2;
    i2cTransaction.readBuf = rxBuffer;
    i2cTransaction.readCount = 1;


    if(I2C_transfer(i2c, &i2cTransaction)) {
    }
    else{
//    		GPIO_write(Board_GPIO_LED0, Board_GPIO_LED_ON);
    }

    data = rxBuffer[0];
    return data;

}

uint16_t I2Cwrite1read2HumByte(uint8_t address, uint8_t command_code){

    uint8_t txBuffer[0];
    uint8_t rxBuffer[1];
    int i=0;

    txBuffer[0] = command_code;
    i2cTransaction.slaveAddress = address;// 0x1e;
    i2cTransaction.writeBuf = txBuffer;
    i2cTransaction.writeCount = 1;
    i2cTransaction.readBuf = rxBuffer;
    i2cTransaction.readCount = 2;


    if(I2C_transfer(i2c, &i2cTransaction)) {
    }
    else{
//    		GPIO_write(Board_GPIO_LED0, Board_GPIO_LED_ON);
    }

    uint16_t msb = rxBuffer[0];
    uint16_t lsb = rxBuffer[1];

    lsb &= 0xFC;

    uint16_t meas = msb << 8 | lsb;

    return meas;

}

uint8_t I2Cwrite1read1HumByte(uint8_t address, uint8_t command_code){

    uint8_t txBuffer[0];
    uint8_t rxBuffer[0];
    int i=0;

    txBuffer[0] = command_code;
    i2cTransaction.slaveAddress = address;// 0x1e;
    i2cTransaction.writeBuf = txBuffer;
    i2cTransaction.writeCount = 1;
    i2cTransaction.readBuf = rxBuffer;
    i2cTransaction.readCount = 1;


    if(I2C_transfer(i2c, &i2cTransaction)) {
    }
    else{
//    		GPIO_write(Board_GPIO_LED0, Board_GPIO_LED_ON);
    }

    return rxBuffer[0];

}

/* Function that performs a burst read of from given address/subaddress */
uint8_t I2CreadHumBytes(uint8_t address, uint8_t subAddress, uint8_t * dest, uint8_t count){

	uint8_t txBuffer[1];
	uint8_t rxBuffer[count];
	int i=0;

	txBuffer[0] = subAddress | 0x80;

    i2cTransaction.slaveAddress = address;// 0x1e;
    i2cTransaction.writeBuf = txBuffer;
    i2cTransaction.writeCount = 1;
    i2cTransaction.readBuf = rxBuffer;
    i2cTransaction.readCount = count;


    if(I2C_transfer(i2c, &i2cTransaction)) {
    }
    else{
//    		GPIO_write(Board_GPIO_LED0, Board_GPIO_LED_ON);
    }

    for (i=0; i<count; i+=1){
    		dest[i] = rxBuffer[i];
    }

    return count;
}


uint8_t checkID(void)
{
		uint8_t ID_1;

		ID_1 = I2Cwrite2read1HumByte(ADDRESS, 0xFC, 0xC9);

		return(ID_1);
}

 void beginHumidity(void)
{
	//  Wire.begin();

	  uint8_t ID_Temp_Hum = checkID();

	  int x = 0;

	  if(ID_Temp_Hum == 0x15)//Ping CheckID register
		x = 1;
	  else if(ID_Temp_Hum == 0x32)
		x = 2;
	  else
		x = 0;

	  if(x == 1)
	  {
	//    Serial.println("Si7021 Found");
//		Display_printf(display, 0, 0, "Si7021 Found");
		//Serial.println(ID_Temp_Hum, HEX);
	  }
	  else if(x == 2)
	  {
	//    Serial.println("HTU21D Found");
//		Display_printf(display, 0, 0, "HTU21D Found");
		//Serial.println(ID_Temp_Hum, HEX);
	  }
	  else
	  {
	//  	Serial.println("No Devices Detected");
//		Display_printf(display, 0, 0, "No Devices Detected");
		//Serial.println(ID_Temp_Hum, HEX);
	  }
}

 uint16_t getRawTemp(void) {
	 uint16_t measurement;
	 measurement = I2Cwrite1read2HumByte(ADDRESS, 0xE3);
	 return measurement;
 }

 uint16_t getRawHumidity(void) {
	 uint16_t measurement;
	 measurement = I2Cwrite1read2HumByte(ADDRESS, 0xE5);
	 return measurement;
 }

 float getTempFarenheit(void) {
	 uint16_t measurement;
	 float degrees_C;
	 float degrees_F;

	 measurement = I2Cwrite1read2HumByte(ADDRESS, 0xE3);
	 degrees_C = (175.72*measurement/65536)-46.85;
	 degrees_F = (degrees_C*1.8) + 32;

	 return degrees_F;
 }

 float getRelativeHumidity(void) {
	 uint16_t rel_hum_measurement;
	 float relative_humidity;

	 rel_hum_measurement = I2Cwrite1read2HumByte(ADDRESS, 0xE5);
	 relative_humidity = (125.0*rel_hum_measurement/65536)-6;

	 return relative_humidity;
 }

 void heaterOn(void) {
	 uint8_t regVal;
	 regVal = I2Cwrite1read1HumByte(ADDRESS, 0xE7);
	 regVal |= _BV(HTRE);
	 //turn on the heater
	 I2Cwrite2read1HumByte(ADDRESS, WRITE_USER_REG, regVal);
 }

 void heaterOff(void) {
	 uint8_t regVal;
	 regVal = I2Cwrite1read1HumByte(ADDRESS, 0xE7);
	 regVal &= ~_BV(HTRE);
	 //turn on the heater
	 I2Cwrite2read1HumByte(ADDRESS, WRITE_USER_REG, regVal);
 }







#endif /* TASKS_HUMIDITY_SI7021_H_ */
