#include "ecanFunctions.h"

// Declare space for our message buffer in DMA
uint16_t ecan1msgBuf[4][8] __attribute__((space(dma)));

// Initialize our circular buffer for receiving CAN messages
struct CircBuffer testBuffer;

CBRef ecanBuffer; // A pointer to our circular buffer

void ecan1_init(uint16_t* parameters) {

  // Make sure the ECAN module is in configuration mode.
  // It should be this way after a hardware reset, but
  // we make sure anyways.
  C1CTRL1bits.REQOP=4;
  while(C1CTRL1bits.OPMODE != 4);

  // Initialize our circular buffers.
  ecanBuffer = (struct CircBuffer* )&testBuffer;
  newCircBuffer(ecanBuffer);
 
  // Initialize our time quanta
  uint16_t a = parameters[3] & 0x0007;
  uint16_t b = (parameters[3] & 0x0038) >> 3;
  uint16_t c = (parameters[3] & 0x01C0) >> 6;
  
  uint32_t ftq = parameters[2]/parameters[1]*10;
  ftq = ftq / (2 * (a+b+c+4)); // Divide by the 2*number of time quanta (4 is because of the 1-offset for a/b/c and the sync segment)
  C1CFG1bits.BRP = ftq - 1;
  C1CFG1bits.SJW = (parameters[3] & 0x0600) >> 9;
  C1CFG2bits.SEG1PH = a; // Set segment 1 time
  C1CFG2bits.PRSEG = b; // Set propagation segment time
  C1CFG2bits.SEG2PHTS = 0x1; // Keep segment 2 time programmable
  C1CFG2bits.SEG2PH = c; // Set phase segment 2 time
  C1CFG2bits.SAM = (parameters[3] & 0x0800) >> 11; // Triple-sample for majority rules at bit sample point
  
  // Setup our frequencies for time quanta calculations.
  // FCAN is selected to be FCY: FCAN = FCY = 40MHz. This is actually a don't care bit in dsPIC33f
  C1CTRL1bits.CANCKS = 1;

  C1FCTRLbits.DMABS = 0; // Use 4 buffers in DMA RAM
  
  // Setup message filters and masks.
  C1CTRL1bits.WIN = 1; // Allow configuration of masks and filters
  
  // Set our filter mask parameters
  C1RXM0SIDbits.SID = parameters[7] >> 5; // Set filter 0
  C1RXM0SIDbits.MIDE = (parameters[7] & 0x0008) >> 3;
  C1RXM0EID = parameters[8];
  C1RXM1SIDbits.SID = parameters[9] >> 5; // Set filter 1
  C1RXM1SIDbits.MIDE = (parameters[9] & 0x0008) >> 3;
  C1RXM1EID = parameters[10];
  C1RXM2SIDbits.SID = parameters[11] >> 5; // Set filter 2
  C1RXM2SIDbits.MIDE = (parameters[11] & 0x0008) >> 3;
  C1RXM2EID = parameters[12];

  C1FEN1 = parameters[4]; // Enable desired filters

  C1FMSKSEL1 = parameters[5]; // Set filter mask selection bits for filters 0-7
  C1FMSKSEL2 = parameters[6]; // Set filter mask selection bits for filters 8-15

  C1BUFPNT1 = parameters[17]; // Buffer pointer for filters 0-3
  C1BUFPNT2 = parameters[18]; // Buffer pointer for filters 4-7
  C1BUFPNT3 = parameters[19]; // Buffer pointer for filters 8-11
  C1BUFPNT4 = parameters[20]; // Buffer pointer for filters 12-15x
  
  // Set our filter parameters
  C1RXF0SID = parameters[21];
  C1RXF0EID = parameters[22];
  C1RXF1SID = parameters[23];
  C1RXF1EID = parameters[24];
  C1RXF2SID = parameters[25];
  C1RXF2EID = parameters[26];
  C1RXF3SID = parameters[27];
  C1RXF3EID = parameters[28];
  C1RXF4SID = parameters[29];
  C1RXF4EID = parameters[30];
  C1RXF5SID = parameters[31];
  C1RXF5EID = parameters[32];
  C1RXF6SID = parameters[33];
  C1RXF6EID = parameters[34];
  C1RXF7SID = parameters[35];
  C1RXF7EID = parameters[36];
  C1RXF8SID = parameters[37];
  C1RXF8EID = parameters[38];
  C1RXF9SID = parameters[39];
  C1RXF9EID = parameters[40];
  C1RXF10SID = parameters[41];
  C1RXF10EID = parameters[42];
  C1RXF11SID = parameters[43];
  C1RXF11EID = parameters[44];
  C1RXF12SID = parameters[45];
  C1RXF12EID = parameters[46];
  C1RXF13SID = parameters[47];
  C1RXF13EID = parameters[48];
  C1RXF14SID = parameters[49];
  C1RXF14EID = parameters[50];
  C1RXF15SID = parameters[51];
  C1RXF15EID = parameters[52];
   
  C1CTRL1bits.WIN=0;
  
  // Return the modules to specified operating mode.
  // 0 normal, 1 disable, 2 loopback, 3 listen-only, 4 configuration, 7 listen all messages
  uint8_t desired_mode = (parameters[0] & 0x001C) >> 2;
  C1CTRL1bits.REQOP = desired_mode;
  while(C1CTRL1bits.OPMODE != desired_mode);
  
  // Clear all interrupt bits
  C1RXFUL1=C1RXFUL2=C1RXOVF1=C1RXOVF2=0x0000;
  
  // Enable interrupts for ECAN1
  IEC2bits.C1IE = 1; // Enable interrupts for ECAN1 peripheral
  C1INTEbits.TBIE = 1; // Enable TX buffer interrupt
  C1INTEbits.RBIE = 1; // Enable RX buffer interrupt
  
  // Configure buffer settings.
  // Must be done after mode setting for some reason
  // (can't find documentation on it)
  C1TR01CON = parameters[13];
  C1TR23CON = parameters[14];
  C1TR45CON = parameters[15];
  C1TR67CON = parameters[16];
  
  // Setup necessary DMA channels for transmission and reception
  // Transmission DMA
  uint16_t dmaParameters[6];
  dmaParameters[0] = 0x4648;
  dmaParameters[1] = (uint16_t)&C1TXD;
  dmaParameters[2] = 7;
  dmaParameters[3] = __builtin_dmaoffset(ecan1msgBuf);
  dmaParameters[4] = ((parameters[0] >> 5) & 7);
  dmaParameters[5] = 0;
  dma_init(dmaParameters);
  
  // Reception DMA
  dmaParameters[0] = 0x2208;
  dmaParameters[1] = (uint16_t)&C1RXD;
  dmaParameters[4] = ((parameters[0] >> 8) & 7);
  dma_init(dmaParameters);
}

