#include "ecanFunctions.h"

// Declare space for our message buffer in DMA
unsigned int ecan1msgBuf[4][8] __attribute__((space(dma)));

// Initialize our circular buffer for receiving CAN messages
struct CircBuffer testBuffer;

CBRef ecanBuffer; // A pointer to our circular buffer

void ecan1_init(uint16_t* parameters) {

  // Make sure the ECAN module is in configuration mode.
  // It should be this way after a hardware reset, but
  // we make sure anyways.
  C1CTRL1bits.REQOP=4;
#ifndef SIM
  while(C1CTRL1bits.OPMODE != 4);
#endif

  // Initialize our circular buffers.
	ecanBuffer = (struct CircBuffer* )&testBuffer;
	newCircBuffer(ecanBuffer);
 
  // Initialize our time quanta (assume 20 total)
  unsigned long int ftq = 400000/(parameters[1]);
  ftq = ftq / (2 * 20);
  C1CFG1bits.BRP = ftq - 1; //TODO: Determine frequency of chip by using existing variable or checking registers somehow (frequency should be used here as itself divided by 100 because of how desired bps is passed in)
  C1CFG1bits.SJW = 0x3; // Set jump width to 4TQ.
  uint16_t a = parameters[2] & 0x0007;
  uint16_t b = (parameters[2] & 0x0038) >> 3;
  uint16_t c = (parameters[2] & 0x01C0) >> 6;
  C1CFG2bits.SEG1PH = a;//(parameters[2] & 0x0007); // Set segment 1 time
  C1CFG2bits.PRSEG = b;//(parameters[2] & 0x0038) >> 3; // Set propagation segment time
  C1CFG2bits.SEG2PHTS = 0x1; // Keep segment 2 time programmable
  C1CFG2bits.SEG2PH = c;//(parameters[2] & 0x01C0) >> 6; // Set phase segment 2 time
  C1CFG2bits.SAM = 0x1; // Triple-sample for majority rules at bit sample point TODO: Make this a user option
  
  // Setup our frequencies for time quanta calculations.
  // FCAN is selected to be FCY: FCAN = FCY = 40MHz
  // We need to check FCY and FOSC to verify that FCAN doesn't exceed 40MHz.
  C1CTRL1bits.CANCKS = 1;

  C1FCTRLbits.DMABS = 0; // Use 4 buffers in DMA RAM
  
  // Setup message filters and masks.
  C1CTRL1bits.WIN = 1; // Allow configuration of masks and filters
  
  // Set our filter mask parameters
  C1RXM0SIDbits.SID = parameters[6] >> 5; // Set filter 0
  C1RXM0SIDbits.MIDE = (parameters[6] & 0x0008) >> 3;
  C1RXM0EID = parameters[7];
  C1RXM1SIDbits.SID = parameters[8] >> 5; // Set filter 1
  C1RXM1SIDbits.MIDE = (parameters[8] & 0x0008) >> 3;
  C1RXM1EID = parameters[9];
  C1RXM2SIDbits.SID = parameters[10] >> 5; // Set filter 2
  C1RXM2SIDbits.MIDE = (parameters[10] & 0x0008) >> 3;
  C1RXM2EID = parameters[11];

  C1FEN1 = parameters[3]; // Enable desired filters

  C1FMSKSEL1 = parameters[4]; // Set filter mask selection bits for filters 0-7
  C1FMSKSEL2 = parameters[5]; // Set filter mask selection bits for filters 8-15

  C1BUFPNT1 = parameters[16]; // Buffer pointer for filters 0-3
  C1BUFPNT2 = parameters[17]; // Buffer pointer for filters 4-7
  C1BUFPNT3 = parameters[18]; // Buffer pointer for filters 8-11
  C1BUFPNT4 = parameters[19]; // Buffer pointer for filters 12-15x
  
  // Set our filter parameters
  C1RXF0SID = parameters[20];
  C1RXF0EID = parameters[21];
  C1RXF1SID = parameters[22];
  C1RXF1EID = parameters[23];
  C1RXF2SID = parameters[24];
  C1RXF2EID = parameters[25];
  C1RXF3SID = parameters[26];
  C1RXF3EID = parameters[27];
   
  C1CTRL1bits.WIN=0;
  
  // Return the modules to specified operating mode.
  // 0 normal, 1 disable, 2 loopback, 3 listen-only, 4 configuration, 7 listen all messages
  uint8_t desired_mode = (parameters[0] & 0x001C) >> 2;
  C1CTRL1bits.REQOP = desired_mode;
#ifndef SIM  
  while(C1CTRL1bits.OPMODE != desired_mode);
#endif
  
  // Clear all interrupt bits
  C1RXFUL1=C1RXFUL2=C1RXOVF1=C1RXOVF2=0x0000;
  
  // Enable interrupts for ECAN1
	IEC2bits.C1IE = 1; // Enable interrupts for ECAN1 peripheral
	C1INTEbits.TBIE = 1; // Enable TX buffer interrupt
	C1INTEbits.RBIE = 1; // Enable RX buffer interrupt
  
  // Configure buffer settings.
  // Must be done after mode setting for some reason
  // (can't find documentation on it)
  C1TR01CON = parameters[12];
  C1TR23CON = parameters[13];
  C1TR45CON = parameters[14];
  C1TR67CON = parameters[15];
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
  parameters[13] = 0x03FF;
  
  ecan1_init(parameters);
  
  return 0;
}
#endif

