/**********************************************************************
* © 2005 Microchip Technology Inc.
*
* FileName:        main.c
* Dependencies:    Header (.h) files if applicable, see below
* Processor:       dsPIC33Fxxxx
* Compiler:        MPLAB® C30 v3.00 or higher
*
* SOFTWARE LICENSE AGREEMENT:
* Microchip Technology Incorporated ("Microchip") retains all ownership and 
* intellectual property rights in the code accompanying this message and in all 
* derivatives hereto.  You may use this code, and any derivatives created by 
* any person or entity by or on your behalf, exclusively with Microchip's
* proprietary products.  Your acceptance and/or use of this code constitutes 
* agreement to the terms and conditions of this notice.
*
* CODE ACCOMPANYING THIS MESSAGE IS SUPPLIED BY MICROCHIP "AS IS".  NO 
* WARRANTIES, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING, BUT NOT LIMITED 
* TO, IMPLIED WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY AND FITNESS FOR A 
* PARTICULAR PURPOSE APPLY TO THIS CODE, ITS INTERACTION WITH MICROCHIP'S 
* PRODUCTS, COMBINATION WITH ANY OTHER PRODUCTS, OR USE IN ANY APPLICATION. 
*
* YOU ACKNOWLEDGE AND AGREE THAT, IN NO EVENT, SHALL MICROCHIP BE LIABLE, WHETHER 
* IN CONTRACT, WARRANTY, TORT (INCLUDING NEGLIGENCE OR BREACH OF STATUTORY DUTY), 
* STRICT LIABILITY, INDEMNITY, CONTRIBUTION, OR OTHERWISE, FOR ANY INDIRECT, SPECIAL, 
* PUNITIVE, EXEMPLARY, INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, FOR COST OR EXPENSE OF 
* ANY KIND WHATSOEVER RELATED TO THE CODE, HOWSOEVER CAUSED, EVEN IF MICROCHIP HAS BEEN 
* ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE FORESEEABLE.  TO THE FULLEST EXTENT 
* ALLOWABLE BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL CLAIMS IN ANY WAY RELATED TO 
* THIS CODE, SHALL NOT EXCEED THE PRICE YOU PAID DIRECTLY TO MICROCHIP SPECIFICALLY TO 
* HAVE THIS CODE DEVELOPED.
*
* You agree that you are solely responsible for testing the code and 
* determining its suitability.  Microchip has no obligation to modify, test, 
* certify, or support the code.
*
* REVISION HISTORY:
*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
* Author          	Date      Comments on this revision
*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
* Vinaya Skanda 	10/18/06  First release of source file
* Vinaya Skanda		07/25/07  Updates from Joe Supinsky and Jatinder Gharoo incorporated
*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
*
* ADDITIONAL NOTES:
* This code is tested on Explorer-16 board with ECAN PICTail Card.
* The device used is dsPIC33FJ256GP710 controller 
*
* The Processor starts with the External Crystal without PLL enabled and then the Clock is switched to PLL Mode.
*************************************************************************************************/

#if defined(__dsPIC33F__)
#include "p33fxxxx.h"
#elif defined(__PIC24H__)
#include "p24hxxxx.h"
#endif

#include <ECAN2Config.h>
#include <common.h>
#include "ecanFunctions.h"

//  Macros for Configuration Fuse Registers 
_FOSCSEL(FNOSC_FRC); 
_FOSC(FCKSM_CSECMD & OSCIOFNC_OFF  & POSCMD_XT);  
								// Clock Switching is enabled and Fail Safe Clock Monitor is disabled
								// OSC2 Pin Function: OSC2 is Clock Output
								// Primary Oscillator Mode: XT Crystanl


_FWDT(FWDTEN_OFF);              // Watchdog Timer Enabled/disabled by user software
								// (LPRC can be disabled by clearing SWDTEN bit in RCON register


// Define ECAN Message Buffers
ECAN2MSGBUF ecan2msgBuf __attribute__((space(dma),aligned(ECAN2_MSG_BUF_LENGTH*16)));

// CAN Messages in RAM
mID rx_ecan2message;

// Prototype Declaration
void oscConfig(void);
void clearIntrflags(void);
void ecan1WriteMessage(void);
void ecan2WriteMessage(void);
void setSpeed(short speed);
void enableDrive();
void disableDrive();