tCanMessage ecan1_receive() {
	return readFront(ecanBuffer);
}

void ecan1_receive_matlab(uint32_t* output) {
	tCanMessage msg;

	msg = readFront(ecanBuffer);

	output[0] = msg.id.ulData;
	output[1] = *((uint32_t*)msg.payload);
	output[2] = *((uint32_t*)&msg.payload[4]);
	output[3] = (((uint32_t)msg.validBytes) << 16);
	if (msg.message_type == CAN_MSG_RTR) {
		output[3] |= 1;
	}
}

void ecan1_transmit(uint8_t buffer, uint32_t txIdentifier, uint8_t ide, uint8_t remoteTransmit, uint8_t dataLength, uint8_t* data){

	uint32_t word0 = 0, word1 = 0, word2 = 0;
	uint32_t sid10_0 = 0, eid5_0 = 0, eid17_6 = 0;
	
	// Variables for setting correct TXREQ bit
	uint16_t bit_to_set;
	uint16_t offset;
	uint16_t* bufferCtrlRegAddr;
	
	if (ide) {
		eid5_0  = (txIdentifier & 0x3F);
		eid17_6 = (txIdentifier>>6) & 0xFFF;
		sid10_0 = (txIdentifier>>18) & 0x7FF;
		word1 = eid17_6;
	}
	else {
		sid10_0 = (txIdentifier & 0x7FF);
	}


	if (remoteTransmit == 1) { 	// Transmit Remote Frame
		word0 = ((sid10_0 << 2) | ide | 0x2);
		word2 = ((eid5_0 << 10)| 0x0200);
	}
	else {
		word0 = ((sid10_0 << 2) | ide);
		word2 = (eid5_0 << 10);
	}

	if (ide) {
		ecan1msgBuf[buffer][0] = (word0 | 0x0002);
	}
	else {
		ecan1msgBuf[buffer][0] = word0;
	}

	ecan1msgBuf[buffer][1] = word1;
	ecan1msgBuf[buffer][2] = ((word2 & 0xFFF0) + dataLength) ;
	ecan1msgBuf[buffer][3] = ((uint16_t)data[1] | ((uint16_t)data[0] << 8));
	ecan1msgBuf[buffer][4] = ((uint16_t)data[3] | ((uint16_t)data[2] << 8));
	ecan1msgBuf[buffer][5] = ((uint16_t)data[5] | ((uint16_t)data[4] << 8));
	ecan1msgBuf[buffer][6] = ((uint16_t)data[7] | ((uint16_t)data[6] << 8));

	// Set the correct transfer intialization bit (TXREQ) based on message buffer.
    offset = buffer >> 1;
	bufferCtrlRegAddr = (uint16_t *)(&C1TR01CON + offset);
	bit_to_set = 1 << (3 | ((buffer & 1) << 3));
	*bufferCtrlRegAddr |= bit_to_set;
	
	// Block while message is being sent
	while(((*bufferCtrlRegAddr) & bit_to_set) != 0);
}

void ecan1_transmit_matlab(uint16_t* parameters) {
	ecan1_transmit((uint8_t)parameters[0], 
	        ((uint32_t)parameters[1])|(((uint32_t)parameters[2])<<16),
			(uint8_t)parameters[3],
			(uint8_t)(parameters[3]>>8),
			(uint8_t)(parameters[0]>>8),
			(uint8_t*)&parameters[4]);
}

