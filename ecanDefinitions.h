// This file contains the constant definitions as well as the required
// data structures to work with on the ECAN module

// This defines the size of the circular buffers
#define BSIZE  40


// Union definitions used to manipulate bytes for data
// sending and receiving but interpreting them as 
// signed/unsigned integers and/or floats

typedef union{
	unsigned char    chData[2];
	unsigned short   usData;
} tUnsignedShortToChar; 

typedef union{
	unsigned char    chData[2];
 	short   		 shData;
} tShortToChar; 

typedef union{
	unsigned char   	chData[4];
 	unsigned long   	uiData;
} tUnsignedIntToChar; 

typedef union{
	unsigned char   chData[4];
 	long   					inData;
} tIntToChar; 

typedef union{
	unsigned char   chData[4];
 	float   		flData;
	unsigned short	shData[2];
} tFloatToChar; 


// Data structures

typedef struct canMessage{
	tUnsingedIntToChar 			id;
	char										type;
	unsigned char						payload [8];		
}canMessage;