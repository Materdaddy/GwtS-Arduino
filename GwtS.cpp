/*
   GWTS.cpp - Library for driving GE Color Effects
   LED Holiday Lights with an Arduino

   Created by Paul Martis, October 1, 2011.
   http://www.digitalmisery.com

   Based on Christmas Light Control by Robert Quattlebaum <darco@deepdarc.com>
   (http://www.deepdarc.com/2010/11/27/hacking-christmas-lights)
   and Arduino adaptation by Scott Harris <scottrharris@gmail.com>
   http://scottrharris.blogspot.com/2010/12/controlling-ge-color-effects-lights.html
*/

#include "Arduino.h"
#include "GwtS.h"

#define PD2 2
#define PD3 3

byte codeLookup[5][5][5] = {
  {
   {0x1D, 0x1D, 0x04, 0x04, 0x04},
   {0x04, 0x04, 0x02, 0x02, 0x02},
   {0x02, 0x02, 0x01, 0x01, 0x01},
   {0x01, 0x01, 0x1A, 0x1A, 0x1A},
   {0x1A, 0x17, 0x17, 0x17, 0x16}
  },
  {
   {0x16, 0x16, 0x16, 0x16, 0x07},
   {0x07, 0x07, 0x07, 0x07, 0x07},
   {0x07, 0x07, 0x07, 0x07, 0x07},
   {0x07, 0x07, 0x07, 0x08, 0x08},
   {0x08, 0x08, 0x08, 0x08, 0x08}
  },
  {
   {0x08, 0x08, 0x08, 0x08, 0x08},
   {0x08, 0x08, 0x08, 0x08, 0x08},
   {0x08, 0x08, 0x08, 0x08, 0x08},
   {0x08, 0x08, 0x08, 0x08, 0x06},
   {0x06, 0x06, 0x06, 0x06, 0x06}
  },
  {
   {0x06, 0x06, 0x06, 0x06, 0x06},
   {0x06, 0x06, 0x06, 0x06, 0x06},
   {0x06, 0x06, 0x06, 0x06, 0x06},
   {0x06, 0x06, 0x00, 0x00, 0x00},
   {0x00, 0x00, 0x00, 0x00, 0x00}
  },
  {
   {0x15, 0x0D, 0x0D, 0x0C, 0x0A},
   {0x10, 0x10, 0x10, 0x10, 0x10},
   {0x11, 0x11, 0x11, 0x11, 0x11},
   {0x0F, 0x0F, 0x05, 0x05, 0x05},
   {0x12, 0x12, 0x1B, 0x1B, 0x1C}
  }
};

//#define DEBUG

unsigned char GWTS::calc_crc(unsigned char *data, unsigned char length) {
  // Courtesy of rjchu and Timon - A godsend
  unsigned char crc = 0;
  while(length--) {
    crc ^= *data++;
    unsigned n = 8;
    do crc = (crc & 1) ? (crc >> 1) ^ 0x8C : crc >> 1;
    while(--n);
  }
  return crc;
}

void GWTS::pulseIR(long microsecs, int hilo) {
  // we'll count down from the number of microseconds we are told to wait
  while (microsecs > 0) {
    // 38 kHz is about 13 microseconds high and 13 microseconds low
	if ( hilo )
		PORTD |= _BV(PD2);
	else
		PORTD &= ~_BV(PD2);
    delayMicroseconds(12);         // hang out for 12 microseconds
	int i;
	for ( i = 0; i < 50; i++ )
		continue;
	PORTD &= ~_BV(PD2);
    delayMicroseconds(12);         // hang out for 12 microseconds
    microsecs -= 26;
  }
}

void GWTS::sendbyte(byte b) {
#ifdef DEBUG
	Serial.print("Sending: ");
	if ( b < 0x10 ) {Serial.print('0');}
	Serial.println(b, HEX);
#endif
  byte i=0;
  // Send 8-N-1 data as the mouse ears communicate in.
  // Data consists of 1 Start Bit, 8 Data Bits, 1 Stop Bit, and NO parity bit
  // 1 bit is 417 microseconds @ 2400 baud
  PORTD |= _BV(PD3);
  PORTD &= ~_BV(PD3);
  pulseIR(400, HIGH); // Start bit
  PORTD |= _BV(PD3);
  PORTD &= ~_BV(PD3);
  while(i<8) {
    pulseIR(400, (b>>(i++)&1)?LOW:HIGH); // Data Bits
  }
  PORTD |= _BV(PD3);
  PORTD &= ~_BV(PD3);
  pulseIR(400, LOW); // Stop bit
  PORTD |= _BV(PD3);
  PORTD &= ~_BV(PD3);
}