void init_DMA0(uint16_t* parameters) {
	DMACS0 = 0; // Clear the status register
	DMA0CONbits.DIR = (parameters[0] & 0x40) >> 6; // Set DMA direction
	DMA0CONbits.AMODE = (parameters[0] & 0xC) >> 2; // Set addressing mode

	DMA0PAD = parameters[1]; // Set the peripheral address that will be using DMA
 	DMA0CNT = parameters[2]; // Set data units to words or bytes
	DMA0REQbits.IRQSEL = (parameters[0] & 0x7F00) >> 8;	// Set the IRQ priority for the DMA transfer
	DMA0STA =  parameters[3]; // Set start address bits
  
	DMA0CONbits.CHEN = 1; // Enable DMA
}

void init_DMA2(uint16_t* parameters) {
	DMACS0 = 0; // Clear the status register
	DMA2CONbits.DIR = (parameters[0] & 0x40) >> 6; // Set DMA direction
	DMA2CONbits.AMODE = (parameters[0] & 0xC) >> 2; // Set addressing mode

	DMA2PAD = parameters[1]; // Set the peripheral address that will be using DMA
 	DMA2CNT = parameters[2]; // Set data units to words or bytes
	DMA2REQbits.IRQSEL = (parameters[0] & 0x7F00) >> 8;	// Set the IRQ priority for the DMA transfer
	DMA2STA =  parameters[3]; // Set start address bits
  
	DMA2CONbits.CHEN = 1; // Enable DMA
}

