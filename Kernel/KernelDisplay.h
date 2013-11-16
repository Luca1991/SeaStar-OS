#ifndef _KERNELDISPLAY_H
#define _KERNELDISPLAY_H

extern void kernelClrScr (const unsigned short c);
extern void kernelPuts (char* str);
extern int kernelPrintf (const char* str, ...);
extern unsigned kernelSetColor (const unsigned c);
extern void kernelGotoXY (unsigned x, unsigned y);

#endif
