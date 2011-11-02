#include <stdio.h>
#include <string.h>
#include "uart2.h"

static const char charMap[] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'};

// Unsigned long to ASCII hex
void ultoah(unsigned long n, char s[], unsigned char *charsWritten) {
	int i = 0;

	// Generate digits in reverse order
	do {
		s[i++] = charMap[n % 16];
	} while ((n /= 16) > 0);
	s[i++] = 'x';
	s[i++] = '0';
	s[i] = '\0';

	// Keep track of how many characters we've written
	*charsWritten = (unsigned char)i;

	// Now reverse the string
	int j;
	char c;
	for (j = i - 1, i = 0; i < j; i++, j--) {
		c = s[i], s[i] = s[j], s[j] = c;
	}
}

void initCommunications() {
	initUart2(42); // Initialize UART2 for 57600 baud.
}

// Enqueues characters into UART2 for displaying a number.
void enqueueNumberText(unsigned long num) {
	char text[20] = {'h', 'e', 'y'};
	unsigned char c = 0;
	ultoah(num, text, &c);
	text[c++] = '\n';
	text[c] = '\0';
	uart2EnqueueData((unsigned char *)text, c);
}

// Enqueues characters into UART2 for displaying a number.
void enqueueData(char num) {
	char text[20] = "Load: ";
	unsigned char c;
	ultoah(num, &text[6], &c);
	c += 6;
	text[c++] = '\n';
	text[c] = '\0';
	uart2EnqueueData((unsigned char *)text, c);
}

