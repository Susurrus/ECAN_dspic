// ==============================================================
// circBuffer.c
// This is the implementation file for the circular buffer used in
// the ECAN module. The buffer is BSIZE long and the enqued members
// are tCanMessage type.
//
// Convention Notes
//
// The HEAD, as in any queue points to the next available byte to be READ
// The TAIL, points to the next available position to be written
// 
// Code by: Mariano I. Lizarraga
// First Revision: Aug 16 2008 @ 00:36
// Second Revision: Dec 2 2008 @ 12:11
// Third Revision: July 29 2009 @ 21:19
// Fourth Revision: August 6 2009 @ 21:57
// ===============================================================
#include "circBuffer.h"

// Constructors - Destructors
// ==========================
// this Function creates a new Circular Buffer 

void newCircBuffer (CBRef cB){
	
	// initialize to zero
	uint8_t i;
	for (i=0; i<BSIZE; i++){
		memset(&(cB->buffer[i]), 0, sizeof(tCanMessage));
	}
			
	// initialize the data members
	cB->head = 0;
	cB->tail = 0;
	cB->size = BSIZE;
	cB->overflowCount = 0;

}

// this function frees the Circular Buffer CB Ref
void freeCircBuffer (CBRef* cB){
	// if it is already null, nothing to free
	if (cB == NULL || *cB == NULL) {return;}
			
	// free and nil the pointer
	*cB = NULL;
}
	


// Accesor Methods
// ===============

// returns the amount of unread bytes in the circular buffer
uint16_t getLength (CBRef cB){	
	// if the circular buffer is not null
	if (cB != NULL){
		if (cB->head <= cB->tail){
			return (cB->tail-cB->head);
		} else{
			return (cB->size + cB->tail - cB->head);
		}		
	}
	else{
		return 0;
	}
	

}

// returns the actual index of the head
uint16_t readHead (CBRef cB){
	// if the circular buffer is not null
	if (cB != NULL){
		return (cB->head);
	}
	else{
		return 0;
	}

}

// returns the actual index of the tail
uint16_t readTail (CBRef cB){
	// if the circular buffer is not null
	if (cB != NULL){
		return (cB->tail);
	}
	else{
		return 0;
	}

}

// returns the structure that the head points to. this
// does not mark the byte as read, so succesive calls to peak will
// always return the same value
tCanMessage peek(CBRef cB){
	tCanMessage retVal;
	// if the circular buffer is not null
	if (cB != NULL)
	{	
		// if there are bytes in the buffer
		if (getLength(cB) > 0){
			return cB->buffer[cB->head];
		}
	}
	return retVal;
}


// Manipulation Procedures
// ======================
// returns the front of the circular buffer and marks it read
tCanMessage readFront (CBRef cB){
	// if the circular buffer is not null
	tCanMessage retVal;
	if (cB != NULL)
	{	
		// if there are bytes in the buffer
		if (getLength(cB) > 0){
			retVal = cB->buffer[cB->head];
			cB->head = cB->head < (cB->size -1)? cB->head+1: 0;
			return retVal;
		}
	}
	retVal.validBytes = 0;
	return retVal;
}

// writes one tCanMessage at the end of the circular buffer, 
// increments overflow count if overflow occurs
void writeBack (CBRef cB, tCanMessage data){
	// if the circular buffer is not null
	if (cB != NULL){			
		if (getLength (cB) == (cB->size -1)){
			cB->overflowCount ++;
		} else {		
			cB->buffer[cB->tail] = data;
			cB->tail = cB->tail < (cB->size -1)? cB->tail+1: 0;
		}
	}
}

// empties the circular buffer. It does not change the capacity
// Use with caution!!
void makeEmpty(CBRef cB){
	if (cB != NULL){
		int i;
		for(i = 0; i < cB->size; ++i)
		{
			memset(&cB->buffer[i], 0, sizeof(tCanMessage));
		}
		cB->head = 0;
		cB->tail = 0;
		cB->overflowCount = 0;
	}
}

// returns the amount of times the CB has overflown;
uint8_t getOverflow(CBRef cB){
	if (cB != NULL){
		return cB->overflowCount;
	}
	return 0;
}

#if DEBUG
// Other Functions
// ===============
// prints the circular buffer, used for debug
void printCircBuf(CBRef cB){
	uint16_t i,j;
	// if the circular buffer is not null
	if (cB != NULL){
		printf("Buffer Size: \t%d\n", cB->size );
		printf("Head at: \t%d\n", cB->head );
		printf("Tail at: \t%d\n\n", cB->tail );
		for(i = 0; i < cB->size; ++i)
		{
			printf("===== ELEMENT %d =====\n", i);
			printf("Message ID:\t%li\n", cB->buffer[i].id.ulData);
			printf("Message Type:\t%d\n", cB->buffer[i].message_type);
			printf("Frame Type:\t%d\n", cB->buffer[i].frame_type);
			printf("Valid Bytes:\t%d\n", cB->buffer[i].validBytes);
			printf("Buffer:\t\t%d\n", cB->buffer[i].buffer);
			printf("Data Bytes: ");
			printf("[");
			for(j = 0; j < 8; j++){
				printf("%d ", cB->buffer[i].payload[j]);
			}
			printf("]\n");			
		}		
	}
	else{
		printf("Calling Print on an unintialized Circular Buffer");
	}
}
#endif
