
#include "IT8951_USB.h"
#include <math.h>

unsigned char sense_buffer[SENSE_LEN];
unsigned char data_buffer[(SPT_BUF_SIZE+1024)];

SystemInfo *Sys_info;	// SystemInfo structure
DWord gulPanelW, gulPanelH;

// global vars for SCSI driver
char *path;
int evpd, page_code;
int status, fd;
SG_IO_HDR *p_hdr;

int main(int argc, char * argv[]) {
	
	path = argv[1];	// arg1: sg path of device
	evpd = page_code = 0;
	status = 0;
	
	fd = open(path, O_RDWR);
	
	if (!fd) {
		printf("failed to open sg file %s\n", path);
		return -1;
	}
	
	p_hdr = init_io_hdr();
	
	// 0x12: get device name
#if 0
	IT8951_Inquiry_API();
#endif
	
	// 0x80: get system info
	Sys_info=(SystemInfo*)malloc(sizeof(SystemInfo));
	IT8951_SysInfo_API(Sys_info);

	// 0x81: read memory
#if 0
	Byte *revBuf = (Byte*)malloc(sizeof(Byte)*16);
	// read 16 bytes from image buffer address
	IT8951_MemRead_API(Sys_info->uiImageBufBase, 16, revBuf);
	for(int i=0; i<16; i++){
		printf("0x%02X, %X\r\n", i, revBuf[i]);	
	}
	free(revBuf);
#endif
	
	// 0x83: read register	
#if 0
	DWord revVal;
	// read reg 0x1800110C (FW's panel width)
	IT8951_RegRead_API(0x1800110C, &revVal);
	printf("0x1800110C: 0x%X\r\n", revVal);	
#endif
	
	// 0xA2 & 0x94: load & display
#if 0
	// Mode 0: initialize all white
	IT8951_DisplayArea_API(0, 0, gulPanelW, gulPanelH, 0, (Sys_info->uiImageBufBase), 1);

	// set full white
	Byte srcW[(gulPanelW*gulPanelH)];
	memset((srcW), 0xF0, (gulPanelW*gulPanelH));
	IT8951_LoadImageArea_API(srcW, (Sys_info->uiImageBufBase), 0, 0, gulPanelW, gulPanelH);
	IT8951_DisplayArea_API(0, 0, gulPanelW, gulPanelH, 2, (Sys_info->uiImageBufBase), 1);
	
	// partial load(200x200, 4 times) & display(400x400)
	Byte src[(200*200)];
	memset((src), 0xF0, (200*200));
	IT8951_LoadImageArea_API(src, (Sys_info->uiImageBufBase), 0, 0, 200, 200);
	
	memset((src), 0x00, (200*200));
	IT8951_LoadImageArea_API(src, (Sys_info->uiImageBufBase), 200, 200, 200, 200);
	
	memset((src), 0x30, (200*200));
	IT8951_LoadImageArea_API(src, (Sys_info->uiImageBufBase), 0, 200, 200, 200);
	
	memset((src), 0x80, (200*200));
	IT8951_LoadImageArea_API(src, (Sys_info->uiImageBufBase), 200, 0, 200, 200);
	
	IT8951_DisplayArea_API(0, 0, 400, 400, 2, (Sys_info->uiImageBufBase), 1);
#endif
	
	// 0xA4: temperature
#if 0
	TempArg TempTest;
	TempTest.ucSetTemp = 0;		// 0: read, 1: write
	TempTest.ucTempVal = 126;	// must set value if want to set temp
	IT8951_TempCtrl_API(&TempTest);
#endif
	
	// 0x96, 0x97, 0x98: upgrade flash
#if 0
	printf("\r\nStart to upgrade FW to flash...\r\n");	
	
	struct stat fs;
	int fdFile;
	
	fdFile = open("DefaultTest.bin", O_RDONLY);
	fstat(fdFile, &fs);
	
	Byte* pFileTemp=(Byte*)malloc(fs.st_size);
	Byte* pCheckTemp=(Byte*)malloc(fs.st_size);
	
	int readBytes = read(fdFile, pFileTemp, fs.st_size);
	if(readBytes!=fs.st_size){
		printf("\nLoading file size errorrrrrrrrr\n");
		close(fdFile);
		return -1;
	}
	close(fdFile);
	
	char FW_SIGN[16];
	memcpy(FW_SIGN, pFileTemp, 16);
	memset(pFileTemp, 0xFF, 16);

	// Erase
	IT8951_EraseSFI(0x00000000, fs.st_size);

	// Write
	IT8951_WriteSFI(0x00000000, fs.st_size, pFileTemp);
		
	// Read & compare
	IT8951_ReadSFI(0x00000000, fs.st_size, pCheckTemp);
	
	if(memcmp(pFileTemp, pCheckTemp, fs.st_size) != 0){
		printf("\nCompare errorrrrrrrrr\n");
		
		for(int i=0; i<fs.st_size; i++){
			if(*(pFileTemp+i)!=*(pCheckTemp+i)){
				printf("%X  ", i);
			}
		}
	}

	memcpy(pFileTemp, FW_SIGN, 16);
	IT8951_WriteSFI(0x00000000, 256, pFileTemp);
	IT8951_ReadSFI(0x00000000, 256, pCheckTemp);

	if(memcmp(pFileTemp, pCheckTemp, 256) != 0){
		printf("\nFW header errorrrrrrrrr\n");
	}
	
	free(pFileTemp);
	free(pCheckTemp);
	
	printf("\r\nUpgrade process finished.\r\n");
#endif
	
	free(Sys_info);	
	close(fd);
	destroy_io_hdr(p_hdr);
	
	return EXIT_SUCCESS;
}

