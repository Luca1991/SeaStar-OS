#include <stdarg.h>
#include <Hal.h>
#include <string.h>
#include "KernelDisplay.h"


// ATTENTION: Some systems may require 0xB0000 instead of 0xB8000 !!!
#define VID_MEMORY	0xB8000
uint16_t *video_memory = (uint16_t *)0xB8000;

static unsigned int _xPos=0, _yPos=0;

static unsigned _color=0; // current color

void kernelUpdateCursor(int x,int y){
	uint16_t cursorLocation = y * 80 + x;
	disable_interrupt();
	outportb(0x3D4, 14);
	outportb(0x3D5, cursorLocation >> 8);
	outportb(0x3D4, 15);
	outportb(0x3D5, cursorLocation);
	enable_interrupt();
}

void scroll(){
	int i=0;
	if(_yPos>=25){
		uint16_t attribute =0;// _color << 8;
		for(i=0;i<24*80;i++)
			video_memory[i]=video_memory[i+80];
		for(i=24*80;i<25*80;i++)
			video_memory[i] = attribute | ' ';

		_yPos = 24;	
	}
}

void kernelGetXY (unsigned* x, unsigned* y) {

	if (x==0 || y==0)
		return;

	*x = _xPos;
	*y = _yPos;
}


void kernelPutc (unsigned char c) {

	uint16_t attribute = _color << 8;

	if(c==0x80 && _xPos)
		_xPos--;
	
	else if(c==0x09)
		_xPos = (_xPos+8) & ~(8-1);


	else if(c == '\r')
		_xPos = 0;

	else if (c == '\n'){
		_xPos = 0;
		_yPos++;
	}

	if(c>=' ') {	
		uint16_t* location = video_memory + (_yPos*80 + _xPos);
		*location = c | attribute;
		_xPos++;
	}

	if (_xPos >= 80) {		// row is full, we need a new line :)
		_xPos=0;
		_yPos++;
	}

	if(_yPos>=25)
		scroll();

	kernelUpdateCursor(_xPos,_yPos);
}

char tbuf[32];
char bchars[] = {'0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F'};

void itoa(unsigned i,unsigned base,char* buf) {
   int pos = 0;
   int opos = 0;
   int top = 0;

   if (i == 0 || base > 16) {
      buf[0] = '0';
      buf[1] = '\0';
      return;
   }

   while (i != 0) {
      tbuf[pos] = bchars[i % base];
      pos++;
      i /= base;
   }
   top=pos--;
   for (opos=0; opos<top; pos--,opos++) {
      buf[opos] = tbuf[pos];
   }
   buf[opos] = 0;
}

void itoa_s(int i,unsigned base,char* buf) {
   if (base > 16) return;
   if (i < 0) {
      *buf++ = '-';
      i *= -1;
   }
   itoa(i,base,buf);
}



unsigned kernelSetColor (const unsigned c) {

	unsigned t=_color;
	_color=c;
	return t;
}

void kernelGotoXY (unsigned x, unsigned y) {
	if(_xPos <= 80)
		_xPos = x;
	if(_yPos <=25)
		_yPos = y;

	kernelUpdateCursor(_xPos,_yPos);
}

int displayGetHorz(){
	return 80;
}

int displayGetVert(){
	return 24;
}

void kernelClrScr (const unsigned short c) {

	
	int i=0;
	for (i=0; i<80*25; i++) {
		video_memory[i] = ' ' | (c<<8);
	}

	// go to start of previous set vector
	kernelGotoXY(0,0);
}

void kernelPuts (char* str) {

	if (!str)
		return;
	
	unsigned int i=0;
	for (i=0; i<strlen(str); i++)
		kernelPutc (str[i]);
}

int kernelPrintf (const char* str, ...) {

	if(!str)
		return 0;

	va_list		args;
	va_start (args, str);

	size_t i=0;
	for (i=0; i<strlen(str);i++) {

		switch (str[i]) {

			case '%':

				switch (str[i+1]) {


					/*** characters ***/
					case 'c': {
						char c =(char) va_arg (args, int);
						kernelPutc (c);
						i++;		// go to next character
						break;
					}

					/*** address of ***/
					case 's': {
						char str[32]={0};
						strcpy(str, va_arg (args, char *));
						kernelPuts (str);
						i++;		// go to next character
						break;
					}

					/*** integers ***/
					case 'd':
					case 'i': {
						int c = va_arg (args, int);
						char str[32]={0};
						itoa_s (c, 10, str);
						kernelPuts (str);
						i++;		// go to next character
						break;
					}

					/*** display in hex ***/
					case 'X':
					case 'x': {
						int c = va_arg (args, int);
						char str[32]={0};
						itoa_s (c,16,str);
						kernelPuts (str);
						i++;		// go to next character
						break;
					}

					default:
						va_end (args);
						return 1;
				}

				break;

			default:
				kernelPutc (str[i]);
				break;
		}

	}

	va_end (args);
	return i;
}

