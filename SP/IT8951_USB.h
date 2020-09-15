#include "IT8951_CMD.h"

#ifndef Include_Main 

#define Include_Main

#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>

#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>



//#define _Test_SW_

void show_sense_buffer(SG_IO_HDR*);

// upgrade flash functions
void IT8951_EraseSFI(DWord, DWord);
void IT8951_ReadSFI(DWord, DWord, Byte*);
void IT8951_WriteSFI(DWord, DWord, Byte*);

void IT8951_Inquiry_API(void);
void IT8951_SysInfo_API(SystemInfo*);
void IT8951_MemRead_API(DWord, Word, Byte*);
void IT8951_MemWrite_API(DWord, Word, Byte*);
void IT8951_RegRead_API(DWord, DWord*);
void IT8951_RegWrite_API(DWord, DWord*);
void IT8951_DisplayArea_API(DWord, DWord, DWord, DWord, DWord, DWord, DWord);
void IT8951_LoadImageArea_API(Byte*, DWord, DWord, DWord, DWord, DWord);
void IT8951_TempCtrl_API(TempArg*);

void IT8951_EraseFlash_API(FlashErase*);
void IT8951_ReadFlash_API(FlashCmd*);
void IT8951_WriteFlash_API(FlashCmd*);

#endif
