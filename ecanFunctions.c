#include "ecanFunctions.h"

void ecan1_init(uint16_t* parameters) {

  // Make sure the ECAN module is in configuration mode.
  // It should be this way after a hardware reset, but
  // we make sure anyways.
  C1CTRL1bits.REQOP=4;
#ifndef SIM
  while(C1CTRL1bits.OPMODE != 4);
#endif
 
  // Setup our frequencies for time quanta calculations.
  // FCAN is selected to be FCY: FCAN = FCY = 40MHz
  // We need to check FCY and FOSC to verify that FCAN doesn't exceed 40MHz.
  C1CTRL1bits.CANCKS = 1;
 
  // Initialize our time quanta.
  uint16_t ftq = 20 * parameters[1];
  C1CFG1bits.BRP = (40000000/(2 * ftq)) - 1; //TODO: Determine frequency of chip by using existing variable or checking registers somehow
  C1CFG1bits.SJW = 0x3; // Set jump width to 4TQ.
  C1CFG2bits.SEG1PH = (parameters[3] & 0x0007); // Set segment 1 time
  C1CFG2bits.PRSEG = ((parameters[3] & 0x003A) >> 3); // Set propagation segment time
  C1CFG2bits.SEG2PHTS = 0x1; // Keep segment 2 time programmable
  C1CFG2bits.SEG2PH = (parameters[3] & 0x01C0) >> 6; // Set phase segment 2 time
  C1CFG2bits.SAM = 0x1; // Triple-sample for majority rules at bit sample point TODO: Make this a user option
  
  // Setup message filters and masks.
  C1CTRL1bits.WIN = 1; // Allow configuration of masks and filters
    
  C1FEN1 = parameters[3]; // Enable desired filters
    
  C1FMSKSEL1 = parameters[4]; // Set filter mask selection bits for filters 0-7
  C1FMSKSEL2 = parameters[5]; // Set filter mask selection bits for filters 8-15
  
  C1RXM0SIDbits.SID = parameters[6]; // Set filter 0
  C1RXM1SIDbits.SID = parameters[7]; // Set filter 1
  C1RXM2SIDbits.SID = parameters[8]; // Set filter 2
      
  C1RXF0SIDbits.SID = parameters[9]; // Set the actual filter bits for filter 0
  C1RXF1SIDbits.SID = parameters[10]; // Set the actual filter bits for filter 0
  C1RXF2SIDbits.SID = parameters[11]; // Set the actual filter bits for filter 0
  C1RXF3SIDbits.SID = parameters[12]; // Set the actual filter bits for filter 0
    
  C1CTRL1bits.WIN = 0; // Restore the WIN bit to re-enable interrupts and access to other registers
  
  // Return the modules to specified operating mode.
  // 0 normal, 1 disable, 2 loopback, 3 listen-only, 4 configuration, 7 listen all messages
  uint8_t desired_mode = (parameters[0] & 0x000C) >> 2;
  C1CTRL1bits.REQOP = desired_mode;
#ifndef SIM  
  while(C1CTRL1bits.OPMODE != desired_mode);
#endif
  
  // Setup our interrupts as our last step and clear all interrupt bits
  C1RXFUL1=0;
  C1INTFbits.RBIF=0;
}

#ifdef SIM
/**
 * This is a main used for testing the validity of all of the functions contained within this file. It is meant to be executed on 32-bit machines.
 * It therefore redefines a uint16_t as a short int.
 */
int main(int argc, char* const argv[]) {
  
  printf("Testing code...\n");
  
  // Test 1: Check on enabling a single ECAN only.
  uint16_t parameters[34];
  
  parameters[0] = 1;
  parameters[1] = 10000;
  parameters[3] = 0x01FF;
  parameters[4] = 0x0001;
  parameters[6] = 0;
  parameters[7] = 1;
  parameters[8] = 2;
  parameters[9] = 0x03FF;
  
  ecan1_init(parameters);
  
  return 0;
}
#endif
