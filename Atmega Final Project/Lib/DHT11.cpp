/*
* DHT11.cpp
* Author: duc
*/

// Source code: https://www.electronicwings.com/avr-atmega/dht11-sensor-interfacing-with-atmega16-32

#include "DHT11.h"

DHT11::DHT11()
{
	this->DHT_INPUTPIN = 0;
	this->Temperature = 0;
	this->Humidity = 0;
}

DHT11::DHT11(uint8_t dht_inputPin)
{
	this->DHT_INPUTPIN = dht_inputPin;
	this->Temperature = 0;
	this->Humidity = 0;
}

DHT11::~DHT11()
{

}

uint8_t DHT11::GetTemperature()
{
	return this->Temperature;
}

uint8_t DHT11::GetHumidity()
{
	return this->Humidity;
}

void DHT11::Request()
{
	sbi(DHT_DDR, DHT_INPUTPIN);		/* OUTPUT */
	cbi(DHT_PORT, DHT_INPUTPIN);	/* HIGH */
	_delay_ms(100);					/* wait for 20ms */
	sbi(DHT_PORT, DHT_INPUTPIN);
}

void DHT11::Response()
{
	cbi(DHT_DDR, DHT_INPUTPIN);		/* INPUT */
	while(DHT_PIN & (1<<DHT_INPUTPIN));
	while((DHT_PIN & (1<<DHT_INPUTPIN))==0);
	while(DHT_PIN & (1<<DHT_INPUTPIN));
}

bool DHT11::Checksum(uint8_t buffer[])
{
	uint8_t checksum = buffer[4];
	if ((uint8_t)(buffer[0] + buffer[1] + buffer[2] + buffer[3] == checksum))
	return true;
	return false;
}

void DHT11::Calculate()
{
	uint8_t buffer[5];
	
	Request();
	Response();
	
	for (int i = 0; i < 5 ; i++)
	{
		uint8_t result = 0;
		for (int j = 0; j < 8 ; j++)
		{
			while((DHT_PIN & (1<<DHT_INPUTPIN)) == 0);  /* check received bit 0 or 1 */
			_delay_us(30);
			if(DHT_PIN & (1<<DHT_INPUTPIN))				/* if high pulse is greater than 30ms */
				result = (result<<1)|(0x01);				/* then its logic HIGH */
			else										/* otherwise its logic LOW */
				result = (result<<1);
			while(DHT_PIN & (1<<DHT_INPUTPIN));
		}
		buffer[i] = result;
	}
	
	if (!Checksum(buffer)) /* error checksum */
	{
		this->Temperature = 0;
		this->Humidity = 0;
	}
	else /* correct checksum */
	{
		this->Humidity = buffer[0];
		this->Temperature = buffer[2];
	}
}