void show_sense_buffer(SG_IO_HDR * hdr) {
	unsigned char * buffer = hdr->sbp;

	for (int i=0; i<hdr->mx_sb_len; ++i) {
		putchar(buffer[i]);
	}
}

void IT8951_EraseSFI(DWord uiFlashAddr, DWord uiSize) {

	DWord uiBulkSize = (128*1024);
	DWord uiEraseCount = (uiSize/uiBulkSize)+((uiSize%uiBulkSize!=0)?1:0);
	
	FlashErase tempFlashErase;
	tempFlashErase.uiLength = uiBulkSize;
	
	#if defined(_Test_SW_)
	printf("\nEraseCount: %d\n", uiEraseCount);
	#endif
	
	for(DWord i=0; i<uiEraseCount; ++i){
		tempFlashErase.uiSPIAddress = uiFlashAddr+(i*uiBulkSize);

		IT8951_EraseFlash_API(&tempFlashErase);
				
		#if defined(_Test_SW_)
		printf("\nErased page: %d\n", i);
		#endif
	}
}

void IT8951_ReadSFI(DWord uiFlashAddr, DWord uiSize, Byte *pReadFlash) {

	DWord uiRemainSize = uiSize;
	DWord uiPageCount = (uiSize/RW_PAGE_SIZE)+((uiSize%RW_PAGE_SIZE!=0)?1:0);
	
	FlashCmd tempFlashRead;
	tempFlashRead.uiDRAMAddress = Sys_info->uiImageBufBase;
	
	for(DWord i=0; i<uiPageCount; ++i){
		tempFlashRead.uiSPIAddress = uiFlashAddr+(i*RW_PAGE_SIZE);
		tempFlashRead.uiLength = (uiRemainSize>RW_PAGE_SIZE)?RW_PAGE_SIZE:uiRemainSize;
		
		IT8951_ReadFlash_API(&tempFlashRead);
		IT8951_MemRead_API(Sys_info->uiImageBufBase, tempFlashRead.uiLength, pReadFlash+(i*RW_PAGE_SIZE));
		
		uiRemainSize-=tempFlashRead.uiLength;
	}
}

void IT8951_WriteSFI(DWord uiFlashAddr, DWord uiSize, Byte *pWriteFlash) {
	
	DWord uiRemainSize = uiSize;
	DWord uiPageCount = (uiSize/RW_PAGE_SIZE)+((uiSize%RW_PAGE_SIZE!=0)?1:0);
	
	FlashCmd tempFlashWrite;
	tempFlashWrite.uiDRAMAddress = Sys_info->uiImageBufBase;
	
	for(DWord i=0; i<uiPageCount; ++i){
		tempFlashWrite.uiSPIAddress = uiFlashAddr+(i*RW_PAGE_SIZE);
		tempFlashWrite.uiLength = (uiRemainSize>RW_PAGE_SIZE)?RW_PAGE_SIZE:uiRemainSize;
		
		IT8951_MemWrite_API(Sys_info->uiImageBufBase, tempFlashWrite.uiLength, pWriteFlash+(i*RW_PAGE_SIZE));
		IT8951_WriteFlash_API(&tempFlashWrite);
	
		uiRemainSize-=tempFlashWrite.uiLength;
	}
}

