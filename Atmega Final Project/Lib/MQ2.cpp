/*
* MQ2.cpp
* Author: duc
*/


#include "MQ2.h"

MQ2::MQ2()
{
	this->adc_channel = 1;
}

MQ2::MQ2(uint8_t adc_channel)
{
	this->adc_channel = adc_channel;
}

MQ2::~MQ2()
{
}

void MQ2::Init()
{
	DDRA = 0x00;		/* DDRA mode: input */
	ADCSRA = 0x87;		/* Enable ADC, fr/128  */
	ADMUX = 0x40;		/* Vref: Avcc*/
	
	Ro = 6.69;
	//Ro = MQCalibration();
}

void MQ2::Close()
{
	Ro = -1.0;
}

uint16_t MQ2::ReadADC()
{
	ADMUX = adc_channel|ADC_VREF_TYPE;	/* Set input channel to read */
	ADCSRA |= (1<<ADSC);				/* Start conversion */
	while((ADCSRA&(1<<ADIF))==0);		/* Monitor end of conversion interrupt */
	_delay_us(10);
	return ADCW;						/* Return ADC value */
}

float MQ2::MQResistanceCalculation(uint16_t raw_adc)
{	if (raw_adc == 0 || raw_adc == 1023)
	return 0.0;
	return ((float)(RL_VALUE * (1023.0 - (float)raw_adc) / (float)raw_adc));
}

float MQ2::MQRead()
{
	float rs = 0.0;	// rs: Resistance
	for (int i = 0; i < READ_SAMPLE_TIMES; i++) {
		rs = rs + MQResistanceCalculation(ReadADC());
		_delay_ms(READ_SAMPLE_INTERVAL);
	}
	return (rs / ((float) READ_SAMPLE_TIMES));	// return the average
}

float MQ2::MQCalibration()
{
	float val = 0.0;
	// take multiple samples
	for (uint8_t i = 0; i < CALIBARAION_SAMPLE_TIMES; i++) {
		val = val + MQResistanceCalculation(ReadADC());
		_delay_ms(CALIBRATION_SAMPLE_INTERVAL);
	}
	val = val / ((float)CALIBARAION_SAMPLE_TIMES);		// calculate the average value
	val = (val / (float)RO_CLEAN_AIR_FACTOR);
	return val;	// divided by RO_CLEAN_AIR_FACTOR yields the Ro according to the chart in the datasheet
}

//
// LPGCurve   = {2.3, 0.21, -0.47};
// COCurve	  = {2.3, 0.72, -0.34};
// SmokeCurve = {2.3, 0.53, -0.44};
// return pow(10.0, ((log(rs_ro_ratio) - pcurve[1]) / pcurve[2]) + pcurve[0]);

void MQ2::Calculate()
{
	float rs_ro_ratio = MQRead() / Ro;
	this->LPG = pow(10.0, ((log(rs_ro_ratio) - 0.21) / -0.47) + 2.3);
	this->CO = pow(10.0, ((log(rs_ro_ratio) - 0.72) / -0.34) + 2.3);
	this->Smoke = pow(10.0, ((log(rs_ro_ratio) - 0.53) / -0.44) + 2.3);
}

float MQ2::GetLPG()
{
	return this->LPG;
}

float MQ2::GetCO()
{
	return this->CO;
}

float MQ2::GetSmoke()
{
	return this->Smoke;
}

float MQ2::GetRo()
{
	//return this->Ro;
	return MQCalibration();
}