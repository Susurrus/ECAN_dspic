// This file contains the constant definitions as well as the required
// data structures to work with on the ECAN module
#ifndef _ECANDEFINITIONS_H_
#define _ECANDEFINITIONS_H_

#ifdef __cplusplus
       extern "C"{
#endif

// Message Types
#define CAN_MSG_DATA	0x01 // message type 
#define CAN_MSG_RTR		0x02 // data or RTR
#define CAN_FRAME_EXT	0x03 // Frame type
#define CAN_FRAME_STD	0x04 // extended or standard

// Specify some primitive types for convenience
typedef long int int32_t;
typedef unsigned long int uint32_t;
typedef int int16_t;
typedef unsigned int uint16_t;
typedef char int8_t;
typedef unsigned char uint8_t;

// Data structures
typedef struct {
	uint32_t id;
	uint8_t             buffer;
	uint8_t             message_type;
	uint8_t             frame_type;
	uint8_t             payload [8];
	uint8_t             validBytes;
} tCanMessage;

typedef union {
	tCanMessage 		message;
	uint8_t				bytes [sizeof(tCanMessage)];
} CanUnion;

#ifdef __cplusplus
       }
#endif

#endif /* _ECANDEFINITIONS_H_ */
