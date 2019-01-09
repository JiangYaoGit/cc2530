
Texas Instruments, Inc.

SmartRF Flash programmer  Release Notes

-------------------------------------------------------------------------------
-------------------------------------------------------------------------------

-------------------------------------------------------------------------------
N O T E!
To ensure that everything is cleaned up from the startup menu, it is 
recommended to uninstall the old version before installing the new version.
-------------------------------------------------------------------------------
Version 1.9.0.1 September 29. 2010 
-------------------------------------------------------------------------------
- Console programmer supporting CC2540 with 6 byte IEEE MAC address.
- Console programmer supporting reading of both primary and secondary IEEE addr.
-------------------------------------------------------------------------------
Version 1.9.0.0 September 28. 2010 
-------------------------------------------------------------------------------
- Support for programming of 48 bit MAC address for CC2540 added.
- Support for 8 digits number for device ID when programming bootloader for
  SmartRF05EB and CC Debugger.
- After programming of MSP430 devices the \"Verify against hex file\" action
  was enabled. This action is not implemented and should therefore not be enabled.
- Increment of Board identification (Device ID) when programming the bootloader.    
-------------------------------------------------------------------------------
Version 1.8.1.0 June 22. 2010 
-------------------------------------------------------------------------------
- Fixed problem with error message if enumeration of MSP430 device failed.
-------------------------------------------------------------------------------
Version 1.8.0.0 June 13. 2010 
-------------------------------------------------------------------------------
- Console programmer supporting patching of encryption keys before programming
  of hex file. 
- Console programmer supporting flash verification by checking the CRC of each 
  page instead of read back of every byte.
- Version numbering added to the console programmer (1.8.l).
- Compiler changed from VC++ 6.0 to VC++ 2005.   
- MSP430.dll updated to version 2.4.4.0
-------------------------------------------------------------------------------
Version 1.7.1.0 January 20. 2010 
-------------------------------------------------------------------------------
- Fixed problem with CC2533 flash size detection
- Fixed bug relating to incorrect message of "debug interface lock" when 
  attempting EPV of a chip with debug interface lock (the debug interface lock 
  is now removed after Erase operation, so there is no longer necessary to 
  attempt twice).
- Descriptive error message added when attempting to program a HEX image with 
  data at addresses exceeding the chip's flash size (earlier only "flash 
  programming failed" was reported).
- Increased max number of connected EBs supported from 32 to 64. 
- Fixed bug relating to warning/request of firmware upgrade on MSP430 Debug 
  tool when doing SoC/EB actions. The fix will only display this warning if 
  the MSP430 tab is shown.
- Filtering on not supported devices improved in SoC device list.  
- MSP430.dll updated to version 2.4.2.0
-------------------------------------------------------------------------------
Version 1.7.0.0 January 05. 2010 
-------------------------------------------------------------------------------
- Added support for CC2533
- Consol programmer now supporting read/write IEEE address for CC2530/CC2533
- SmartRF04 EB Firmware upgrade: Revision 0042
- SmartRF05 EB Firmware upgrade: Revision 0012
- CC Debugger Firmware upgrade: Revision 0012
-------------------------------------------------------------------------------
Version 1.6.2.2 April 30. 2009 
-------------------------------------------------------------------------------
- Fixed problem with programming of lock bits for bootloader and page lock
  for CC243x, CC251x and CC111x
-------------------------------------------------------------------------------
Version 1.6.1.0 April 24. 2009 
-------------------------------------------------------------------------------
- Added support for CC-Debugger
- Added support for programming of CC2530 and CC2531.
- Updated version of msp430.dll (2.4.0.0) to support CC430.
- A button to refresh the list of connected MSP430 devices has been added.
-------------------------------------------------------------------------------
Version 1.5.5.0 December 12. 2008 
-------------------------------------------------------------------------------
- Fixed problem with not giving an error message if "Retain IEEE" used on a
  chip with debug interface is locked.
-------------------------------------------------------------------------------
Version 1.5.4.0 November 20. 2008 (Beta) 
-------------------------------------------------------------------------------
- Include a fix to enable programming of bootloader on 05EB from another 05EB board
-------------------------------------------------------------------------------
Version 1.5.3.0 October 28. 2008 (Beta)
-------------------------------------------------------------------------------
- Support for reading information page if applicable.
-------------------------------------------------------------------------------
Version 1.5.2.0 October 28. 2008 (Beta)
-------------------------------------------------------------------------------
- Support for programming of 05 Temp board added. 
- Problem with overlapping input fields on GUI fixed.
- Version 2.3.5.0 of the msp430.dll
-------------------------------------------------------------------------------
Version 1.5.1.0 July 7. 2008 (Beta)
-------------------------------------------------------------------------------
- Support for programming of more devices

      
  