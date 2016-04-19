/*
 * TWI.h
 *
 *  Created on: 21 Oct 2015
 *      Author: leo
 */

#ifndef TWI_H_
#define TWI_H_
//#define F_CPU 3200000
#define TWI_TIMEOUT 1000
#include <avr/io.h>

/*! Transaction result enumeration. */
typedef enum TWIM_RESULT_enum {
	TWIM_RESULT_UNKNOWN          = (0x00<<0),
	TWIM_RESULT_OK               = (0x01<<0),
	TWIM_RESULT_BUFFER_OVERFLOW  = (0x02<<0),
	TWIM_RESULT_ARBITRATION_LOST = (0x03<<0),
	TWIM_RESULT_BUS_ERROR        = (0x04<<0),
	TWIM_RESULT_NACK_RECEIVED    = (0x05<<0),
	TWIM_RESULT_FAIL             = (0x06<<0),
} TWIM_RESULT_t;

typedef enum TWIM_STATUS_enum {
	TWIM_STATUS_BUSY = (0x00<<0),
	TWIM_STATUS_READY = (0x01<<0),
	TWIM_STATUS_BEGIN_WRITE = (0x02<<0),
	TWIM_STATUS_BEGIN_READ = (0x04<<0),
	TWIM_STATUS_BEGIN_WRITEREAD = (0x08<<0)
} TWIM_STATUS_t;

typedef struct TWI_Data_struct{
	TWI_t * twi_port;
	PORT_t * port;
	unsigned long baud_hz;
	uint8_t maxDataLength;
	register8_t master_addr;
	register8_t result;
	register8_t status;
} TWI_Data;

class TWI {
private:
	TWI_Data * twi_data;
	TWI_t * twi_port;
	PORT_t * port;
	TWIM_STATUS_t twim_status;
	register8_t returnAddresses[127];
	void checkTWIStatus();
	register8_t getBaudVal(int baud);
public:
	TWI();
	TWI(TWI_Data * twi_data);
	TWIM_STATUS_t getTWIMStatus();
	void beginWrite(register8_t address);
	void beginRead(register8_t address);
	char beginReadFirstByte(register8_t address);
	void beginWriteRead(register8_t address, register8_t write_val);
	void putChar(char c);
	char getChar();
	void endTransmission();
	void writeData(register8_t address, const char * data, int length);
	char * readData(register8_t address);
	void begin(register8_t);
	void end();
	void setDataLength(uint8_t length);
	register8_t * pollBus();

	virtual ~TWI();
};

#endif /* TWI_H_ */
