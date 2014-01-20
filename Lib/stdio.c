#include <string.h>
#include <stdio.h>
#include <ctype.h>
#include <limits.h>



extern void itoa_s(int i,unsigned base,char* buf);


//! writes formatted string to buffer
int vsprintf(char *str, const char *format, va_list ap) {

	if (!str)
		return 0;

	if (!format)
		return 0;

	size_t loc=0;
	size_t i;

	for (i=0 ; i<=strlen(format);i++, loc++) {

		switch (format[i]) {

			case '%':

				switch (format[i+1]) {

					// characters 
					case 'c': {
						char c = va_arg (ap, char);
						str[loc] = c;
						i++;
						break;
					}

					// integers
					case 'd':
					case 'i': {
						int c = va_arg (ap, int);
						char s[32]={0};
						itoa_s (c, 10, s);
						strcpy (&str[loc], s);
						loc+= strlen(s) - 2;
						i++;		// next character
						break;
					}

					// hex
					case 'X':
					case 'x': {
						int c = va_arg (ap, int);
						char s[32]={0};
						itoa_s (c,16,s);
						strcpy (&str[loc], s);
						i++;		// next character
						loc+=strlen(s) - 2;
						break;
					}

					// strings
					case 's': {
						int c = (int) va_arg (ap, char); // FIXME
						char s[32]={0};
						strcpy (s,(const char*)c);						
						strcpy (&str[loc], s);
						i++;		// next character
						loc+=strlen(s) - 2;
						break;
					}
				}
				break;

			default:
				str[loc] = format[i];
				break;
		}
	}

	return i;
}

// Convert a string to a long
long strtol(const char* nptr, char** endptr, int base){
	const char *s = nptr;
	unsigned long acc;
	int c;
	unsigned long cutoff;
	int neg = 0, any, cutlim;
	/* Skipe white spaces and pick up leading +/- sign.
	   If base is 0, allow 0x for hex and 0 for octal,
	   else assume decimal. If base is 16, allow 0x.
	*/
	do{
		c = *s++;	
	}while (isspace(c));
	if(c == '-'){
		neg = 1;
		c = *s++;	
	}
	else if (c == '+')
		c = *s++;
	if((base == 0 || base == 16) && c=='0' && (*s == 'x' || *s == 'X')){
		c = s[1];
		s += 2;
		base = 16;
	}
	else if ((base == 0 || base == 2) && c=='0' && (*s == 'b' || *s == 'B')){
		c = s[1];
		s+=2;
		base = 2;
	}
	if(base==0)
		base = c == '0' ? 8:10;

	cutoff = neg ? -(unsigned long)LONG_MIN : LONG_MAX;
	cutlim = cutoff % (unsigned long)base;
	cutoff /= (unsigned long)base;
	for(acc = 0, any=0;;c=*s++){
		if(isdigit(c))
			c-='0';
		else if(isalpha(c))
			c-= isupper(c) ? 'A'-10 : 'a'-10;
		else
			break;
		if(any<0 || acc> cutoff || acc == cutoff && c > cutlim)
			any=-1;
		else{
			any = 1;
			acc *= base;
			acc += c;
		}
	}
	if(any<0){
		acc = neg ? LONG_MIN : LONG_MAX;
	}
	else if (neg)
		acc = -acc;
	if(endptr != 0)
		*endptr = (char *)(any ? s-1:nptr);
	return (acc);
}

// Convert a string to unsigned long
unsigned long strtoul(const char* nptr, char** endptr, int base){
	const char *s = nptr;
	unsigned long acc;
	int c;
	unsigned long cutoff;
	int neg=0;
	int any, cutlim;

	do{
		c = *s++;	
	}while(isspace(c));
	if(c=='-'){
		neg = 1;
		c = *s++;
	}
	else if (c=='+')
		c = *s++;
	if((base == 0 || base == 16) && c == '0' && (*s == 'x' || *s == 'X')){
		c = s[1];
		s += 2;
		base = 16;
	}
	else if ((base==0 || base==2) && c=='0' && (*s == 'b' || *s == 'B')){
		c = s[1];
		s += 2;
		base = 2;
	}
	if(base == 0)
		base = c == '0' ? 8:10;
	cutoff = (unsigned long)ULONG_MAX / (unsigned long)base;
	cutlim = (unsigned long)ULONG_MAX % (unsigned long)base;
	for(acc=0,any=0;;c=*s++){
		if(isdigit(c))
			c -= '0';
		else if (isalpha(c))
			c -= isupper(c) ? 'A' - 10 : 'a' - 10;
		else
			break;
		if (c >= base)
			break;
		if(any<0 || acc > cutoff || acc == cutoff && c > cutlim)
			any = -1;
		else{
			any = 1;
			acc *= base;
			acc += c;
		}
	}
	if (any < 0){
		acc = ULONG_MAX;
	}
	else if (neg)
		acc = -acc;
	if(endptr != 0)
		*endptr = (char *)(any ? s-1 : nptr);
	return (acc);
}

int atoi(const char * str){
	return (int) strtol (str,0,10);
}
