/*
 * This file contains all the functions used by the ecan blocks. It does not
 * contain helper functions such as the circular buffer code.
 *
 */
 
// The SIM variable declares that this code is running in simulation mode on an x86 processor. It cuts or modifies code
// that won't work or is unneccesary for simulation. This is useful for validating the settings the code makes.
#define SIM

// Include various helpful header files
// Only include dsPIC33f header if not simulating
#ifdef SIM
#include "p33F.h"
#include <stdio.h>
#else
#include <p33Fxxxx.h>
#include "ecanDefinitions.h"
#endif
#include <stdint.h>

#ifdef SIM
typedef unsigned short int uint16_t; // Redefine a uint16_t to be 16-bits
#else
typedef unsigned int uint16_t;
#endif
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
                    bits 5-6 specify which DMA channel to use
  * parameters[1] = baud rate
  * parameters[2] = bits 0-2 are phase segment 1, bits 3-6 are propagation delay, bits 7-9 are phase segment 2
  * parameters[3] = filters 0 through 15 enable
  * parameters[4] = filters 0 through 7 mask select
  * parameters[5] = filters 8 through 15 mask select
  * parameters[6] = mask 0
  * parameters[7] = mask 1
  * parameters[8] = mask 2
  * parameters[9] = filter 0
  * parameters[10] = filter 1
  * parameters[11] = filter 2
  * parameters[12] = filter 3
  */
void ecan1_init(uint16_t* parameters);

/**
 * This function provides a general way to initialize the DMA peripheral.
 *
 * parameters[0] = IRQ address & squeezed version of DMAxCON minus CHEN bit
 * parameters[1] = address of peripheral (DMAxPAD)
 * parameters[2] = Number of memory units per DMA packet, starting at 1(DMAxCNT)
 * parameters[3] = Primary DPSRAM start address offset bits (DMAxSTA)
 */
void init_DMA0(uint16_t* parameters);

#ifdef SIM
int main(int argc, char* const argv[]);
#endif
