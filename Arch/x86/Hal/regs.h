#ifndef _REGS_H_INCLUDED
#define _REGS_H_INCLUDED

#include  <stdint.h>

//32Bit Regs
struct _R32BIT{
	uint32_t eax,ebx,ecx,edx,esi,edi,ebp,esp,eflags;
	uint8_t cflag;
};

//16Bit Regs
struct _R16BIT{
	uint16_t ax,bx,cx,dx,si,di,bp,sp,es,cs,ss,ds,flags;
	uint8_t cflags;
};

//16Bit Regs expressed in 32Bit regs
struct _R16BIT32{
	uint16_t ax,axh,bx,bxh,cx,cxh,dx,dxh;
	uint16_t si,di,bp,sp,es,cs,ss,ds,flags;
	uint8_t cflags;
};

struct _R8BIT{
	uint8_t al,ah,bl,bh,cl,ch,dl,dh;
};

//8Bit Regs expressed in 32Bit regs
struct _R8BIT32{
	uint8_t al,ah;
	uint16_t axh;
	uint8_t bl,bh;
	uint16_t bxh;
	uint8_t cl,ch;
	uint16_t cxh;
	uint8_t dl,dh;
	uint16_t dxh;
};


// 16Bit and 8Bit Regs Union
union _INTR16{
	struct _R16BIT x;
	struct _R8BIT h;
};

// 32Bit, 16Bit and 8Bit Regs Union
union _INTR32{
	struct _R32BIT x;
	struct _R16BIT32 l;
	struct _R8BIT32 h;
};


#endif
