/*
 * This file contains all the functions used by the ecan blocks. It does not
 * contain helper functions such as the circular buffer code.
 *
 */
 
// Include various helpful header files
//#include <p33Fxxxx.h> 
#include <p33FJ256GP710.h>
#include <stdint.h>

typedef unsigned int uint16_t;
typedef unsigned char uint8_t;

// Obtain just the high or low order bits of an uint16_t.
#define HIGH(x) ((x) >> 8)
#define LOW(x) ((x) & 0x00FF)


 /**
  * This function initializes the ECAN hardware. It takes a parameters array
  * of uint16s to specify all of the options. The first uint8 of each parameter option
  * is designated for ECAN1 and the second for ECAN2 fitting into a uint16-size area of data.
  *
  * This code is only being developed to work for filters 0 through 3 for simplicity's sake.
  *
  * The parameters array is used as follows:
  * parameters[0] = specifies standard or extended frames (1 for standard, 2 for extended, 0 means ECAN is not used)
  * parameters[1] = baud rate
  * parameters[2] = number of time quanta per bit
  * parameters[3] = bits 3-6 are propagation delay starting at 1, bits 0-2 are phase segment 2 starting at 1, phase segment 1 is deduced from parameters[2] and parameters[3]
  * parameters[4] = filters 0 through 7 enable
  * parameters[5] = filters 8 through 15 enable
  * parameters[6] = filters 0 through 3 masks
  * parameters[7] = filters 4 through 7 masks
  * parameters[8] = filters 8 through 11 masks
  * parameters[9] = filters 12 through 15 masks
  * parameters[10] = filter 0 bytes 0 through 7
  * parameters[11] = filter 0 bytes 8 through 10, filter 1 bytes 0 through 4
  * parameters[12] = filter 1 bytes 5 through 10
  * parameters[13] = filter 2 bytes 0 through 7
  * parameters[14] = filter 2 bytes 8 through 10, filter 3 bytes 0 through 4
  * parameters[15] = filter 3 bytes 5 through 10
  * parameters[16] = filter 4 bytes 0 through 7
  * parameters[17] = filter 4 bytes 8 through 10, filter 5 bytes 0 through 4
  * parameters[18] = filter 5 bytes 5 through 10
  * parameters[19] = filter 6 bytes 0 through 7
  * parameters[20] = filter 6 bytes 8 through 10, filter 7 bytes 0 through 4
  * parameters[21] = filter 7 bytes 5 through 10
  * parameters[22] = filter 8 bytes 0 through 7
  * parameters[23] = filter 8 bytes 8 through 10, filter 9 bytes 0 through 4
  * parameters[24] = filter 9 bytes 5 through 10
  * parameters[25] = filter 10 bytes 0 through 7
  * parameters[26] = filter 10 bytes 8 through 10, filter 11 bytes 0 through 4
  * parameters[27] = filter 11 bytes 5 through 10
  * parameters[28] = filter 12 bytes 0 through 7
  * parameters[29] = filter 12 bytes 8 through 10, filter 13 bytes 0 through 4
  * parameters[30] = filter 13 bytes 5 through 10
  * parameters[31] = filter 14 bytes 0 through 7
  * parameters[32] = filter 14 bytes 8 through 10, filter 15 bytes 0 through 4
  * parameters[33] = filter 15 bytes 5 through 10
  */
void ecan_init(uint16_t* parameters);