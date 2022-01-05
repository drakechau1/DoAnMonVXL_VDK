/*
* Atmega Final Project.cpp
* Author : duc
*/

#pragma GCC diagnostic ignored "-Wwrite-strings"

#define F_CPU 8000000UL

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <stdio.h>
#include <stdlib.h>
#include "Lib/LCD.h"
#include "Lib/DHT11.h"
#include "Lib/MQ135.h"
#include "Lib/MQ2.h"

#define UINT_MAX   4294967295U

#define DHT_INPUT_PIN	7
#define MQ135_INPUT_PIN	0
#define MQ2_INPUT_PIN	1
#define BUZZER_PIN		6

#define GAS_THRESHOLD	300
#define NUMBER_OF_SENSOR 3


// Global variables
LCD lcd;	// Create LCD
DHT11 dht11(DHT_INPUT_PIN);		// Create DHT
MQ135 mq135(MQ135_INPUT_PIN);	// Create MQ135
MQ2 mq2(MQ2_INPUT_PIN);

uint8_t temperature, humidity;
float co2ppm;
float smoke, lpg, co;
char data[10];

char* ftoa(float f);
char* ftoa(float f, bool flag);
void CalculateSensor();
bool CheckGas(float lpg);
void ShowDHT11();
void ShowMQ135();
void ShowMQ2();

int main(void)
{
	// Init functions
	lcd.Init();
	mq135.Init();
	mq2.Init();

	sbi(DDRD, BUZZER_PIN);	// output BUZZER
	
	unsigned int count = 0;
	while (1)
	{
		if (count >	UINT_MAX)	// Avoid overloading
		count = 0;
		
		CalculateSensor();
		
		if (!CheckGas(lpg))
		{
			ShowMQ2();
		}
		else
		{
			if (count % NUMBER_OF_SENSOR == 0)
			ShowDHT11();
			else if (count % NUMBER_OF_SENSOR == 1)
			ShowMQ135();
			else
			ShowMQ2();
			count += 1;
		}
		
		_delay_ms(500);
	}
}




void CalculateSensor()
{
	dht11.Calculate();
	temperature = dht11.GetTemperature();
	humidity = dht11.GetHumidity();
	
	mq2.Calculate();
	smoke = mq2.GetSmoke();
	lpg = mq2.GetLPG();
	co = mq2.GetCO();
	
	co2ppm = mq135.GetPPM();
}

bool CheckGas(float lpg)
{
	if (lpg < GAS_THRESHOLD)
	return true;	// Safe
	return false;	// Unsafe
}

void ShowDHT11()
{
	lcd.Clear();
	
	lcd.SetCursor(1, 1);
	lcd.Print("Temp:");
	lcd.SetCursor(1, 6);
	lcd.Print(ftoa(temperature, 0));
	lcd.SetCursor(1, 9);
	lcd.PutChar(0xDF);
	lcd.SetCursor(1, 10);
	lcd.PutChar('C');
	
	lcd.SetCursor(2, 1);
	lcd.Print("Humi:");
	lcd.SetCursor(2, 6);
	lcd.Print(ftoa(humidity, 0));
	
	lcd.SetCursor(2, 9);
	lcd.PutChar('%');
}

void ShowMQ2()
{
	lcd.Clear();
	
	lcd.SetCursor(1, 1);
	lcd.Print("Smoke:");
	lcd.SetCursor(1, 7);
	lcd.Print(ftoa(smoke, 0));
	
	lcd.SetCursor(2, 1);
	lcd.Print("GAS:");
	lcd.SetCursor(2, 5);
	lcd.Print(ftoa(lpg, 0));
	
	lcd.SetCursor(2, 10);
	if (CheckGas(lpg))
	{
		lcd.Print("SAFE");
		cbi(PORTD, BUZZER_PIN);
	}
	else
	{
		sbi(PORTD, BUZZER_PIN);
		lcd.Print("UNSAFE");
	}
}

void ShowMQ135()
{
	lcd.Clear();
	lcd.SetCursor(1, 1);
	lcd.Print("C02:");
	lcd.Print(ftoa(co2ppm));
	lcd.SetCursor(2, 5);
	if (co2ppm < ATMOCO2)
	{
		lcd.Print("LOW CO2");
	}
	else if (mq135.GetPPM() < 1000.0)
	{
		lcd.Print("MEDIUM CO2");
	}
	else
	{
		lcd.Print("HIGH CO2");
	}
}

/* convert float to tring */
char *ftoa(float f)
{
	return ftoa(f, true);
}
char *ftoa(float f, bool flag)
{
	static char        buf[17];
	char *            cp = buf;
	unsigned long    l, rem;

	if(f < 0) {
		*cp++ = '-';
		f = -f;
	}
	l = (unsigned long)f;
	f = (f - (float)l);
	rem = (unsigned long)(f * 1e6);
	if (flag)
	sprintf(cp, "%lu.%lu", l, rem/10000);
	else
	sprintf(cp, "%lu", l);
	return buf;
}
/* convert float to tring */
