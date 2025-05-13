#include <stdint.h>
#include <stddef.h>

/* need only for RTT */

void *memcpy(void *dest, const void *src, size_t n) {
	unsigned int *dword_dest = (unsigned int *) dest;
	const unsigned int *dword_src = (const unsigned int *) src;

	while (n >= 4) {
		*dword_dest++ = *dword_src++;
		n -= 4;
	}

	unsigned char *byte_dest = (unsigned char *) dword_dest;
	const unsigned char *byte_src = (const unsigned char *) dword_src;
	while (n--) {
		*byte_dest++ = *byte_src++;
	}
	return dest;
}

void *memset(void *s, int c, size_t n) {
	unsigned int *dword_ptr = (unsigned int *) s;
	unsigned int fill_value = (unsigned char) c;
	fill_value |= fill_value << 8;
	fill_value |= fill_value << 16;

	while (n >= 4) {
		*dword_ptr++ = fill_value;
		n -= 4;
	}

	unsigned char *byte_ptr = (unsigned char *) dword_ptr;
	while (n--) {
		*byte_ptr++ = (unsigned char) c;
	}
	return s;
}


size_t strlen(const char *s) {
	const char *start = s;
	while ((uintptr_t) s % 4) { // Вирівнюємо адресу
		if (*s == '\0')
			return s - start;
		s++;
	}

	const uint32_t *word_ptr = (const uint32_t *) s;
	while (1) {
		uint32_t chunk = *word_ptr++;
		if ((chunk - 0x01010101) & ~chunk & 0x80808080) {
			s = (const char *) (word_ptr - 1);
			while (*s)
				s++;
			return s - start;
		}
	}
}