void rxECAN1(tCanMessage* message)
{
	unsigned int ide=0;
	unsigned int srr=0;
	unsigned long id=0,d;
			
	// read word 0 to see the message type 
	ide=ecan1msgBuf[message->buffer][0] & 0x0001;	
	srr=ecan1msgBuf[message->buffer][0] & 0x0002;	
	
	/* check to see what type of message it is */
	/* message is standard identifier */
	if(0==ide)
	{
		message->id = (tUnsignedLongToChar)(unsigned long)((ecan1msgBuf[message->buffer][0] & 0x1FFC) >> 2);		
		message->frame_type = CAN_FRAME_STD;
	}
	/* mesage is extended identifier */
	else
	{
		// If extended compute the extended ID
		id = ecan1msgBuf[message->buffer][0] & 0x1FFC;		
		message->id.ulData = (id << 16);
		id = ecan1msgBuf[message->buffer][1] & 0x0FFF;
		message->id.ulData = (message->id.ulData + (id << 6));
		id = (ecan1msgBuf[message->buffer][2] & 0xFC00) >> 10;
		message->id.ulData = (message->id.ulData + id);
		
		// Se the frame type to extended
		message->frame_type = CAN_FRAME_EXT;
	}
	/* check to see what type of message it is */
	/* RTR message */
	if(1==srr)
	{
		message->message_type=CAN_MSG_RTR;	
	}
	/* normal message */
	else
	{
		// set the data type
		message->message_type=CAN_MSG_DATA;
		
		message->payload[0]=(unsigned char)ecan1msgBuf[message->buffer][3];
		message->payload[1]=(unsigned char)((ecan1msgBuf[message->buffer][3] & 0xFF00) >> 8);
		message->payload[2]=(unsigned char)ecan1msgBuf[message->buffer][4];
		message->payload[3]=(unsigned char)((ecan1msgBuf[message->buffer][4] & 0xFF00) >> 8);
		message->payload[4]=(unsigned char)ecan1msgBuf[message->buffer][5];
		message->payload[5]=(unsigned char)((ecan1msgBuf[message->buffer][5] & 0xFF00) >> 8);
		message->payload[6]=(unsigned char)ecan1msgBuf[message->buffer][6];
		message->payload[7]=(unsigned char)((ecan1msgBuf[message->buffer][6] & 0xFF00) >> 8);
		message->validBytes=(unsigned char)(ecan1msgBuf[message->buffer][2] & 0x000F);
	}	
}

void txECAN1(unsigned char buf, long txIdentifier, unsigned int ide, unsigned int remoteTransmit, unsigned char dataLength, unsigned char* data){

  unsigned long word0=0, word1=0, word2=0;
  unsigned long sid10_0=0, eid5_0=0, eid17_6=0,a;


  if(ide) {
		eid5_0  = (txIdentifier & 0x3F);
		eid17_6 = (txIdentifier>>6) & 0xFFF;
		sid10_0 = (txIdentifier>>18) & 0x7FF;
		word1 = eid17_6;
	}	else {
		sid10_0 = (txIdentifier & 0x7FF);
	}
	
	
	if(remoteTransmit==1) { 	// Transmit Remote Frame
		word0 = ((sid10_0 << 2) | ide | 0x2);
		word2 = ((eid5_0 << 10)| 0x0200);
  }	else {
	  word0 = ((sid10_0 << 2) | ide);
	  word2 = (eid5_0 << 10);
	}

  if(ide) {
	  ecan1msgBuf[buf][0] = (word0 | 0x0002);
  } else {
	  ecan1msgBuf[buf][0] = word0;
  }
  
	ecan1msgBuf[buf][1] = word1;
	ecan1msgBuf[buf][2] = ((word2 & 0xFFF0) + dataLength) ;
	ecan1msgBuf[buf][3] = ((unsigned short*)data)[0];
	ecan1msgBuf[buf][4] = ((unsigned short*)data)[1];
	ecan1msgBuf[buf][5] = ((unsigned short*)data)[2];
	ecan1msgBuf[buf][6] = ((unsigned short*)data)[3];

  // TODO: Allow this to automatically set bit for correct buffer
	C1TR01CONbits.TXREQ0=1;	
}


void __attribute__((interrupt, no_auto_psv))_C1Interrupt(void) {    
  
  tCanMessage canMsg; // Give us a CAN message struct to populate and use
	
  // If the interrupt was set because of a transmit
	if(C1INTFbits.TBIF){ 
    	C1INTFbits.TBIF = 0;
  } 
 
	// if the interrupt was fired because of a received message
  if (C1INTFbits.RBIF) {      
		// read the message 
	  if (C1RXFUL1bits.RXFUL1==1) {
	    canMsg.buffer=1; // Set which buffer the message is in
	    C1RXFUL1bits.RXFUL1=0;
	  }
		
    //  Move the message from the DMA buffer to a data structure and then push it into our circular buffer.
	  rxECAN1(&canMsg);
    writeBack(ecanBuffer, canMsg);
    
    // Be sure to clear the interrupt flag.
		C1INTFbits.RBIF = 0;
	}
	
	IFS2bits.C1IF = 0;
}
