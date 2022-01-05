/*
* LCD.cpp
*  Author: duc
*/


#include "LCD.h"

char LCD::Read2Nib()
{
	char HNib, LNib;
	DATA_O |=0xF0;
	
	sbi(CTRL,EN); //enable
	DDR_DATA &=0x0F;
	HNib=DATA_I & 0xF0;
	cbi(CTRL,EN); //disable
	
	sbi(CTRL,EN); //enable
	LNib = DATA_I & 0xF0;
	cbi(CTRL,EN); //disable
	LNib>>=4;
	return (HNib|LNib);
}

void LCD::Write2Nib(uint8_t ch)
{
	uint8_t HNib, LNib, temp_data;
	temp_data=DATA_O & 0x0F;

	HNib=ch & 0xF0;
	LNib=(ch<<4) & 0xF0;
	
	DATA_O =(HNib |temp_data);
	sbi(CTRL,EN); //enable
	cbi(CTRL,EN); //disable
	
	DATA_O =(LNib|temp_data);
	sbi(CTRL,EN); //enable
	cbi(CTRL,EN); //disable
}

void LCD::Write8Bit(uint8_t ch)
{
	DATA_O=ch;   //out 8 bits to DATA Line
	sbi(CTRL,EN); //enable
	cbi(CTRL,EN); //disable
}

void LCD::Wait()
{
	#ifdef LCD8BIT
	while(1){
		cbi(CTRL,EN); //disable before doing sth
		cbi(CTRL,RS); //RS=0 mean the following data is COMMAND (not normal DATA)
		sbi(CTRL,RW); //the Direction of this COMMAND is from LCD -> chip
		
		DDR_DATA=0xFF;//Ready to Out
		DATA_O=0xFF; //Out the COMMAND, 0xFF means "Read Status Bit" COMMAND
		sbi(CTRL,EN); //enable
		
		DDR_DATA=0x00;//Ready to IN
		if(bit_is_clear(DATA_I,7)) break;
	}
	cbi(CTRL,EN); //disable for next step
	cbi(CTRL,RW); //ready for next step
	DDR_DATA=0xFF;//Ready to Out
	#else
	char temp_val;
	while(1){
		cbi(CTRL,RS); //RS=0 mean the following data is COMMAND (not normal DATA)
		sbi(CTRL,RW); //the Direction of this COMMAND is from LCD -> chip
		temp_val=Read2Nib();
		if (bit_is_clear(temp_val,7)) break;
	}
	cbi(CTRL,RW); //ready for next step
	DDR_DATA=0xFF;//Ready to Out
	#endif
	
	//_delay_ms(1);
}

void LCD::Init()
{
	DDR_CTRL=0xFF;
	DDR_DATA=0xFF;
	//Function set------------------------------------------------------------------------------
	cbi(CTRL,RS);   // the following data is COMMAND
	cbi(CTRL, RW); // AVR->LCD
	cbi(CTRL, EN);
	#ifdef LCD8BIT
	Write8Bit(0x38);
	Wait();
	#else
	sbi(CTRL,EN); //enable
	sbi(DATA_O, 5);
	cbi(CTRL,EN); //disable
	Wait();
	Write2Nib(0x28);//
	Wait();
	#endif
	//Display control-------------------------------------------------------------------------
	cbi(CTRL,RS); // the following data is COMMAND
	#ifdef LCD8BIT
	Write8Bit(0x0E);
	Wait();
	#else
	Write2Nib(0x0E);
	Wait();
	#endif
	//Entry mode set------------------------------------------------------------------------
	cbi(CTRL,RS); // the following data is COMMAND
	#ifdef LCD8BIT
	Write8Bit(0x06);
	Wait();
	#else
	Write2Nib(0x06);
	Wait();
	#endif
}

void LCD::Clear()
{
	cbi(CTRL,RS); //RS=0 mean the following data is COMMAND (not normal DATA)
	#ifdef LCD8BIT
	Write8Bit(0x01);
	Wait();
	#else
	Write2Nib(0x01);
	Wait();
	
	#endif
}

void LCD::Home()
{
	cbi(CTRL,RS); // the following data is COMMAND
	#ifdef LCD8BIT
	Write8Bit(0x02);
	Wait();
	#else
	Write2Nib(0x02);
	Wait();
	#endif
}

void LCD::SetCursor(uint8_t y, uint8_t x)
{
	uint8_t Ad;
	Ad=64*(y-1)+(x-1)+0x80; //
	cbi(CTRL,RS); // the following data is COMMAND
	#ifdef LCD8BIT
	Write8Bit(Ad);
	Wait();
	#else
	Write2Nib(Ad);
	Wait();
	#endif
}

void LCD::PutChar(char ch)
{
	sbi(CTRL,RS); //this is a normal DATA
	#ifdef LCD8BIT
	Write8Bit(ch);
	Wait();
	#else
	Write2Nib(ch);
	Wait();
	#endif
}

void LCD::Print(char* str)
{
	unsigned char i;
	for (i=0; i<strlen(str); i++) {
		if(str[i] > 0) PutChar(str[i]);
		else PutChar(' ');
	}
}