void IT8951_Inquiry_API(void) {

	set_xfer_data(p_hdr, data_buffer, sizeof(data_buffer));
	set_sense_data(p_hdr, sense_buffer, SENSE_LEN);

	#if defined(_Test_SW_)
	printf("\nCommand: Inquiry\n");
	#endif
	
	status = IT8951_CMD_Inq(fd, page_code, evpd, p_hdr);
	if (status!=0) {
		show_sense_buffer(p_hdr);
	} 
	else{
		#if defined(_Test_SW_)
		for(int i=8; i<36; i++){
			printf("%c", data_buffer[i]);
		}
		printf("\n");
		#endif
	} 
}

void IT8951_SysInfo_API(SystemInfo* Sys_info) {

	set_xfer_data(p_hdr, Sys_info, sizeof(SystemInfo));
	set_sense_data(p_hdr, sense_buffer, SENSE_LEN);
	
	#if defined(_Test_SW_)
	printf("\nCommand: Get device information\n");
	#endif
	
	status = IT8951_CMD_System_Info(fd, page_code, evpd, p_hdr);
	if (status!=0) {
		show_sense_buffer(p_hdr);
	} 
	else{
		SystemInfo *temp = p_hdr->dxferp;

		Sys_info->uiStandardCmdNo = SWAP_32(temp->uiStandardCmdNo);
		Sys_info->uiExtendCmdNo = SWAP_32(temp->uiExtendCmdNo);
		Sys_info->uiSignature = SWAP_32(temp->uiSignature);
		Sys_info->uiVersion = SWAP_32(temp->uiVersion);
		Sys_info->uiWidth = SWAP_32(temp->uiWidth);
		Sys_info->uiHeight = SWAP_32(temp->uiHeight);
		Sys_info->uiUpdateBufBase = SWAP_32(temp->uiUpdateBufBase);
		Sys_info->uiImageBufBase = SWAP_32(temp->uiImageBufBase);
		Sys_info->uiTemperatureNo = SWAP_32(temp->uiTemperatureNo);
		Sys_info->uiModeNo = SWAP_32(temp->uiModeNo);
			
		for(int i=0; i<8; i++){
			Sys_info->uiFrameCount[i] = SWAP_32(temp->uiFrameCount[i]);
		}

		Sys_info->uiNumImgBuf = SWAP_32(temp->uiNumImgBuf);
		Sys_info->uiWbfSFIAddr = SWAP_32(temp->uiWbfSFIAddr);
			
		for(int i=0; i<9; i++){
			Sys_info->uiReserved[i] = SWAP_32(temp->uiReserved[i]);
		}
			
		gulPanelW = Sys_info->uiWidth;
		gulPanelH = Sys_info->uiHeight;

		//#if defined(_TEST_SW_)
		printf("Panel Width: %d\n", Sys_info->uiWidth);
		printf("Panel Height: %d\n", Sys_info->uiHeight);
		printf("Image buffer address: %X\n", Sys_info->uiImageBufBase);
		//printf("Update buffer address: %X\n", Sys_info->uiUpdateBufBase);		
		//#endif
	}
}

void IT8951_MemRead_API(DWord memAddr, Word length, Byte* revBuf) {

	set_xfer_data(p_hdr, revBuf, length);
	set_sense_data(p_hdr, sense_buffer, SENSE_LEN);

	#if defined(_Test_SW_)
	printf("\nCommand: Read memory\n");
	#endif	
	
	// Notice that the memAddr should be 0x format
	status = IT8951_CMD_Read_Mem(fd, page_code, evpd, p_hdr, memAddr, length);
	if (status!=0) {
		show_sense_buffer(p_hdr);
	} 
	else{
		#if defined(_Test_SW_)
		printf("Read memory 0x%X: \r\n", memAddr);
		for(int i=0; i<length; i++){
			printf("%X \r", revBuf[i]);
		}
		printf("\n");	
		#endif
	}
}

void IT8951_MemWrite_API(DWord memAddr, Word length, Byte* srcBuf) {

	set_xfer_data(p_hdr, srcBuf, length);
	set_sense_data(p_hdr, sense_buffer, SENSE_LEN);
	
	#if defined(_Test_SW_)
	printf("\nCommand: Write memory\n");
	#endif	
	
	status = IT8951_CMD_Write_Mem(fd, page_code, evpd, p_hdr, memAddr, length);
	if (status!=0) {
		show_sense_buffer(p_hdr);
	} 
	else{
		#if defined(_TEST_SW_)
		printf("Write memory 0x%X: \r\n", memAddr);
			
		for(int i=0; i<length; i++){
			printf("%X \r", srcBuf[i]);
		}
		printf("\n");	
		#endif
	} 
}

