/*
* LCD.h
*  Author: duc
*/


#ifndef LCD_H_
#define LCD_H_

#define F_CPU 8000000UL

#include <avr/sfr_defs.h>
#include <avr/io.h>
#include <util/delay.h>
#include <string.h>

#define sbi(reg, bit) (reg |= (1<<bit))
#define cbi(reg, bit) (reg &= ~(1<<bit))

#define EN 			2
#define RW 			1
#define RS 			0
#define CTRL		PORTB
#define DDR_CTRL	DDRB

#define DATA_O		PORTB
#define DATA_I		PINB
#define DDR_DATA	DDRB

class LCD
{
	private:
	char Read2Nib();
	void Write2Nib(uint8_t ch);
	void Write8Bit(uint8_t ch);
	void Wait();
	public:
	LCD() {}
	~LCD() {return;}
	void Init();
	void Clear();
	void Home();
	void SetCursor(uint8_t y, uint8_t x);
	void PutChar(char ch);
	void Print(char* str);
};


#endif /* LCD_H_ */