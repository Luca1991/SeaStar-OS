#include <string.h>
#include <stdio.h>



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
