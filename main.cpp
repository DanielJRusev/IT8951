#include <iostream>
#include "IT8951UsbCmd.h"
#include "IT8951_USB_API.h"

typedef bitset<8> BYTE;

int main (int argc, char *argv[])
{
    BYTE retCode = 1;
    BYTE status = 1;
    status = Open();

    if (status != 0)
    {
        UINT32 info[28];
        BYTE infoSize = GetDeviceInfo(info);
        if (infoSize > 7)
        {
            
            LOAD_IMG_AREA area;

            //x,y,w,h最好按4字节对齐(32bits)

            area.iX = 0;
            area.iY = 0;
            area.iW = info[4];
            area.iH = info[5];
            //ASSERT(area.iX % 4 == 0);//4字节对齐
            //ASSERT(area.iY % 4 == 0);//4字节对齐
            //ASSERT(area.iW % 4 == 0);//4字节对齐
            //ASSERT(area.iH % 4 == 0);//4字节对齐
            area.iAddress = info[7];
            INT32 buffLength = area.iW * area.iH;
            BYTE* img = (BYTE*)malloc(buffLength);
            memset(img, 255, buffLength);//全部设置为白色
            for (INT32 y = 300; y < area.iH; y++)
            {
                for (INT32 x = 300; x < area.iW; x++)
                {
                    img[y * area.iW + x] = 0x00;
                }
            }
            status = SendImage(img, area.iAddress, area.iX, area.iY, area.iW, area.iH);
            if (status!=0)
            {
                status = RenderImage(area.iAddress, area.iX, area.iY, area.iW, area.iH);
                //status = RenderImage(area.iAddress, 600, 600, area.iW - 600, area.iH-600);
                if (status != 0)
                {
                    retCode = 0;
                }
            }
            else
            {
                Erase(area.iAddress, area.iW * area.iH);
            }
        }
        Close();
    }


    std::cout << "Hello World!\n";

    return retCode;
}

BYTE Open() {

    BYTE nRetCode = 0 ;

    if (hDev == NULL) 
    {
        BYTE deviceNo = 0 ; // device number
        BYTE bFlag = IT8951GetDriveNo (&deviceNo); // Get the device number, what disk is the connected USB device
        if (bFlag != 0)
        {
            // Drive letter
            char* path = (char*)malloc(7);
            memcpy(path, "\\\\.\\A:", 6);
            path[6] = 0;
            path[4] = 0x41 + deviceNo;
            // Turn on the device
            HANDLE dev = IT8951OpenDeviceAPI(path);
            if (dev == INVALID_HANDLE_VALUE)
            {
                wprintf ( L " Failed to initialize the device \n " );
                return nRetCode;
            }
            else {
                nRetCode = 1;
            }
        }
    }
    return nRetCode;
    return 100;
}

BYTE GetDeviceInfo(UINT32* info){

    _TRSP_SYSTEM_INFO_DATA sysInfo;
    BYTE count = 0;
    if (IT8951GetSystemInfoAPI(&sysInfo))
    {
        UINT32* pi = (UINT32*)&sysInfo;
        /*count = sizeof(_TRSP_SYSTEM_INFO_DATA) / sizeof(UINT32) - 1;*/
        count = 28;
        for (int i = 0; i < count; i++)
        {
            info [i] = pi [i];
        }
    }
    return count;
}
BYTE SendImage(BYTE* image,UINT32 address, INT32 x, INT32 y, INT32 w, INT32 h){
    BYTE nRetCode = IT8951LoadImage(image, address, x, y, w, h);
    return nRetCode;
}
BYTE RenderImage(INT32 address, UINT32 x, INT32 y, INT32 w, INT32 h){
    DWORD mode = 2;
    DWORD waitReady = 0;
    BYTE nRetCode = IT8951DisplayAreaAPI(x, y, w, h, mode, address, waitReady);
    return nRetCode;
}
BYTE ShowImage(BYTE* image, UINT32 address, INT32 x, INT32 y, INT32 w, INT32 h){
    BYTE nRetCode = SendImage(image, address, x, y, w, h);
    if (nRetCode != 0)
    {
        nRetCode = RenderImage(address, x, y, w, h);
    }
    return nRetCode;
}
BYTE Erase(UINT32 address, INT32 size){
    TSPICmdArgEraseData eraseArea;
    eraseArea.iSPIAddress = address;
    eraseArea.iLength = size;
    BYTE nRetCode = IT8951SFIBlockEraseAPI(&eraseArea);
    return nRetCode;
}
BYTE Close() {
    if (hDev != NULL) {
        CloseHandle(hDev);
        hDev = NULL;
        return 0;
    }
    return 0;
}