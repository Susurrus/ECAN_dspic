/*
 * This file contains all the functions used by the ecan blocks. It does not
 * contain helper functions such as the circular buffer code.
 *
 */
 
#ifndef _ECANFUNCTIONS_H_
#define _ECANFUNCTIONS_H_

// Include various helpful header files
// Only include dsPIC33f header if not simulating
#include <p33Fxxxx.h>
#include "ecanDefinitions.h"
#include "circBuffer.h"

 /**
  * This function initializes the first ECAN module. It takes a parameters array
  * of uint16s to specify all of the options.
  *
  * The parameters array is used as follows:
  * parameters[0] = bits 0-1: specify standard or extended frames (1 for standard, 2 for extended, 0 means ECAN is not used), 
                    bits 2-4: specify module mode (loopback, disabled, etc.),
                    bits 5-7: specify which DMA channel to use for receiving,
                    bits 8-10: specify which DMA channel to use for transmitting
  * parameters[1] = bit rate in units of hundreds of bits per second
  * parameters[2] = oscillator frequency in hundreds of Hertz
  * parameters[3] = bits 0-2: phase segment 1
                    bits 3-5: propagation delay
					bits 6-8: phase segment 2
					bits 9-10: sync jump width
					bit 11: specifies triple sample at sampling point
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
  * parameters[29] = filter 4 (standard, exide, ei17-16)
  * parameters[30] = filter 4 
  * parameters[31] = filter 5 (standard, exide, ei17-16)
  * parameters[32] = filter 5 (extended 15-0)(extended 15-0)
  * parameters[33] = filter 6 (standard, exide, ei17-16)
  * parameters[34] = filter 6 (extended 15-0)
  * parameters[35] = filter 7 (standard, exide, ei17-16)
  * parameters[36] = filter 7 (extended 15-0)
  * parameters[37] = filter 8 (standard, exide, ei17-16)
  * parameters[38] = filter 8 (extended 15-0)
  * parameters[39] = filter 9 (standard, exide, ei17-16)
  * parameters[40] = filter 9 (extended 15-0)
  * parameters[41] = filter 10 (standard, exide, ei17-16)
  * parameters[42] = filter 10 (extended 15-0)
  * parameters[43] = filter 11 (standard, exide, ei17-16)
  * parameters[44] = filter 11 (extended 15-0)
  * parameters[45] = filter 12 (standard, exide, ei17-16)
  * parameters[46] = filter 12 (extended 15-0)
  * parameters[47] = filter 13 (standard, exide, ei17-16)
  * parameters[48] = filter 13 (extended 15-0)
  * parameters[49] = filter 14 (standard, exide, ei17-16)
  * parameters[50] = filter 14 (extended 15-0)
  * parameters[51] = filter 15 (standard, exide, ei17-16)
  * parameters[52] = filter 15 (extended 15-0)
  */
void ecan1_init(uint16_t* parameters);

/**
 * Pops the top message from the ECAN1 reception buffer.
 * @return A tCanMessage struct with the older message data.
 */
tCanMessage ecan1_receive();

/**
 * Pop the top message from the ECAN1 reception buffer.
 * Parameters designed to interface with MATLAB C-function block.
 * @param output A pointer to a 4-element uint32 array.
 * output[0] = identifier (bits 0-28)
 * output[1] = CAN data, low-order bits
 * output[2] = CAN data, high-order bits
 * output[3] = bits 0-7: number of valid data bytes
 *             bits 8-15: remote transmit bit
 */
void ecan1_receive_matlab(uint32_t* output);

/**
 * This function transmits a CAN message on the ECAN1 CAN bus.
 * NOTE: This function blocks until message is sent.
 * @param buf Transmission buffer ID
 * @param txIdentifier CAN message identifier (either 11 or 29 bits)
 * @param ide Single bit specifying if the message uses an extended ID
 * @param remoteTransmit RTR bit specifying if this message requests a remote transmission
 * @param dataLength Number of bytes of data passed
 * @param data Data to transmit, between 0 and 8 bytes
 */
void ecan1_transmit(uint8_t buffer, uint32_t txIdentifier, uint8_t ide, uint8_t remoteTransmit, uint8_t dataLength, uint8_t* data);

/**
 * Transmits an ECAN message by calling txECAN1.
 * Parameters designed to interface with MATLAB C-function block.
 * @param parameters An array of uint16_ts with configuration options documented below.
 * parameters[0] = bits 0-7: ECAN buffer number
 *                 bits 8-15: data length (in bytes)
 * parameters[1] = CAN identifier
 * parameters[2] = CAN identifier (high-order bits)
 * parameters[3] = bits 0-7: ide bit
 *                 bits 8-15: remote transmit bit
 * parameters[4] = data bytes 0 and 1
 * parameters[5] = data bytes 2 and 3
 * parameters[6] = data bytes 4 and 5
 * parameters[7] = data bytes 6 and 7
 */
void ecan1_transmit_matlab(uint16_t* parameters);

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
void dma_init(uint16_t* parameters);

extern unsigned int ecan1msgBuf[4][8] __attribute__((space(dma)));

#endif /* _ECANFUNCTIONS_H_ */
