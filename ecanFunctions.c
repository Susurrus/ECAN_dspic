#include "ecanFunctions.h"
#include "CircularBuffer.h"

/**
 * @file   ecanFunctions.c
 * @author Bryant Mairs
 * @author Pavlo Manovi
 * @date   September 28th, 202
 * @brief  Provides C functions for ECAN blocks
 */

#define TRUE 1
#define FALSE 0

//Number of 8-byte CAN messages buffer supports
#define ARRAYSIZE 8 * 24


// Declare space for our message buffer in DMA
uint16_t ecan1msgBuf[4][8] __attribute__((space(dma)));

// Initialize our circular buffers and data arrays for transreceiving CAN messages
CircularBuffer ecan1_rx_buffer;
uint8_t rx_data_array[ARRAYSIZE];
CircularBuffer ecan1_tx_buffer;
uint8_t tx_data_array[ARRAYSIZE];

// Track whether or not we're currently transmitting
unsigned char currentlyTransmitting = 0;
unsigned char receivedMessagesPending = 0;

void ecan1_init(const uint16_t *parameters)
{
    // Make sure the ECAN module is in configuration mode.
    // It should be this way after a hardware reset, but
    // we make sure anyways.
    C1CTRL1bits.REQOP = 4;
    while (C1CTRL1bits.OPMODE != 4);

    // Initialize our circular buffers. If this fails, we crash and burn.
    if (!CB_Init(&ecan1_tx_buffer, rx_data_array, ARRAYSIZE)) {
        while (1);
    }
    if (!CB_Init(&ecan1_rx_buffer, rx_data_array, ARRAYSIZE)) {
        while (1);
    }

    // Initialize our time quanta
    uint16_t a = parameters[3] & 0x0007;
    uint16_t b = (parameters[3] & 0x0038) >> 3;
    uint16_t c = (parameters[3] & 0x01C0) >> 6;

    uint32_t ftq = parameters[2] / parameters[1]*10;
    ftq = ftq / (2 * (a + b + c + 4)); // Divide by the 2*number of time quanta (4 is because of the 1-offset for a/b/c and the sync segment)
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

    C1CTRL1bits.WIN = 0;

    // Return the modules to specified operating mode.
    // 0 normal, 1 disable, 2 loopback, 3 listen-only, 4 configuration, 7 listen all messages
    uint8_t desired_mode = (parameters[0] & 0x001C) >> 2;
    C1CTRL1bits.REQOP = desired_mode;
    while (C1CTRL1bits.OPMODE != desired_mode);

    // Clear all interrupt bits
    C1RXFUL1 = C1RXFUL2 = C1RXOVF1 = C1RXOVF2 = 0x0000;

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
    dmaParameters[1] = (uint16_t) & C1TXD;
    dmaParameters[2] = 7;
    dmaParameters[3] = __builtin_dmaoffset(ecan1msgBuf);
    dmaParameters[4] = ((parameters[0] >> 5) & 7);
    dmaParameters[5] = 0;
    dma_init(dmaParameters);

    // Reception DMA
    dmaParameters[0] = 0x2208;
    dmaParameters[1] = (uint16_t) & C1RXD;
    dmaParameters[4] = ((parameters[0] >> 8) & 7);
    dma_init(dmaParameters);
}

int ecan1_receive(tCanMessage *msg, unsigned char *messagesLeft)
{
    int foundOne = getMessageFromBuffer(msg, &ecan1_rx_buffer);

    if (messagesLeft) {
        if (foundOne) {
            *messagesLeft = --receivedMessagesPending;
        } else {
            *messagesLeft = 0;
        }
    }

    return foundOne;
}

int getMessageFromBuffer(tCanMessage *msg, CircularBuffer *buffer)
{
    CanUnion *bottle = (CanUnion *) msg;
    unsigned char i;

    if (buffer->dataSize >= sizeof (tCanMessage)) {
        for (i = 0; i < sizeof (tCanMessage); i++) {
            CB_ReadByte(buffer, &bottle->bytes[i]);
        }
        return TRUE;
    } else {
        // Make sure that if no message was returned that the ID is blank to signify this.
        msg->id = 0;
        return FALSE;
    }
}