byte GWTS::bytefromhex(char hexed[2]) {
  // This can be massaged if desired to allow better work. I'm just going to use a cheap ASCII offset.
  // This isn't "safe" or "pretty" programming but works for this purpose.
  if(hexed[1]>'9') hexed[1] -= 7; // (7 is the offset from 'A' to ':' which follows '9')
  if(hexed[0]>'9') hexed[0] -= 7;
  return (byte)(((hexed[0] - '0')<<4) + hexed[1] - '0');
}

GWTS::GWTS()
{
  pinMode(2, OUTPUT);
  pinMode(3, OUTPUT);

  cmdcount = 0;

  _lred = 0;
  _lgreen = 0;
  _lblue = 0;
  _rred = 0;
  _rgreen = 0;
  _rblue = 0;
}

void GWTS::set_colors(uint8_t lRed, uint8_t lGreen, uint8_t lBlue, uint8_t rRed, uint8_t rGreen, uint8_t rBlue)
{
  _lred   = floor(lRed/51);
  _lgreen = floor(lGreen/51);
  _lblue  = floor(lBlue/51);
  _rred   = floor(rRed/51);
  _rgreen = floor(rGreen/51);
  _rblue  = floor(rBlue/51);

  if (_lred > 4)   _lred = 4;
  if (_lgreen > 4) _lgreen = 4;
  if (_lblue > 4)  _lblue = 4;
  if (_rred > 4)   _rred = 4;
  if (_rgreen > 4) _rgreen = 4;
  if (_rblue > 4)  _rblue = 4;

#ifdef DEBUG
  //Serial.print("Left RGB Values are ");
  Serial.print("LR:");
  Serial.print(_lred, HEX);
  Serial.print(" G:");
  Serial.print(_lgreen, HEX);
  Serial.print(" B:");
  Serial.println(_lblue, HEX);
  //Serial.print("Right RGB Values are ");
  Serial.print("RR:");
  Serial.print(_rred, HEX);
  Serial.print(" G:");
  Serial.print(_rgreen, HEX);
  Serial.print(" B:");
  Serial.println(_rblue, HEX);
#endif

  char tempArray[2];
  if ((_lred == _rred) && (_lgreen == _rgreen) && (_lblue == _rblue)) {
    tempArray[0] = '9';
    tempArray[1] = '1';
    cmdbuf[0] = bytefromhex(tempArray); //0x91;
    tempArray[0] = '0';
    tempArray[1] = 'E';
    cmdbuf[1] = bytefromhex(tempArray); //0x0E;
    cmdbuf[2] = codeLookup[_lred][_lgreen][_lblue]; // Color code for both ears
    cmdcount = 3;
    checksum = calc_crc(cmdbuf, cmdcount);
  }
  else {
    tempArray[0] = '9';
    tempArray[1] = '4';
    cmdbuf[0] = bytefromhex(tempArray); //0x94;
    tempArray[0] = '0';
    tempArray[1] = 'E';
    cmdbuf[1] = bytefromhex(tempArray); //0x0E;
    cmdbuf[2] = codeLookup[_lred][_lgreen][_lblue]; // Color code for the left ear
    tempArray[0] = '0';
    tempArray[1] = 'E';
    cmdbuf[3] = bytefromhex(tempArray); //0x0E;
    cmdbuf[4] = codeLookup[_rred][_rgreen][_rblue]; // Add 0x80 to color code for the right ear
    tempArray[0] = '8';
    tempArray[1] = '0';
    cmdbuf[4] = cmdbuf[4] + bytefromhex(tempArray);
    cmdcount = 6;
    checksum = calc_crc(cmdbuf, cmdcount);
  }
#ifdef DEBUG
  int i = 0;
  while ( i < cmdcount )
  {
    if (cmdbuf[i]<0x10) {Serial.print("0");} 
    Serial.print(cmdbuf[i++], HEX);
  }
  if (checksum<0x10) {Serial.print("0");}
  Serial.print(checksum, HEX);
  Serial.println("");
#endif

  send_data();
}

void GWTS::send_data()
{
  cli();
  int j = 0;
  while (j < cmdcount) {
    sendbyte(cmdbuf[j++]);
  }
  sendbyte(checksum);
  sei();
}