void dma_init(uint16_t* parameters) {

	// Determine the correct addresses for all needed registers
	uint16_t offset = (parameters[4]*6);
	uint16_t* chanCtrlRegAddr = (uint16_t *)(&DMA0CON + offset);
	uint16_t* irqSelRegAddr = (uint16_t *)(&DMA0REQ + offset);
	uint16_t* addrOffsetRegAddr = (uint16_t *)(&DMA0STA + offset);
	uint16_t* secAddrOffsetRegAddr = (uint16_t *)(&DMA0STB + offset);
	uint16_t* periAddrRegAddr = (uint16_t *)(&DMA0PAD + offset);
	uint16_t* transCountRegAddr = (uint16_t *)(&DMA0CNT + offset);

	DMACS0 = 0; // Clear the status register

	*periAddrRegAddr = (uint16_t)parameters[1]; // Set the peripheral address that will be using DMA
 	*transCountRegAddr = (uint16_t)parameters[2]; // Set data units to words or bytes
	*irqSelRegAddr = (uint16_t)(parameters[0] >> 8);	// Set the IRQ priority for the DMA transfer
	*addrOffsetRegAddr = (uint16_t)parameters[3]; // Set primary DPSRAM start address bits
	*secAddrOffsetRegAddr = (uint16_t)parameters[5]; // Set secondary DPSRAM start address bits
	
	// Setup the configuration register & enable DMA
	*chanCtrlRegAddr = (uint16_t)(0x8000 | ((parameters[0] & 0x00F0) << 7) | ((parameters[0] & 0x000C) << 2));  
}

/**
 * This is an interrupt handler for the ECAN1 peripheral.
 * It clears interrupt bits and pushes received message into
 * the circular buffer.
 */
void __attribute__((interrupt, no_auto_psv))_C1Interrupt(void) {    

	// Give us a CAN message struct to populate and use
	tCanMessage message;
	uint8_t ide = 0;
	uint8_t srr = 0;
	uint32_t id = 0;
	uint8_t buffer = 0;
	
	// If the interrupt was set because of a transmit,
	// just clear the corresponding bit.
	if (C1INTFbits.TBIF) { 
		C1INTFbits.TBIF = 0;
	}

	// If the interrupt was fired because of a received message
	// package it all up and store in the circular buffer.
	if (C1INTFbits.RBIF) {
		// read the message 
		if (C1RXFUL1bits.RXFUL1 == 1) {
			buffer = 1; // Set which buffer the message is in
			C1RXFUL1bits.RXFUL1 = 0;
		}
		
		//  Move the message from the DMA buffer to a data structure and then push it into our circular buffer.
				
		// read word 0 to see the message type 
		ide = ecan1msgBuf[buffer][0] & 0x0001;	
		srr = ecan1msgBuf[buffer][0] & 0x0002;	
		
		/* Format the message properly according to whether it
		 * uses an extended identifier or not.
		 */
		if (ide == 0) {		
			message.frame_type = CAN_FRAME_STD;
			
			message.id = (tUnsignedLongToChar)(uint32_t)((ecan1msgBuf[buffer][0] & 0x1FFC) >> 2);
		}
		else {
			message.frame_type = CAN_FRAME_EXT;
			
			id = ecan1msgBuf[buffer][0] & 0x1FFC;		
			message.id.ulData = (id << 16);
			id = ecan1msgBuf[buffer][1] & 0x0FFF;
			message.id.ulData = (message.id.ulData + (id << 6));
			id = (ecan1msgBuf[buffer][2] & 0xFC00) >> 10;
			message.id.ulData = (message.id.ulData + id);
		}
		
		/* If message is a remote transmit request, mark it as such.
		 * Otherwise it will be a regular transmission so fill its
		 * payload with the relevant data.
		 */
		if (srr == 1) {
			message.message_type = CAN_MSG_RTR;	
		}
		else {
			message.message_type = CAN_MSG_DATA;
			
			message.validBytes = (uint8_t)(ecan1msgBuf[buffer][2] & 0x000F);
			message.payload[0] = (uint8_t)ecan1msgBuf[buffer][3];
			message.payload[1] = (uint8_t)((ecan1msgBuf[buffer][3] & 0xFF00) >> 8);
			message.payload[2] = (uint8_t)ecan1msgBuf[buffer][4];
			message.payload[3] = (uint8_t)((ecan1msgBuf[buffer][4] & 0xFF00) >> 8);
			message.payload[4] = (uint8_t)ecan1msgBuf[buffer][5];
			message.payload[5] = (uint8_t)((ecan1msgBuf[buffer][5] & 0xFF00) >> 8);
			message.payload[6] = (uint8_t)ecan1msgBuf[buffer][6];
			message.payload[7] = (uint8_t)((ecan1msgBuf[buffer][6] & 0xFF00) >> 8);
		}
		
		// Send off the message
		writeBack(ecanBuffer, message);

		// Be sure to clear the interrupt flag.
		C1INTFbits.RBIF = 0;
	}
	
	IFS2bits.C1IF = 0;
}