int main(void)
{

/* Configure Oscillator Clock Source 	*/
	oscConfig();

/* Clear Interrupt Flags 				*/
	clearIntrflags();

/* ECAN1 Initialisation 		
   Configure DMA Channel 0 for ECAN1 Transmit
   Configure DMA Channel 2 for ECAN1 Receive */
  
  //Initialize ECAN1
  uint16_t parameters[28];
  parameters[0] = 0x0040; // Normal mode, standard frames, DMA0 for transmission, DMA2 for reception
  parameters[1] = 2500; // Bit rate to 250kbps (parameter is in hundreds of bps)
  parameters[2] = 40000; // Oscillator frequency 40000000 (parameter is in khz)
  parameters[3] = 0x0F67; // phase segment 1: 8, phase segment 2: 6, propagation: 5, sjw: 4, triple-sample:on
  parameters[4] = 0x0007; // Enable filters 0,1,2
  parameters[5] = 0x0000; // Select filter mask 1 for filters 0,1,2
  parameters[6] = 0x0000;
  parameters[7] = 0;
  parameters[8] = 0; // Set filter mask 1 to ignore all bits
  parameters[9] = 0;
  parameters[10] = 0;
  parameters[11] = 0;
  parameters[12] = 0;
  parameters[13] = 0x0383; // Buffer 0/1 highest priority and 0 set to transmit, 1 set to receive
  parameters[14] = 0;
  parameters[15] = 0;
  parameters[16] = 0;
  parameters[17] = 0x0111; // Filters 0 through 2 direct messages to buffer 1
  parameters[18] = 0;
  parameters[19] = 0;
  parameters[20] = 0;
  parameters[21] = 0x8000; // Catch 0x400 messages
  parameters[22] = 0;
  parameters[23] = 0x8020; // Catch 0x401 messages
  parameters[24] = 0;
  parameters[25] = 0x8040; // Catch 0x402 messages
  parameters[26] = 0;
  ecan1_init(parameters);

/* ECAN2 Initialisation 		
   Configure DMA Channel 1 for ECAN2 Transmit
   Configure DMA Channel 3 for ECAN2 Receive */
	ecan2Init();
	dma1init();	
	dma3init();

/* Enable ECAN2 Interrupt */ 
	
	IEC3bits.C2IE = 1;
	C2INTEbits.TBIE = 1;	
	C2INTEbits.RBIE = 1;

	ecan2WriteTxMsgBufId(0,0x402,0,0);
	ecan2WriteTxMsgBufData(0,8,0xabcd,0xef12,0x3456,0x789a);
	C2TR01CONbits.TXREQ0=1;
 
/* Write a Message in ECAN1 Transmit Buffer	
   Request Message Transmission			*/
	unsigned short payload[4];
	
	payload[0] = 0x1234;
	payload[1] = 0x5678;
	payload[2] = 0x1234;
	payload[3] = 0x5678;
	
	txECAN1(0,0x300,0,0,8,(unsigned char*)payload);

	// Make sure to wait for the first message to send before sending a second
	while(C2TR01CONbits.TXREQ0 == 1);

/* Write a Message in ECAN2 Transmit Buffer
   Request Message Transmission			*/
	ecan2WriteTxMsgBufId(0,0x402,0,0);
	ecan2WriteTxMsgBufData(0,6,0x3344,0x7788,0x9911,0);
	C2TR01CONbits.TXREQ0=1;
	
	// Check that every message was successfully sent. If the code runs
	// past these checks, then the code probably executed correctly.
	while(C1TR01CONbits.TXREQ0 == 1);
	while(C2TR01CONbits.TXREQ0 == 1);
	
	while(1);
}

