#include <Hal.h>
#include <stdarg.h>
#include "KernelDisplay.h"


void  kernel_panic (const char* fmt, ...) {
		
	/*va_list		args;
	va_start (args, fmt);
	va_end (args);*/

	disable_interrupt ();
	char* panicMessage="Sorry SeaStar encountered a critical problem and cannot continue\n\
			    Yeah this is a Kernel Panic. Any unsaved work is lost.\n\
			    Please restart your computer.\n\n";

	kernelClrScr (0x1f);
	kernelGotoXY (0,0);
	kernelSetColor (0x1f);
	kernelPuts (panicMessage);

	kernelPrintf ("*** STOP: ");
	int i=0;	
	for(i=0;fmt[i] != '\0';i++)
		kernelPrintf ("%c",fmt[i]);

	while(1);
}

