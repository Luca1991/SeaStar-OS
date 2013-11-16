#include <stdarg.h>
#include <string.h>
#include "KernelDisplay.h"


// ATTENTION: Some systems may require 0xB0000 instead of 0xB8000 !!!
#define VID_MEMORY	0xB8000


static unsigned int _xPos=0, _yPos=0;
static unsigned _startX=0, _startY=0;

static unsigned _color=0; // current color


void kernelPutc (unsigned char c) {

	if (c==0)
		return;

	if (c == '\n'||c=='\r') {	// we need a new line :)
		_yPos+=2;
		_xPos=_startX;
		return;
	}

	if (_xPos > 79) {		// row is full, we need a new line :)
		_yPos+=2;
		_xPos=_startX;
		return;
	}

	unsigned char* p = (unsigned char*)VID_MEMORY + (_xPos++)*2 + _yPos * 80; // grab current coordinate in vid_men
	*p++ = c; // add our character to the next position in video memory
	*p =_color; // set the color
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

	// reposition starting vectors for next text to follow
	// multiply by 2 do to the video modes 2byte per character layout
	_xPos = x*2;
	_yPos = y*2;
	_startX=_xPos;
	_startY=_yPos;
}

void kernelClrScr (const unsigned short c) {

	unsigned char* p = (unsigned char*)VID_MEMORY;
	
	int i=0;
	for (i=0; i<160*30; i+=2) {

		p[i] = ' '; 
		p[i+1] = c;
	}

	// go to start of previous set vector
	_xPos=_startX;_yPos=_startY;
}

void kernelPuts (char* str) {

	if (!str)
		return;
	
	size_t i=0;
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
						int c = (int)  va_arg (args, char);//(int&) FIXME
						char str[32]={0};
						itoa_s (c, 16, str);
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
	return 0;
}