/******************************************************************************
*                                                                             
*    Function:			rxECAN2
*    Description:       moves the message from the DMA memory to RAM
*                                                                             
*    Arguments:			*message: a pointer to the message structure in RAM 
*						that will store the message. 
*	 Author:            Jatinder Gharoo                                                      
*	                                                                 
*                                                                              
******************************************************************************/
void rxECAN2(mID *message)
{
	unsigned int ide=0;
	unsigned int srr=0;
	unsigned long id=0,a1;
			
	/*
	Standard Message Format: 
	Word0 : 0bUUUx xxxx xxxx xxxx
			     |____________|||
 					SID10:0   SRR IDE(bit 0)     
	Word1 : 0bUUUU xxxx xxxx xxxx
			   	   |____________|
						EID17:6
	Word2 : 0bxxxx xxx0 UUU0 xxxx
			  |_____||	     |__|
			  EID5:0 RTR   	  DLC
	word3-word6: data bytes
	word7: filter hit code bits
	
	Substitute Remote Request Bit
	SRR->	"0"	 Normal Message 
			"1"  Message will request remote transmission
	
	Extended  Identifier Bit			
	IDE-> 	"0"  Message will transmit standard identifier
	   		"1"  Message will transmit extended identifier
	
	Remote Transmission Request Bit
	RTR-> 	"0"  Message transmitted is a normal message
			"1"  Message transmitted is a remote message
	*/
	/* read word 0 to see the message type */
	ide=ecan2msgBuf[message->buffer][0] & 0x0001;	
	srr=ecan2msgBuf[message->buffer][0] & 0x0002;	
	
	/* check to see what type of message it is */
	/* message is standard identifier */
	if(ide==0)
	{
		message->id=(ecan2msgBuf[message->buffer][0] & 0x1FFC) >> 2;		
		message->frame_type=CAN_FRAME_STD;
	}
	/* mesage is extended identifier */
	else
	{
		id=ecan2msgBuf[message->buffer][0] & 0x1FFC;		
		message->id=id << 16;
		id=ecan2msgBuf[message->buffer][1] & 0x0FFF;
		message->id=message->id+(id << 6);
		id=(ecan2msgBuf[message->buffer][2] & 0xFC00) >> 10;
		message->id=message->id+id;		
		message->frame_type=CAN_FRAME_EXT;
	}
	/* check to see what type of message it is */
	/* RTR message */
	if(srr==1)
	{
		message->message_type=CAN_MSG_RTR;	
	}
	/* normal message */
	else
	{
		message->message_type=CAN_MSG_DATA;
		message->data[0]=(unsigned char)ecan2msgBuf[message->buffer][3];
		message->data[1]=(unsigned char)((ecan2msgBuf[message->buffer][3] & 0xFF00) >> 8);
		message->data[2]=(unsigned char)ecan2msgBuf[message->buffer][4];
		message->data[3]=(unsigned char)((ecan2msgBuf[message->buffer][4] & 0xFF00) >> 8);
		message->data[4]=(unsigned char)ecan2msgBuf[message->buffer][5];
		message->data[5]=(unsigned char)((ecan2msgBuf[message->buffer][5] & 0xFF00) >> 8);
		message->data[6]=(unsigned char)ecan2msgBuf[message->buffer][6];
		message->data[7]=(unsigned char)((ecan2msgBuf[message->buffer][6] & 0xFF00) >> 8);
		message->data_length=(unsigned char)(ecan2msgBuf[message->buffer][2] & 0x000F);
	}	
}

void clearIntrflags(void){
/* Clear Interrupt Flags */

	IFS0=0;
	IFS1=0;
	IFS2=0;
	IFS3=0;
	IFS4=0;
}

void oscConfig(void){

/*  Configure Oscillator to operate the device at 40Mhz
 	Fosc= Fin*M/(N1*N2), Fcy=Fosc/2
 	Fosc= 8M*40/(2*2)=80Mhz for 8M input clock */

	PLLFBD=38;					/* M=40 */
	CLKDIVbits.PLLPOST=0;		/* N1=2 */
	CLKDIVbits.PLLPRE=0;		/* N2=2 */
	OSCTUN=0;					/* Tune FRC oscillator, if FRC is used */

/* Disable Watch Dog Timer */

	RCONbits.SWDTEN=0;

/* Clock switch to incorporate PLL*/
	__builtin_write_OSCCONH(0x03);		// Initiate Clock Switch to Primary
													// Oscillator with PLL (NOSC=0b011)
	__builtin_write_OSCCONL(0x01);		// Start clock switching
	while (OSCCONbits.COSC != 0b011);	// Wait for Clock switch to occur	


/* Wait for PLL to lock */

	while(OSCCONbits.LOCK!=1) {};
}

void __attribute__((interrupt, no_auto_psv))_C2Interrupt(void)  
{
	IFS3bits.C2IF = 0;        // clear interrupt flag
	if(C2INTFbits.TBIF)
    { 
		C2INTFbits.TBIF = 0;
    } 
    
    if(C2INTFbits.RBIF)
     {      
		// read the message 
	    if(C2RXFUL1bits.RXFUL1==1)
	    {
	    	rx_ecan2message.buffer=1;
	    	C2RXFUL1bits.RXFUL1=0;
	    }	    
	    rxECAN2(&rx_ecan2message); 	    	    
		C2INTFbits.RBIF = 0;
     }
}
 

//------------------------------------------------------------------------------
//    DMA interrupt handlers
//------------------------------------------------------------------------------

void __attribute__((interrupt, no_auto_psv)) _DMA0Interrupt(void)
{
   IFS0bits.DMA0IF = 0;          // Clear the DMA0 Interrupt Flag;
}

void __attribute__((interrupt, no_auto_psv)) _DMA1Interrupt(void)
{
   IFS0bits.DMA1IF = 0;          // Clear the DMA1 Interrupt Flag;
}

void __attribute__((interrupt, no_auto_psv)) _DMA2Interrupt(void)
{
   IFS1bits.DMA2IF = 0;          // Clear the DMA2 Interrupt Flag;
}

void __attribute__((interrupt, no_auto_psv)) _DMA3Interrupt(void)
{
   IFS2bits.DMA3IF = 0;          // Clear the DMA3 Interrupt Flag;
}
