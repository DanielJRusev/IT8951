#ifndef IT8951_USB_CMD_H	// IT8951_USB_CMD_H{ 

#define IT8951_USB_CMD_H

#include <scsi/sg.h>
#include <sys/ioctl.h>

#define SENSE_LEN		(256)
#define BLOCK_LEN		(32)
#define SCSI_TIMEOUT	(50000)

#define SPT_BUF_SIZE  	(60*1024)//(2048)

#define IT8951_USB_INQUIRY				(0x12)

#define IT8951_USB_OP_GET_SYS			(0x80)
#define IT8951_USB_OP_READ_MEM			(0x81)
#define IT8951_USB_OP_WRITE_MEM			(0x82)
#define IT8951_USB_OP_READ_REG			(0x83)
#define IT8951_USB_OP_WRITE_REG			(0x84)
#define IT8951_USB_OP_DPY_AREA			(0x94)
#define IT8951_USB_OP_LD_IMG_AREA		(0xA2)
#define IT8951_USB_OP_PMIC_CTRL			(0xA3)
#define IT8951_USB_OP_FSET_TEMP			(0xA4)
#define IT8951_USB_OP_FAST_WRITE_MEM	(0xA5)		// if using FW not released from ITE
	#define EN_FAST_WRITE_MEM						// ensure the FW version support it first

#define RW_PAGE_SIZE   					(32*1024)
#define IT8951_USB_OP_SFI_ERASE			(0x96)
#define IT8951_USB_OP_SFI_READ			(0x97)
#define IT8951_USB_OP_SFI_WRITE			(0x98)

typedef unsigned char Byte;
typedef unsigned short Word;
typedef unsigned int DWord;

typedef struct sg_io_hdr SG_IO_HDR;

typedef struct _TRSP_SYSTEM_INFO_DATA{
	DWord uiStandardCmdNo;		// Standard command number2T-con Communication Protocol
	DWord uiExtendCmdNo; 		// Extend command number
	DWord uiSignature;		// 31 35 39 38h (8951)
	DWord uiVersion;		// command table version
	DWord uiWidth;			// Panel Width
	DWord uiHeight;			// Panel Height
	DWord uiUpdateBufBase; 		// Update Buffer Address
	DWord uiImageBufBase;		// Image Buffer Address
	DWord uiTemperatureNo;		// Temperature segment number
	DWord uiModeNo;			// Display mode number
	DWord uiFrameCount[8];		// Frame count for each mode(8).
	DWord uiNumImgBuf;		// Number of image buffer
	DWord uiWbfSFIAddr;
	DWord uiReserved[9]; 	
	void* lpCmdInfoDatas[1];	// Command table pointer
}SystemInfo;

typedef struct _TDRAW_UPD_ARG_DATA{
    DWord     uiMemAddr;
    DWord     uiWavMode;
    DWord     uiPosX;
    DWord     uiPosY;
    DWord     uiWidth;
    DWord     uiHeight;
    DWord     uiEngineIndex;
}DisplayArg;

typedef struct _TLOAD_IMG_ARG_DATA{
    DWord     uiAddress;
    DWord     uiX;
    DWord     uiY;
    DWord     uiW;
    DWord     uiH;
}LoadArg;

typedef struct _TSET_TEMP_ARG_DATA{
	Byte ucSetTemp;
	Byte ucTempVal;
}TempArg;

typedef struct  _TSPI_CMD_ARG_DATA {
    DWord     uiSPIAddress;
    DWord     uiDRAMAddress;
    DWord     uiLength;            /*Byte count*/
} FlashCmd;

typedef struct  _TSPI_CMD_ARG_ERASE_DATA {
    DWord     uiSPIAddress;
    DWord     uiLength;            /*Byte count*/
} FlashErase;


SG_IO_HDR* init_io_hdr(void);
void destroy_io_hdr(SG_IO_HDR *);
void set_xfer_data(SG_IO_HDR * p_hdr, void * data, DWord length);
void set_sense_data(SG_IO_HDR * p_hdr, Byte * data, DWord length);

Word SWAP_16(Word);
DWord SWAP_32(DWord);

int IT8951_CMD_Inq(int, int, int, SG_IO_HDR*);
int IT8951_CMD_System_Info(int, int, int, SG_IO_HDR*);
int IT8951_CMD_Read_Mem(int, int, int, SG_IO_HDR*, DWord, Word);
int IT8951_CMD_Write_Mem(int, int, int, SG_IO_HDR*, DWord, Word);
int IT8951_CMD_Read_Reg(int, int, int, SG_IO_HDR*, DWord);
int IT8951_CMD_Write_Reg(int, int, int, SG_IO_HDR*, DWord);
int IT8951_CMD_Display_Area(int, int, int, SG_IO_HDR*);
int IT8951_CMD_Load_Img(int, int, int, SG_IO_HDR*);
int IT8951_CMD_Set_Temp(int, int, int, SG_IO_HDR*, Byte, Byte);

int IT8951_CMD_Erase_Block(int, int, int, SG_IO_HDR*);
int IT8951_CMD_Read_Page(int, int, int, SG_IO_HDR*);
int IT8951_CMD_Write_Page(int, int, int, SG_IO_HDR*);

#endif // }IT8951_USB_CMD_H
