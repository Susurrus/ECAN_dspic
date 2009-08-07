#include <stdio.h>
#include <stdarg.h>
#include <string.h>

// #define DEBUG

#include "circBuffer.h"

tCanMessage canMsg;

struct CircBuffer testBuffer;
CBRef ecanBuffer;

int main(int argc, char* argv[])
{
	// initialize the buffer
	ecanBuffer = (struct CircBuffer* )&testBuffer;
	newCircBuffer(ecanBuffer);
		
	canMsg.payload[0] = 12;
	canMsg.validBytes = 1;
	canMsg.id.ulData = 1234;	
	
	writeBack(ecanBuffer, canMsg);
	
	canMsg.payload[1] = 14;
	canMsg.payload[2] = 16;
	canMsg.payload[3] = 18;
	canMsg.validBytes = 4;
	canMsg.id.ulData = 4321;	
		
	writeBack(ecanBuffer, canMsg);
	
	printCircBuf(ecanBuffer);
	
	getLength(ecanBuffer);
	
	return 0;
}