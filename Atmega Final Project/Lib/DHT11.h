/*
* DHT11.h
* Author: duc
*/


#ifndef __DHT11_H__
#define __DHT11_H__

#define F_CPU 8000000UL
#include <avr/io.h>
#include <avr/sfr_defs.h>
#include <util/delay.h>

#define DHT_DDR		DDRD
#define DHT_PORT	PORTD
#define DHT_PIN		PIND
#define DHT_TIMEOUT	200

#define sbi(reg, bit) (reg |= (1<<bit))
#define cbi(reg, bit) (reg &= ~(1<<bit))

class DHT11
{
private:
	uint8_t Temperature, Humidity;
	uint8_t DHT_INPUTPIN;
	void Request();
	void Response();
	bool Checksum(uint8_t buffer[]);

public:
	DHT11();
	DHT11(uint8_t dht_inputPin);
	~DHT11();
	uint8_t GetTemperature();
	uint8_t GetHumidity();
	void Calculate();
};

#endif
