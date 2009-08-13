// This file contains the constant definitions as well as the required
// data structures to work with on the ECAN module


// Message Types
#define CAN_MSG_DATA	0x01 // message type 
#define CAN_MSG_RTR		0x02 // data or RTR
#define CAN_FRAME_EXT	0x03 // Frame type
#define CAN_FRAME_STD	0x04 // extended or standard


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
 	unsigned long   	ulData;
} tUnsignedLongToChar; 

typedef union{
	unsigned char   chData[4];
 	long   					loData;
} tLongToChar; 

typedef union{
	unsigned char   chData[4];
 	float   		flData;
	unsigned short	shData[2];
} tFloatToChar; 


// Data structures

typedef struct tCanMessage{
	tUnsignedLongToChar 		id;
	unsigned char						message_type;
	unsigned char						frame_type;
	unsigned char						payload [8];
	unsigned char 					validBytes;
	unsigned char						buffer;
}tCanMessage;
