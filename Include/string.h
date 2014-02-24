#ifndef _STRING_H
#define _STRING_H


#include <size_t.h>


extern int strcmp (const char* str1, const char* str2);
extern size_t strlen ( const char* str );
extern char *strcpy (char *s1,const char *s2);

extern void* memcpy(void *dest, const void *src, size_t count);
extern void *memset(void *dest, char val, size_t count);
extern unsigned short* memsetw(unsigned short *dest, unsigned short val, size_t count);
extern char* strchr(char *str, int character);

extern char* strcat(char *dest, const char *src);

int strncmp(const char * s1, const char * s2, size_t n);

#endif
