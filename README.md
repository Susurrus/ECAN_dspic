======= Project Description
This project is being actively developed by Mariano and Bryant with the goal of developing ECAN blocks for Simulink for compiling for the dsPIC33f with the Real Time Workshop. It is currently in the early stages of development.

======= File Organization
/ - Root directory. Contains all source files.
/Documentation/ - Directory containing documentation files.
/Documentation/README.txt - This file.
/Documentation/TODO.txt - A task list for this project.
/Documentation/AN1249.pdf - Microchip documentation for Crosswire code example.
/Projects/ - Projects directory including examples.
/Projects/Crosswire - The original ECAN example project from Microchip, AN1249.
/Projects/Crosswire with library - The Crosswire project with the ECAN1 module converted to use this library's code.
/Projects/Crosswire in Simulink - The Crosswire project reimplemented in Simulink using blocks provided with this project.
/blocks.mdl - A Simulink model containing all necessary blocks for using the ECAN on dsPIC33f chips
/circBuffer.{h,c} - A circular buffer implementation supporting CAN message structs. Code deployed in autopilot so very reliable.
/ecanDefinitions.h - A file defining common strucuts, unions, and constants used by other code.
/ecanFunctions.{h,c} - The actual ECAN functions called from the dsPIC blocks.
/ecanTest.c - A file used for testing the circular buffer. Made redundant by live tests included in folders.