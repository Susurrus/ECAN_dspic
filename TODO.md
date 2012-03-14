# Tasks left

* Fix mask for ECAN ERR block to document that block.
* Move supporting C code into a clib directory.
* Fix display of the 'data' input label on the mask.
* Modify the Configure ECAN1 block to use the 'Start' function mode of the C-function block instead of the current 'Update' mode.
* Extract the data length and remove it as an input to the ECAN1 TX block.
* Remove the trigger input to the block.
    * Let users place it in a triggered subsystem if they want it triggered instead of building it into the block.
* Add a model that sends many more messages than possible during a single timestep on the press of a button to test multi-step transmission of the buffer.
* Rename the Projects directory to Examples.
* Add a README.md file to the Examples directory to document the various examples in there.
* After all of these changes, tag it as a Version 1 release.
    * Push this back up to Lubin for incorporation into his blockset.
* The ECAN ERR block should output two booleans.
* Go through and revamp all of the example code so that it's tested and works correctly.
* Extended ID messages are not handled properly for transmission using the Simulink block. It looks like that the block will truncate the ID and only send a standard transmission packet.