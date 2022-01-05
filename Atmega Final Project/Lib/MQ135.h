/*
* MQ135.h
* Author: duc
*/


#ifndef __MQ135_H__
#define __MQ135_H__

#define F_CPU 8000000UL
#include <avr/io.h>
#include <util/delay.h>

#define RLOAD 22.0			/// The load resistance on the board
#define RZERO 76.63			/// Calibration resistance at atmospheric CO2 level
#define PARA 116.6020682	/// Parameters for calculating ppm of CO2 from sensor resistance
#define PARB 2.769034857

#define CORA 0.00035		/// Parameters to model temperature and humidity dependence
#define CORB 0.02718
#define CORC 1.39538
#define CORD 0.0018

#define ATMOCO2 419.13		/// Atmospheric CO2 level for calibration purposes. As of September 2020 https://www.co2.earth/

// VREF ADC MODE
#define AREF_MODE 0
#define AVCC_MODE (1<<REFS0)
#define INT_MODE (1<<REFS0)|(1<<REFS1)
#define ADC_VREF_TYPE AVCC_MODE

class MQ135
{
	private:
	uint8_t adc_channel;
	uint16_t ReadADC();
	float GetCorrectionFactor(float temperature, float humidity);
	float GetResistance();
	float GetCorrectedResistance(float temperature, float humidity);
	
	public:
	MQ135();
	MQ135(uint8_t channel);
	~MQ135();
	void Init();
	float GetRZero();
	float GetCorectedRZero(float temperature, float humidity);
	float GetPPM();
	float GetCorrectedPPM(float temperature, float humidity);
	
};

#endif //__MQ135_H__
