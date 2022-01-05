/*
* MQ135.cpp
* Author: duc
*/

// Reference: 
//		https://github.com/apassiou/mq135_atmega32
//		http://www.hocavr.com/2018/06/bai-6-chuyen-oi-adc.html


#include "MQ135.h"

MQ135::MQ135()
{
	this->adc_channel = 0;
}

MQ135::MQ135(uint8_t adc_channel)
{
	this->adc_channel = adc_channel;
}

MQ135::~MQ135()
{
}

void MQ135::Init()
{
	DDRA = 0x00;		/* DDRA mode: input */
	ADCSRA = 0x87;		/* Enable ADC, fr/128  */ 
	ADMUX = 0x40;		/* Vref: Avcc*/
}

uint16_t MQ135::ReadADC()
{
	//ADMUX=ADMUX|(channel & 0x0f);	/* Set input channel to read */
	ADMUX = adc_channel|ADC_VREF_TYPE;
	ADCSRA |= (1<<ADSC);			/* Start conversion */
	while((ADCSRA&(1<<ADIF))==0);	/* Monitor end of conversion interrupt */
	_delay_us(10);
	return ADCW;					// Return ADC value
}

float MQ135::GetCorrectionFactor(float temperature, float humidity)
{
	return CORA * temperature * temperature - CORB * temperature + CORC - (humidity-33.)*CORD;
}

float MQ135::GetResistance()
{
	uint16_t adcValue = ReadADC();
	return ((1023./(float)adcValue) - 1.)*RLOAD;
}

float MQ135::GetCorrectedResistance(float temperature, float humidity)
{
	return GetResistance()/GetCorrectionFactor(temperature, humidity);
}

float MQ135::GetRZero()
{
	return GetResistance() * pow((ATMOCO2/PARA), (1./PARB));
}

float MQ135::GetCorectedRZero(float temperature, float humidity)
{
	return GetCorrectedResistance(temperature, humidity) * pow((ATMOCO2/PARA), (1./PARB));
}

float MQ135::GetPPM()
{
	return PARA * pow((GetResistance()/RZERO), -PARB);
}

float MQ135::GetCorrectedPPM(float temperature, float humidity)
{
	return PARA * pow((GetCorrectedResistance(temperature, humidity)/RZERO), -PARB);
}
