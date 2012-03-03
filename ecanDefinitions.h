// This file contains the constant definitions as well as the required
// data structures to work with on the ECAN module
#ifndef _ECANDEFINITIONS_H_
#define _ECANDEFINITIONS_H_

#ifdef __cplusplus
       extern "C"{
#endif

#include "inttypes.h"

// Message Types either a data message or a remote transmit request
enum {
	CAN_MSG_DATA,
	CAN_MSG_RTR
};

// CAN frame type: either extended or standard
enum {
	CAN_FRAME_EXT,
	CAN_FRAME_STD
};

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