void IT8951_RegRead_API(DWord regAddr, DWord *rev) {

	set_xfer_data(p_hdr, rev, sizeof(DWord));
	set_sense_data(p_hdr, sense_buffer, SENSE_LEN);
	
	#if defined(_Test_SW_)
	printf("\nCommand: Read register\n");
	#endif	
	
	status = IT8951_CMD_Read_Reg(fd, page_code, evpd, p_hdr, regAddr);
	if (status!=0) {
		show_sense_buffer(p_hdr);
	} 
	else{
		*rev = SWAP_32(*rev);
			
		#if defined(_Test_SW_)
		printf("Read register 0x%X: \r\n", regAddr);
		printf("%X \r", *rev);
		#endif
	}
}

void IT8951_RegWrite_API(DWord regAddr, DWord *src) {

	DWord srcIn = SWAP_32(*src);
	
	set_xfer_data(p_hdr, &srcIn, sizeof(DWord));
	set_sense_data(p_hdr, sense_buffer, SENSE_LEN);
	
	#if defined(_Test_SW_)
	printf("\nCommand: Write register\n");
	#endif
	
	status = IT8951_CMD_Write_Reg(fd, page_code, evpd, p_hdr, regAddr);
	if (status!=0) {
		show_sense_buffer(p_hdr);
	} 
	else{
		#if defined(_TEST_SW_)
		printf("Write register 0x%X: \r\n", regAddr);
		#endif
	}
}

void IT8951_DisplayArea_API(DWord dpyX, DWord dpyY, DWord dpyW, DWord dpyH, DWord dpyMode, DWord memAddr, DWord enWaitRdy){
	
	DisplayArg displayArg;
	
	displayArg.uiPosX          = SWAP_32(dpyX); //Conver Little to Big for IT8951/61
    displayArg.uiPosY          = SWAP_32(dpyY);
    displayArg.uiWidth         = SWAP_32(dpyW);
    displayArg.uiHeight        = SWAP_32(dpyH);
    displayArg.uiEngineIndex   = SWAP_32(enWaitRdy);
    displayArg.uiMemAddr       = SWAP_32(memAddr);
    displayArg.uiWavMode       = SWAP_32(dpyMode);
	
	set_xfer_data(p_hdr, &displayArg, sizeof(DisplayArg));
	set_sense_data(p_hdr, sense_buffer, SENSE_LEN);
	
	#if defined(_Test_SW_)
	printf("\nCommand: Display\n");
	#endif	
	
	status = IT8951_CMD_Display_Area(fd, page_code, evpd, p_hdr);
	if (status!=0) {
		show_sense_buffer(p_hdr);
	} 
	else{
		printf("Refreshing (%d, %d, %d, %d) with mode %d ... \r\n", dpyX, dpyY, dpyW, dpyH, dpyMode);
	}
}

void IT8951_LoadImageArea_API(Byte *srcImg, DWord memAddr, DWord ldX, DWord ldY, DWord ldW, DWord ldH){
	
	#if defined(_Test_SW_)
	printf("\nCommand: Load image\n");
	#endif
	
	if(ldW <= 2048 && ldW != gulPanelW){
			
		//Using IT8951 New USB Command for Loading Image - it Needs IT8951 F/W support
		DWord counter = (SPT_BUF_SIZE/ldW);
		LoadArg loadArg;

		for(DWord i=0; i<ldH; i+=counter){
			if(counter > (ldH-i)){
				counter = (ldH-i);
			}
				
			Byte* tempBuf = (Byte*)malloc((sizeof(LoadArg)+(ldW*counter)));
				
			loadArg.uiX = SWAP_32(ldX);
			loadArg.uiY = SWAP_32((ldY + i));
			loadArg.uiW = SWAP_32(ldW);
			loadArg.uiH = SWAP_32(counter);

			//Set Image Buffer Start Address
			loadArg.uiAddress = SWAP_32(memAddr);

			memcpy(tempBuf, &loadArg, sizeof(LoadArg));
			memcpy((tempBuf+sizeof(LoadArg)), (srcImg+(i*ldW)), (ldW*counter));
						
			set_xfer_data(p_hdr, tempBuf, (sizeof(LoadArg)+(ldW*counter)));
			set_sense_data(p_hdr, sense_buffer, SENSE_LEN);
				
			status = IT8951_CMD_Load_Img(fd, page_code, evpd, p_hdr);
			if (status!=0) {
				show_sense_buffer(p_hdr);
			} 
			else{
				// Show some information of loaded data?
			}
			 
			free(tempBuf);
		}
	}
	else{
		//Full Image - Fast Method => Send Multi Lines for each transfer
		DWord counter = (SPT_BUF_SIZE/ldW);
		
		for(DWord i=0; i<ldH; i+=counter){
			if(counter > (ldH-i)){
				counter = (ldH-i);
			}
			
			//We Send Multi Lines for each Bulk Transfer
			IT8951_MemWrite_API((memAddr+ldX+((ldY+i)*gulPanelW)), (Word)(ldW*counter), (srcImg+(i*ldW)));
		}
	}	
}

