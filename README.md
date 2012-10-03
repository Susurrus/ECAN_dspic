# Project Description
This project is being actively developed by Mariano, Bryant, and Pavlo with the goal of developing ECAN blocks for Simulink for compiling for the dsPIC33f with the Real Time Workshop.

## Licensing

All user-created code within this project is licensed under the standard two-clause BSD license (see LICENSE.txt).

## File Organization
**/Documentation/AN1249.pdf** - Microchip documentation for Crosswire code example.

**/Examples/** - Projects directory including examples.

**/Examples/Multireceive** - A Simulink-based project demonstrating reception of multiple messages per timestep. (configured for dspic33fj28MC802)

**/Examples/Simulink Echo** - A Simulink-based project that echoes any received messages. (configured for dspic33fj28MC802)

**/ecan_dspic.mdl** - The Simulink library model.

**/CircularBuffer.{h,c}** - A circular buffer implementation supporting CAN message structs.

**/ecanDefinitions.h** - A file defining common strucuts, unions, and constants used by other code.

**/ecanFunctions.{h,c}** - The actual ECAN functions called from the dsPIC blocks.  ECAN message data array size is pound defined here.
