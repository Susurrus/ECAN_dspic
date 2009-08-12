#include "ecanFunctions.h"

unsigned int ecan1MsgBuf[4][8] __attribute__(space(dma));
tCanMessage canMsg;

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
  
  C1BUFPNT1 = parameters[13]; // Buffer pointer for filters 0-3
  C1BUFPNT2 = parameters[14]; // Buffer pointer for filters 4-7
  C1BUFPNT3 = parameters[15]; // Buffer pointer for filters 8-11
  C1BUFPNT4 = parameters[16]; // Buffer pointer for filters 12-15
      
  C1RXF0SIDbits.SID = parameters[17]; // Set the actual filter bits for filter 0
  C1RXF1SIDbits.SID = parameters[18]; // Set the actual filter bits for filter 0
  C1RXF2SIDbits.SID = parameters[19]; // Set the actual filter bits for filter 0
  C1RXF3SIDbits.SID = parameters[20]; // Set the actual filter bits for filter 0
    
  C1CTRL1bits.WIN = 0; // Restore the WIN bit to re-enable interrupts and access to other registers
  
  // Setup CiTRmnCON registers (for dealing with buffers)
  C1TR01CON = parameters[9];
  C1TR23CON = parameters[10];
  C1TR45CON = parameters[11];
  C1TR67CON = parameters[12];
  
  // Deal with DMA setup
  C1FCTRL.DMABS = 0; // Use 4 buffers in DMA RAM
  uint16_t dma_params[4];
  switch ((parameters[0] & 0x0060) >> 5) {
    case 0:
      init_DMA0();
      break;
    case 1:
      init_DMA1();
      break;
    case 2:
      init_DMA2();
      break;
    case 3:
      init_DMA3();
      break;
  }
  
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


	// Configfure the DMA here Need two DMAs. Choose 7 and 8
	
	// Need to configure which buffer for receive and which to send
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
		message->id=(ecan1msgBuf[message->buffer][0] & 0x1FFC) >> 2;		
		message->frame_type=CAN_FRAME_STD;
	}
	/* mesage is extended identifier */
	else
	{
		// If extended compute the extended ID
		id = ecan1msgBuf[message->buffer][0] & 0x1FFC;		
		message->id = id << 16;
		id = ecan1msgBuf[message->buffer][1] & 0x0FFF;
		message->id = message->id+(id << 6);
		id = (ecan1msgBuf[message->buffer][2] & 0xFC00) >> 10;
		message->id = message->id + id;
		
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


void __attribute__((interrupt, no_auto_psv))_C1Interrupt(void)  
{    
	// If the interrupt was set because of a transmit
	if(C1INTFbits.TBIF){ 
    	C1INTFbits.TBIF = 0;
  } 
 
	// if the interrupt was fired because of a received message
  if(C1INTFbits.RBIF){      
		// read the message 
	  if(C1RXFUL1bits.RXFUL1==1)
	    {
	    	rx_ecan1message.buffer=1;
	    	C1RXFUL1bits.RXFUL1=0;
	    }
		   //  Move the message from the DMA buffer to a data structure.
	    rxECAN1(&canMsg);
		C1INTFbits.RBIF = 0;
	}
	
	IFS2bits.C1IF = 0;
}

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