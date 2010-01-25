/*
 * This file contains all the functions used by the ecan blocks. It does not
 * contain helper functions such as the circular buffer code.
 *
 */
 
#ifndef _ECANFUNCTIONS_H_
#define _ECANFUNCTIONS_H_

 
// The SIM variable declares that this code is running in simulation mode on an x86 processor. It cuts or modifies code
// that won't work or is unneccesary for simulation. This is useful for validating the settings the code makes.
//#define SIM

// Include various helpful header files
// Only include dsPIC33f header if not simulating
#include <p33Fxxxx.h>
#include "ecanDefinitions.h"
#include "circBuffer.h"
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>

typedef unsigned int uint16_t;
typedef unsigned char uint8_t;

 /**
  * This function initializes the first ECAN module. It takes a parameters array
  * of uint16s to specify all of the options.
  *
  * This code is only being developed to work for filters 0 through 3 for simplicity's sake.
  *
  * The parameters array is used as follows:
  * parameters[0] = bits 0-1 specify standard or extended frames (1 for standard, 2 for extended, 0 means ECAN is not used), 
                    bits 2-4 specify module mode (loopback, disabled, etc.),
                    bits 5-7 specify which DMA channel to use for receiving,
                    bits 8-10 specify which DMA channel to use for transmitting
  * parameters[1] = bit rate in units of hundreds of bits per second
  * parameters[2] = oscillator frequency in hundreds of Hertz
  * parameters[3] = bits 0-2 are phase segment 1, bits 3-5 are propagation delay, bits 6-8 are phase segment 2, bits 9-10 are sync jump width, bits 11 specifies triple sample at sampling point
  * parameters[4] = filters 0 through 15 enable
  * parameters[5] = filters 0 through 7 mask select
  * parameters[6] = filters 8 through 15 mask select
  * parameters[7] = mask 0 (standard, exide, ei17-16)
  * parameters[8] = mask 0 (ei15-0)
  * parameters[9] = mask 1 (standard, exide, ei17-16)
  * parameters[10] = mask 1 (ei15-0)
  * parameters[11] = mask 2 (standard, exide, ei17-16)
  * parameters[12] = mask 2 (ei15-0)
  * parameters[13] = C1TR01CON
  * parameters[14] = C1TR23CON
  * parameters[15] = C1TR45CON
  * parameters[16] = C1TR67CON
  * parameters[17] = Buffer pointer for filters 0-3
  * parameters[18] = Buffer pointer for filters 4-7
  * parameters[19] = Buffer pointer for filters 8-11
  * parameters[20] = Buffer pointer for filters 12-15
  * parameters[21] = filter 0 (standard, exide, ei17-16)
  * parameters[22] = filter 0 (extended 15-0)
  * parameters[23] = filter 1 (standard, exide, ei17-16)
  * parameters[24] = filter 1 (extended 15-0)
  * parameters[25] = filter 2 (standard, exide, ei17-16)
  * parameters[26] = filter 2 (extended 15-0)
  * parameters[27] = filter 3 (standard, exide, ei17-16)
  * parameters[28] = filter 3 (extended 15-0)
  */
void ecan1_init(uint16_t* parameters);

/**
 * This function provides a general way to initialize the DMA peripheral.
 *
 * parameters[0] = IRQ address & squeezed version of DMAxCON minus CHEN bit
 * parameters[1] = address of peripheral (DMAxPAD)
 * parameters[2] = Number of memory units per DMA packet, starting at 1(DMAxCNT)
 * parameters[3] = Primary DPSRAM start address offset bits (DMAxSTA)
 * parameters[4] = Which DMA channel to configure
 * parameters[5] = Secondary DPSRAM start address offset bits (DMAxSTB)
 */
void init_DMA(uint16_t* parameters);

/**
 * This function copies a can message into the global
 * reception CAN circular buffer
 */
void rxECAN1(tCanMessage* message);

/**
 * This function transmits a CAN message on the ECAN1 CAN bus.
 */
void txECAN1(unsigned char buf, long txIdentifier, unsigned int ide, unsigned int remoteTransmit, unsigned char dataLength, unsigned char* data);

extern unsigned int ecan1msgBuf[4][8] __attribute__((space(dma)));

#endif /* _ECANFUNCTIONS_H_ */
