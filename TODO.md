# Tasks left

* Fix mask for ECAN ERR block to document that block.
* Never disable the sample time input for the ECAN TX block.
* Copy the proper blocks over to blocks.mdl.
    * Delete all of the old ones that are in it still.
* Move supporting C code into a clib directory.
* Add a README.md file.
* Fix display of the 'data' input label on the mask.
* Modify the Configure ECAN1 block to use the 'Start' function mode of the C-function block instead of the current 'Update' mode.
* Extract the data length and remove it as an input to the ECAN1 TX block.
* Remove the trigger input to the block.
    * Let users place it in a triggered subsystem if they want it triggered instead of building it into the block.
* Add a model that sends many more messages than possible during a single timestep on the press of a button to test multi-step transmission of the buffer.
* Update the in-model documentation for the Simulink Echo model.
* Rename the Projects directory to Examples.
* Add a README.md file to the Examples directory to document the various examples in there.
* After all of these changes, tag it as a Version 1 release.
    * Push this back up to Lubin for incorporation into his blockset.
* ECAN1 RX block should output a 32-bit int for the identifier
* The ECAN ERR block should output two booleans.