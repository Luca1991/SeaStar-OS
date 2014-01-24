#ifndef _KERNELDISPLAY_H
#define _KERNELDISPLAY_H

extern void kernelGetXY (unsigned* x, unsigned* y);
extern void kernelClrScr (const unsigned short c);
extern void kernelPuts (char* str);
extern int kernelPrintf (const char* str, ...);
extern unsigned kernelSetColor (const unsigned c);
extern void kernelGotoXY (unsigned x, unsigned y);
extern void kernelPutc (unsigned char c);

#endif
