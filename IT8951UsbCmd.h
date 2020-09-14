#ifndef __IT8951_USB_CMD_H__  //{__IT8951_USB_CMD_H__
#define __IT8951_USB_CMD_H__

//IT8951 USB Op Code - Fill this code to CDB[6]
#define IT8951_USB_OP_GET_SYS      0x80
#define IT8951_USB_OP_READ_MEM     0x81
#define IT8951_USB_OP_WRITE_MEM    0x82
#define IT8951_USB_OP_READ_REG     0x83
#define IT8951_USB_OP_WRITE_REG    0x84

#define IT8951_USB_OP_DPY_AREA     0x94

#define IT8951_USB_OP_USB_SPI_ERASE 0x96 //Eric Added
#define IT8951_USB_OP_USB_SPI_READ  0x97
#define IT8951_USB_OP_USB_SPI_WRITE 0x98

#define IT8951_USB_OP_LD_IMG_AREA  0xA2
#define IT8951_USB_OP_PMIC_CTRL    0xA3
#define IT8951_USB_OP_IMG_CPY      0xA4 //Not used in Current Version(IT8961 Samp only)
#define IT8951_USB_OP_FSET_TEMP    0xA4

#define IT8951_USB_OP_FAST_WRITE_MEM 0xA5
	#define EN_FAST_WRITE_MEM


#define PANEL_W  gulPanelW//1024//1600
#define PANEL_H  gulPanelH//758//1200

//*****************************************************
//  Structure of Get Device Information
//*****************************************************
typedef struct _TRSP_SYSTEM_INFO_DATA
{
	unsigned int uiStandardCmdNo; // Standard command number2T-con Communication Protocol
	unsigned int uiExtendCmdNo; // Extend command number
	unsigned int uiSignature; // 31 35 39 38h (8951)
	unsigned int uiVersion; // command table version
	unsigned int uiWidth; // Panel Width
	unsigned int uiHeight; // Panel Height
	unsigned int uiUpdateBufBase; // Update Buffer Address
	unsigned int uiImageBufBase; // Image Buffer Address
	unsigned int uiTemperatureNo; // Temperature segment number
	unsigned int uiModeNo; // Display mode number
	unsigned int uiFrameCount[8]; // Frame count for each mode(8).
	unsigned int uiNumImgBuf;
	unsigned int uiWbfSFIAddr; 
	unsigned int uiReserved[8]; 
	
	void* lpCmdInfoDatas[1]; // Command table pointer
} TRSP_SYSTEM_INFO_DATA;

//--------------------------------------------------------
//    Display Area
//--------------------------------------------------------

typedef struct  _TDRAW_UPD_ARG_DATA
{
    int     iMemAddr;
    int     iWavMode;
    //int     iAlpha;
    int     iPosX;
    int     iPosY;
    int     iWidth;
    int     iHeight;
    int     iEngineIndex;
} TDRAW_UPD_ARG_DATA;
typedef TDRAW_UPD_ARG_DATA  TDrawUPDArgData;
typedef TDrawUPDArgData*    LPDrawUPDArgData;

//--------------------------------------------------------
//    Load Image Area
//--------------------------------------------------------
typedef struct _LOAD_IMG_AREA_
{
    int     iAddress;
    int     iX;
    int     iY;
    int     iW;
    int     iH;
} LOAD_IMG_AREA;
//--------------------------------------------------------
//    PMIC Control
//--------------------------------------------------------
typedef struct _PMIC_CTRL_
{
	unsigned short usSetVComVal;      
    unsigned char  ucDoSetVCom;
    unsigned char  ucDoPowerSeqSW;
    unsigned char  ucPowerOnOff;
	unsigned char  ucReserved[3];

    
}T_PMIC_CTRL;
//--------------------------------------------------------
//    Forece Set/Get Temperature Control
//--------------------------------------------------------
typedef struct 
{
	unsigned char ucSetTemp;
	unsigned char ucTempVal;

}T_F_SET_TEMP;
//--------------------------------------------------------
//    Image DMA Copy 
//--------------------------------------------------------
typedef struct _DMA_IMG_COPY_
{
    int     iSrcAddr;
    int     iDestAddr;
    int     iX;    //Src X
    int     iY;    //Src Y
    int     iW;
    int     iH;
    int     iX2;   //Target X For different (x,y)
    int     iY2;   //Target Y
} TDMA_IMG_COPY;

//---------------------------------------------------------------------------
// SPI Struct
//---------------------------------------------------------------------------
typedef struct  _TSPI_CMD_ARG_DATA
{
    int     iSPIAddress;
    int     iDRAMAddress;
    int     iLength;            /*Byte count*/
} TSPI_CMD_ARG_DATA;
typedef TSPI_CMD_ARG_DATA  TSPICmdArgData;
typedef TSPICmdArgData*     LPSPICmdArgData;
//---------------------------------------------------------------------------
typedef struct  _TSPI_CMD_ARG_ERASE_DATA
{
    int     iSPIAddress;
    int     iLength;            /*Byte count*/
} TSPI_CMD_ARG_ERASE_DATA;
typedef TSPI_CMD_ARG_ERASE_DATA  TSPICmdArgEraseData;
typedef TSPICmdArgEraseData*     LPSPICmdArgEraseData;

//----------------------------------------------------
//   For USB Command 0xA5
//----------------------------------------------------
#define TABLE_TYPE_LUT                      0  //See Tableinfo.h
#define TABLE_TYPE_VCOM                     1
#define TABLE_TYPE_TCON                     2

typedef struct 
{
	BYTE ucType;        //Table type
	BYTE ucMode;        //Display Mode
	BYTE ucTempSeg;     //Temperature segments
	BYTE ucSizeL;       //Table Size L
	BYTE ucSizeH;       //Table Size H
	BYTE ucFrameCntL;   //Frame counts L
	BYTE ucFrameCntH;   //Frame Counts H
	BYTE ucReserved;    //Reserved for padding to 8-bytes

}TSetLUTInfo;

#endif //}__IT8951_USB_CMD_H__