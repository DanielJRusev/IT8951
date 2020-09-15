
1. 
build environment: ubuntu 16.04 (64 bit)
(the Linux SCSI generic driver must be installed)

*You can compile and execute it on same system.
*If you're using different system, this sample cannot be promised usable.
*Then it can just for reference.


2.
"make" to compile
"make clean" to clean the executable file


3. 
"sudo ./IT8951_USB /dev/sg2" to execute

*Your sg path may be not /sg2.
*On Ubuntu 16.04, you can call "lsscsi -g" to get it.
