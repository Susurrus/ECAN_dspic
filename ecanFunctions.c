#include <ecanFunctions.h>

void ecan_init(uint16_t* parameters) {

  // Cache the desired states of the ECANs for quick use later.
  uint8 ecan1_state = (uint8_t)HIGH(parameters[0]);
  uint8 ecan2_state = (uint8_t)LOW(parameters[0]);

	// Make sure the ECAN module is in configuration mode.
  // It should be this way after a hardware reset, but
  // we make sure anyways.
  if (ecan1_state) {
    C1CTRL1bits.REQOP=4;
    while(C1CTRL1bits.OPMODE != 4);
  }
  if (ecan2_state) {
    C2CTRL1bits.REQOP=4;
    while(C2CTRL1bits.OPMODE != 4);
  }
 
  // Setup our frequencies for time quanta calculations.
  // FCAN is selected to be FCY: FCAN = FCY = 40MHz
  // We need to check FCY and FOSC to verify that FCAN doesn't exceed 40MHz.
	if (ecan1_state) {
    C1CTRL1bits.CANCKS = 0x1;
  }
	if (ecan2_state) {
    C2CTRL1bits.CANCKS = 0x1;
  }
 
  // Initialize our time quanta.
	if (ecan1_state) {
    uint16 ftq = 20 * HIGH(parameters[1]);
    C1CFG1bits.BRP = (40000000/(2 * ftq)) – 1; //TODO: Determine frequency of chip by using existing variable or checking registers somehow
    C1CFG1bits.SJW = 0x3; // Set jump width to 4TQ.
    C1CFG2bits.SEG2PHTS = 0x1; // Keep segment 2 time programmable
    C1CFG2bits.SEG2PH = ((parameters[3] & 0x0700) >> 3); // Set segment 2 time
    C1CFG2bits.PRSEG = ((parameters[3] & 0x003A) >> 3); // Set propagation segment time
    C1CFG2bits.SEG1PH = HIGH(parameters[2]) - 1 - C1CFG2bits.SEG2PH - C1CFG2bits.PRSEG; // Calculate phase segment 1 time
    C1CFG2bits.SAM = 0x1; // Triple-sample for majority rules at bit sample point TODO: Make this a user option
  }
  
  // Setup message filters and masks.
  if (ecan1_state) {
  	C1CTRL1bits.WIN = 1; // Allow configuration of masks and filters
    
    C1FEN1 = HIGH(parameters[5]) << 8 & HIGH(parameters[4]); // Enable desired filters
    
    C1FMSKSEL1 = HIGH(parameters[7]) << 8 & HIGH(parameters[6]); // Set filter mask selection bits for filters 0-7
    C1FMSKSEL2 = HIGH(parameters[9]) << 8 & HIGH(parameters[8]); // Set filter mask selection bits for filters 8-15
    
    C1RXF0SID = C1RXF0SID & HIGH(parameters[10]) << 5 & HIGH(parameters[11]) << 13; // Set the actual filter bits for filter 0
    C1RXF1SID = C1RXF1SID & (HIGH(parameters[11]) << 1 && 0xE0) & HIGH(parameters[12]) << 10; // Set the actual filter bits for filter 1
    C1RXF2SID = C1RXF2SID & (HIGH(parameters[12]) << 1 && 0xE0) & HIGH(parameters[13]) << 10; // Set the actual filter bits for filter 2
    C1RXF3SID = C1RXF3SID & (HIGH(parameters[13]) << 1 && 0xE0) & HIGH(parameters[14]) << 10; // Set the actual filter bits for filter 3
    
  	C1CTRL1bits.WIN = 0; // Restore the WIN bit to re-enable interrupts and access to other registers
  }
  
  // Return the modules to normal operating mode.
  if (ecan1_state) {
    C1CTRL1bits.REQOP=0;
    while(C1CTRL1bits.OPMODE != 0);
  }
  if (ecan2_state) {
    C2CTRL1bits.REQOP=0;
    while(C2CTRL1bits.OPMODE != 0);
  }
  
  // Setup our interrupts as our last step and clear all interrupt bits
  if (ecan1_state) {
    C1RXFUL1=0;
    C1INTFbits.RBIF=0;
  }
}

int main(int argc, char* const argv[]) {
  
  return 0;
}