void putMessageInBuffer(CircularBuffer *buffer, tCanMessage message)
{
    CanUnion bottle;
    unsigned char i;

    bottle.message = message;

    for (i = 0; i < sizeof (tCanMessage); i++) {
        CB_WriteByte(buffer, bottle.bytes[i]);
    }
}

int ecan1_receive_matlab(uint32_t *output)
{
    tCanMessage msg;

    if (receivedMessagesPending > 0) {
        getMessageFromBuffer(&msg, &ecan1_rx_buffer);

        output[0] = msg.id;
        output[1] = ((uint32_t) msg.payload[3]) << 24;
        output[1] |= ((uint32_t) msg.payload[2]) << 16;
        output[1] |= ((uint32_t) msg.payload[1]) << 8;
        output[1] |= (uint32_t) msg.payload[0];
        output[2] = ((uint32_t) msg.payload[7]) << 24;
        output[2] |= ((uint32_t) msg.payload[6]) << 16;
        output[2] |= ((uint32_t) msg.payload[5]) << 8;
        output[2] |= (uint32_t) msg.payload[4];
        output[3] = (uint32_t) msg.validBytes;

        if (msg.message_type == CAN_MSG_RTR) {
            output[3] |= 0x00000100;
        }

        output[3] |= ((uint32_t) receivedMessagesPending--) << 16;

        return TRUE;
    } else {
        int i;
        for(i = 0; i < 4; i++) {
        output[i] = 0;
        }
        return FALSE;
    }
}

// NOTE: We do not block for message transmission to complete. Message queuing
// is handled by the transmission circular buffer.

void ecan1_transmit(tCanMessage message)
{
    uint32_t word0 = 0, word1 = 0, word2 = 0;
    uint32_t sid10_0 = 0, eid5_0 = 0, eid17_6 = 0;
    uint16_t *ecan_msg_buf_ptr = ecan1msgBuf[message.buffer];

    // Variables for setting correct TXREQ bit
    uint16_t bit_to_set;
    uint16_t offset;
    uint16_t *bufferCtrlRegAddr;

    // Divide the identifier into bit-chunks for storage
    // into the registers.
    if (message.frame_type == CAN_FRAME_EXT) {
        eid5_0 = (message.id & 0x3F);
        eid17_6 = (message.id >> 6) & 0xFFF;
        sid10_0 = (message.id >> 18) & 0x7FF;
        word0 = 1;
        word1 = eid17_6;
    } else {
        sid10_0 = (message.id & 0x7FF);
    }

    word0 |= (sid10_0 << 2);
    word2 |= (eid5_0 << 10);

    // Set remote transmit bits
    if (message.message_type == CAN_MSG_RTR) {
        word0 |= 0x2;
        word2 |= 0x0200;
    }

    ecan_msg_buf_ptr[0] = word0;
    ecan_msg_buf_ptr[1] = word1;
    ecan_msg_buf_ptr[2] = ((word2 & 0xFFF0) + message.validBytes);
    ecan_msg_buf_ptr[3] = ((uint16_t) message.payload[1] << 8 | ((uint16_t) message.payload[0]));
    ecan_msg_buf_ptr[4] = ((uint16_t) message.payload[3] << 8 | ((uint16_t) message.payload[2]));
    ecan_msg_buf_ptr[5] = ((uint16_t) message.payload[5] << 8 | ((uint16_t) message.payload[4]));
    ecan_msg_buf_ptr[6] = ((uint16_t) message.payload[7] << 8 | ((uint16_t) message.payload[6]));

    // Set the correct transfer intialization bit (TXREQ) based on message buffer.
    offset = message.buffer >> 1;
    bufferCtrlRegAddr = (uint16_t *) (&C1TR01CON + offset);
    bit_to_set = 1 << (3 | ((message.buffer & 1) << 3));
    *bufferCtrlRegAddr |= bit_to_set;

    // Keep track of whether we're in a transmission train or not.
    currentlyTransmitting = 1;
}

/**
 * Transmits a tCanMessage using the transmission circular buffer.
 */
void ecan1_buffered_transmit(tCanMessage msg)
{
    // Append the message to the queue.
    // Message are only removed upon successful transmission.
    // They will be overwritten by newer message overflowing
    // the circular buffer however.
    putMessageInBuffer(&ecan1_tx_buffer, msg);

    // If this is the only message in the queue, attempt to
    // transmit it.
    if (!currentlyTransmitting) {
        ecan1_transmit(msg);
    }
}

