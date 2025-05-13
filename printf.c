#include <stdarg.h>

/* Simple printf.
 * It only has 3 triggers:
 *  %s to inject a string
 *  %d to inject a decimal number
 *  %h to inject a 32 bit hex value as xxxxyyyy
 */


#define PRINTF_BUF_SIZE 128
static void asnprintf(char *abuf, unsigned int size, const char *fmt, va_list args);

void printf(char *fmt, ...) {
	char buf[PRINTF_BUF_SIZE];
	va_list args;

	va_start(args, fmt);
	asnprintf(buf, PRINTF_BUF_SIZE, fmt, args);
	va_end(args);

	puts(buf);
}

/* The limit is absurd, so take care */
void sprintf(char *buf, char *fmt, ...) {
	va_list args;

	va_start(args, fmt);
	asnprintf(buf, 256, fmt, args);
	va_end(args);
}


#define PUTCHAR(x)           \
	if (buf <= end)          \
	*buf++ = (x)

static const char hex_table[] = "0123456789ABCDEF";

#define HEX(x) hex_table[(x)]

static char *sprintn(char *buf, char *end, unsigned n) {
	char prbuf[16];
	char *cp;

	cp = prbuf;

	do {
		*cp++ = hex_table[n % 10];
		n /= 10;
	} while (n);

	do {
		PUTCHAR(*--cp);
	} while (cp > prbuf);

	return buf;
}

static char *shex2(char *buf, char *end, int val) {
	PUTCHAR(HEX((val >> 4) & 0xf));
	PUTCHAR(HEX(val & 0xf));
	return buf;
}

#ifdef notdef
static char *shex3(char *buf, char *end, int val) {
	PUTCHAR(HEX((val >> 8) & 0xf));
	return shex2(buf, end, val);
}

static char *shex4(char *buf, char *end, int val) {
	buf = shex2(buf, end, val >> 8);
	return shex2(buf, end, val);
}
#endif

static char *shex8(char *buf, char *end, int val) {
	buf = shex2(buf, end, val >> 24);
	buf = shex2(buf, end, val >> 16);
	buf = shex2(buf, end, val >> 8);
	return shex2(buf, end, val);
}

static void asnprintf(char *abuf, unsigned int size, const char *fmt, va_list args) {
	char *buf, *end;
	int c;
	char *p;

	buf = abuf;
	end = buf + size - 1;
	if (end < buf - 1) {
		end = ((void *) -1);
		size = end - buf + 1;
	}

	while (c = *fmt++) {
		if (c != '%') {
			PUTCHAR(c);
			continue;
		}
		c = *fmt++;
		if (c == 'd') {
			buf = sprintn(buf, end, va_arg(args, unsigned));
			continue;
		}
		if (c == 'x') {
			buf = shex2(buf, end, va_arg(args, int) & 0xff);
			continue;
		}
		if (c == 'h' || c == 'X') {
			buf = shex8(buf, end, va_arg(args, int));
			continue;
		}
		if (c == 'c') {
			PUTCHAR(va_arg(args, int));
			continue;
		}
		if (c == 's') {
			p = va_arg(args, char *);
			// printf ( "Got: %s\n", p );
			while (c = *p++)
				PUTCHAR(c);
			continue;
		}
	}
	if (buf > end)
		buf = end;
	PUTCHAR('\0');
}
