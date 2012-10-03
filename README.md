# Project Description
This project is being actively developed by Mariano and Bryant with the goal of developing ECAN blocks for Simulink for compiling for the dsPIC33f with the Real Time Workshop. It is currently in the early stages of development.

## Licensing

All user-created code within this project is licensed under the standard two-clause BSD license (see LICENSE.txt).

## File Organization
**/Documentation/AN1249.pdf** - Microchip documentation for Crosswire code example.

**/Examples/** - Projects directory including examples.

**/Examples/Multireceive** - A Simulink-based project demonstrating reception of multiple messages per timestep

**/Examples/Simulink Echo** - A Simulink-based project that echoes any received messages.

**/ecan_dspic.mdl** - The Simulink library model.

**/CircularBuffer.{h,c}** - A circular buffer implementation supporting CAN message structs.

**/ecanDefinitions.h** - A file defining common strucuts, unions, and constants used by other code.

**/ecanFunctions.{h,c}** - The actual ECAN functions called from the dsPIC blocks.