/**
 * Merely preprocesses data from the MATLAB array format
 * into a tCanMessage to be passed to ecan1_buffered_transmit()
 */
void ecan1_buffered_transmit_matlab(uint16_t *parameters)
{
    tCanMessage message;

    message.id = ((uint32_t) parameters[1]) | (((uint32_t) parameters[2]) << 16);
    message.buffer = (uint8_t) parameters[0];

    // Set remote transmit bits
    if ((parameters[3] & 0xFF00) == 0) {
        message.message_type = CAN_MSG_DATA;
    } else {
        message.message_type = CAN_MSG_RTR;
    }

    // Set extended frame bits
    if ((parameters[3] & 0xFF) == 0) {
        message.frame_type = CAN_FRAME_STD;
    } else {
        message.frame_type = CAN_FRAME_EXT;
    }

    // Set data and data length bits
    message.payload[0] = (uint8_t) parameters[4];
    message.payload[1] = (uint8_t) ((parameters[4] & 0xFF00) >> 8);
    message.payload[2] = (uint8_t) parameters[5];
    message.payload[3] = (uint8_t) ((parameters[5] & 0xFF00) >> 8);
    message.payload[4] = (uint8_t) parameters[6];
    message.payload[5] = (uint8_t) ((parameters[6] & 0xFF00) >> 8);
    message.payload[6] = (uint8_t) parameters[7];
    message.payload[7] = (uint8_t) ((parameters[7] & 0xFF00) >> 8);
    message.validBytes = (parameters[0] & 0xFF00) >> 8;

    // Transmit the message via the circular buffer
    ecan1_buffered_transmit(message);
}

void ecan1_error_status_matlab(uint8_t *errors)
{
    // Set transmission errors in first array element.
    if (C1INTFbits.TXBO) {
        errors[0] = 3;
    } else if (C1INTFbits.TXBP) {
        errors[0] = 2;
    } else if (C1INTFbits.TXWAR) {
        errors[0] = 1;
    }

    // Set reception errors in second array element.
    if (C1INTFbits.RXBP) {
        errors[1] = 2;
    } else if (C1INTFbits.RXWAR) {
        errors[1] = 1;
    }
}

void dma_init(uint16_t *parameters)
{
    // Determine the correct addresses for all needed registers
    uint16_t offset = (parameters[4]*6);
    uint16_t *chanCtrlRegAddr = (uint16_t *) (&DMA0CON + offset);
    uint16_t *irqSelRegAddr = (uint16_t *) (&DMA0REQ + offset);
    uint16_t *addrOffsetRegAddr = (uint16_t *) (&DMA0STA + offset);
    uint16_t *secAddrOffsetRegAddr = (uint16_t *) (&DMA0STB + offset);
    uint16_t *periAddrRegAddr = (uint16_t *) (&DMA0PAD + offset);
    uint16_t *transCountRegAddr = (uint16_t *) (&DMA0CNT + offset);

    DMACS0 = 0; // Clear the status register

    *periAddrRegAddr = (uint16_t) parameters[1]; // Set the peripheral address that will be using DMA
    *transCountRegAddr = (uint16_t) parameters[2]; // Set data units to words or bytes
    *irqSelRegAddr = (uint16_t) (parameters[0] >> 8); // Set the IRQ priority for the DMA transfer
    *addrOffsetRegAddr = (uint16_t) parameters[3]; // Set primary DPSRAM start address bits
    *secAddrOffsetRegAddr = (uint16_t) parameters[5]; // Set secondary DPSRAM start address bits

    // Setup the configuration register & enable DMA
    *chanCtrlRegAddr = (uint16_t) (0x8000 | ((parameters[0] & 0x00F0) << 7) | ((parameters[0] & 0x000C) << 2));
}

/**
 * This is an interrupt handler for the ECAN1 peripheral.
 * It clears interrupt bits and pushes received message into
 * the circular buffer.
 */
