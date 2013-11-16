#include <string.h>

// strlen - Returns String Length
size_t strlen(const char *str)
{
        const char *s;

        for (s = str; *s; ++s);

        return (s - str);
}

// strcpy - Copy String s2 to s1
char *strcpy(char *s1, const char *s2)
{
    char *s1_p = s1;
    while ((*s1++ = *s2++));
    return s1_p;
}

// memcpy - Copy count ammount of bytes from src to dest
void *memcpy(void *dest, const void *src, size_t count)
{
    const char *sp = (const char *)src;
    char *dp = (char *)dest;
    for(; count != 0; count--) *dp++ = *sp++;
    return dest;
}

// memset - Sets count bytes of dest to val
void *memset(void *dest, char val, size_t count)
{
    unsigned char *temp = (unsigned char *)dest;
	for( ; count != 0; count--, temp[count] = val);
	return dest;
}

// memsetq - Sets count bytes of dest to val
unsigned short *memsetw(unsigned short *dest, unsigned short val, size_t count)
{
    unsigned short *temp = (unsigned short *)dest;
    for( ; count != 0; count--)
		*temp++ = val;
    return dest;
}