void IT8951_TempCtrl_API(TempArg *pTempCtrl) {

	Byte flag = pTempCtrl->ucSetTemp;
	Byte value = pTempCtrl->ucTempVal;
	
	set_xfer_data(p_hdr, pTempCtrl, sizeof(TempArg));
	set_sense_data(p_hdr, sense_buffer, SENSE_LEN);

	#if defined(_Test_SW_)
	printf("\r\nCommand: Temperature control\r\n");
	#endif
	
	status = IT8951_CMD_Set_Temp(fd, page_code, evpd, p_hdr, pTempCtrl->ucSetTemp, pTempCtrl->ucTempVal);
	if (status!=0) {
		show_sense_buffer(p_hdr);
	} 
	else{
		if((flag)==0x00){
			// returned temp will be stored in first byte
			Byte *temp = (p_hdr->dxferp);
			printf("Get temperature: %d\n", *temp);
		}
		else{
			printf("Set temperature, now forced as %d\r\n", value);
			printf("Please read temperature again to check it.\r\n");							
		}
	} 
}

void IT8951_EraseFlash_API(FlashErase *pEraseFlash) {

	FlashErase tempFlashErase;

	tempFlashErase.uiLength	= SWAP_32(pEraseFlash->uiLength);
	tempFlashErase.uiSPIAddress	= SWAP_32(pEraseFlash->uiSPIAddress);
		
	#if defined(_Test_SW_)
	printf("\nCommand: Erase flash\n");
	#endif
		
	set_xfer_data(p_hdr, &tempFlashErase, sizeof(FlashErase));
	set_sense_data(p_hdr, sense_buffer, SENSE_LEN);
		
	status = IT8951_CMD_Erase_Block(fd, page_code, evpd, p_hdr);
	if (status!=0) {
		printf("\nErase Flash failed!\n");
	}
}

void IT8951_ReadFlash_API(FlashCmd *pReadFlash) {

	FlashCmd tempFlashRead;
	
	tempFlashRead.uiSPIAddress	= SWAP_32(pReadFlash->uiSPIAddress);
	tempFlashRead.uiDRAMAddress	= SWAP_32(pReadFlash->uiDRAMAddress);
	tempFlashRead.uiLength	= SWAP_32(pReadFlash->uiLength);
	
	#if defined(_Test_SW_)
	printf("\nCommand: Read flash\n");
	#endif
	
	set_xfer_data(p_hdr, &tempFlashRead, sizeof(FlashCmd));
	set_sense_data(p_hdr, sense_buffer, SENSE_LEN);
		
	status = IT8951_CMD_Read_Page(fd, page_code, evpd, p_hdr);
	if (status!=0) {
		printf("\nRead Flash failed!\n");
	}
}

void IT8951_WriteFlash_API(FlashCmd *pWriteFlash) {
	
	FlashCmd tempFlashWrite;
	
	tempFlashWrite.uiSPIAddress	= SWAP_32(pWriteFlash->uiSPIAddress);
	tempFlashWrite.uiDRAMAddress	= SWAP_32(pWriteFlash->uiDRAMAddress);
	tempFlashWrite.uiLength	= SWAP_32(pWriteFlash->uiLength);
	
	#if defined(_Test_SW_)
	printf("\nCommand: Write flash\n");
	#endif
	
	set_xfer_data(p_hdr, &tempFlashWrite, sizeof(FlashCmd));
	set_sense_data(p_hdr, sense_buffer, SENSE_LEN);
		
	status = IT8951_CMD_Write_Page(fd, page_code, evpd, p_hdr);
	if (status!=0) {
		printf("\nWrite Flash failed!\n");
	}
}