void __attribute__((interrupt, no_auto_psv))_C1Interrupt(void)
{
    // Give us a CAN message struct to populate and use
    tCanMessage message;
    uint8_t ide = 0;
    uint8_t srr = 0;
    uint32_t id = 0;
    uint16_t *ecan_msg_buf_ptr;

    // If the interrupt was set because of a transmit, check to
    // see if more messages are in the circular buffer and start
    // transmitting them.
    if (C1INTFbits.TBIF) {

        // After a successfully sent message, there should be at least
        // one message in the queue, so pop it off.
        getMessageFromBuffer(&message, &ecan1_tx_buffer);

        // Check for a buffer overflow. Then clear the entire buffer if there was.
        if (ecan1_tx_buffer.overflowCount) {
            CB_Init(&ecan1_tx_buffer, tx_data_array, ARRAYSIZE);
        }

        // Now if there's still a message left in the buffer,
        // try to transmit it.
        if (ecan1_tx_buffer.dataSize >= sizeof (tCanMessage)) {
            CanUnion bottle;
            CB_PeekMany(&ecan1_tx_buffer, bottle.bytes, sizeof (tCanMessage));
            ecan1_transmit(bottle.message);
        } else {
            currentlyTransmitting = 0;
        }

        C1INTFbits.TBIF = 0;
    }

    // If the interrupt was fired because of a received message
    // package it all up and store in the circular buffer.
    if (C1INTFbits.RBIF) {

        // Obtain the buffer the message was stored into, checking that the value is valid to refer to a buffer
        if (C1VECbits.ICODE < 32) {
            message.buffer = C1VECbits.ICODE;
        }

        ecan_msg_buf_ptr = ecan1msgBuf[message.buffer];

        // Clear the buffer full status bit so more messages can be received.
        if (C1RXFUL1 & (1 << message.buffer)) {
            C1RXFUL1 &= ~(1 << message.buffer);
        }

        //  Move the message from the DMA buffer to a data structure and then push it into our circular buffer.

        // Read the first word to see the message type
        ide = ecan_msg_buf_ptr[0] & 0x0001;
        srr = ecan_msg_buf_ptr[0] & 0x0002;

        /* Format the message properly according to whether it
         * uses an extended identifier or not.
         */
        if (ide == 0) {
            message.frame_type = CAN_FRAME_STD;

            message.id = (uint32_t) ((ecan_msg_buf_ptr[0] & 0x1FFC) >> 2);
        } else {
            message.frame_type = CAN_FRAME_EXT;

            id = ecan_msg_buf_ptr[0] & 0x1FFC;
            message.id = id << 16;
            id = ecan_msg_buf_ptr[1] & 0x0FFF;
            message.id |= id << 6;
            id = ecan_msg_buf_ptr[2] & 0xFC00;
            message.id |= id >> 10;
        }

        /* If message is a remote transmit request, mark it as such.
         * Otherwise it will be a regular transmission so fill its
         * payload with the relevant data.
         */
        if (srr == 1) {
            message.message_type = CAN_MSG_RTR;
        } else {
            message.message_type = CAN_MSG_DATA;

            message.validBytes = (uint8_t) (ecan_msg_buf_ptr[2] & 0x000F);
            message.payload[0] = (uint8_t) ecan_msg_buf_ptr[3];
            message.payload[1] = (uint8_t) ((ecan_msg_buf_ptr[3] & 0xFF00) >> 8);
            message.payload[2] = (uint8_t) ecan_msg_buf_ptr[4];
            message.payload[3] = (uint8_t) ((ecan_msg_buf_ptr[4] & 0xFF00) >> 8);
            message.payload[4] = (uint8_t) ecan_msg_buf_ptr[5];
            message.payload[5] = (uint8_t) ((ecan_msg_buf_ptr[5] & 0xFF00) >> 8);
            message.payload[6] = (uint8_t) ecan_msg_buf_ptr[6];
            message.payload[7] = (uint8_t) ((ecan_msg_buf_ptr[6] & 0xFF00) >> 8);
        }

        // Store the message in the buffer
        putMessageInBuffer(&ecan1_rx_buffer, message);

        // Increase the number of messages stored in the buffer
        ++receivedMessagesPending;

        // Be sure to clear the interrupt flag.
        C1INTFbits.RBIF = 0;
    }

    // Clear the general ECAN1 interrupt flag.
    IFS2bits.C1IF = 0;

}
