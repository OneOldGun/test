#include "HIFIAMP.h"
#include "uart.h"
#include "commanddefs.h"
#include "propertydefs.h"
#include "si47xxFMRX.h"
#include "io.h"
#include "operationProcessing.h"

bit STB_FLAG;
bit MUTE_FLAG;
u8 xdata USB_LINK_FLAG=0;

void waitUartReceiveComplete()  //用于等待串口接收数据完成，最大延时为100MS
{
    u16  i=200;
    wptr=0;
    UART_RECEIVE_COMPLETE=0;
    Delay_ms(1);
    while((!UART_RECEIVE_COMPLETE)&&(i--))
    {
         Delay_ms(1);
    }
//     UartSend(0xaa) ;
//    UartSend(i>>8) ;
//    UartSend(i) ;
}
void InitData()//待后开机初始化一些数据
{
     USB_DISPLAY_UPDATE_FLAG=1;
     usbLastTuningFlag=1;
    // btLastTuningFlag=0 ;
}
void cpuInit()
{
   // AMP_MUTE
     //P1M0 |= 0x40;                                //设置P1.7为为强推挽模式
     //P1M1 |= 0x00;
   // P1M0 = 0x00;                                //设置P1.5为高阻输入模式
   // P1M1 = 0x20;
//    P4M0 = 0x08;                                //设置P4.3为开漏模式
//    P4M1 = 0x08;
    //P0M0 = 0x10;                                //设置P0.4为强推挽模式
    //P0M1 = 0x00;
    //IT0 = 1;                                    //使能INT0下降沿中断
    //IT1 = 0;                                    //使能INT1上升沿和下降沿中断
   // EX0 = 1;                                    //使能INT0中断
    //ET0=1; //启动定时器0中断
     //P0M0 = 0x00;                                //设置P0.1为高阻输入模式
     //P0M1 = 0x01;
    EA=1;
}
u16 xdata standbytime=0;
void musicDetect()
{
     if(STB_FLAG) return;
     if(!MUSIC_DETECT)  //低电平的时候无信号
     {
        if(time3_10ms>100)
        {
            time3_10ms=0;
            standbytime++;
            //UartSend(standbytime);
        }
        if(standbytime>3600)
        {
            standbytime=0;
            A_LOUT_EN=1;
            B_LOUT_EN=1;
            STB_LED=1;
            si47xxFMRX_powerdown();
            Delay_ms(100);
//            AP8248_POWER_EN=0;
            POWER_EN=0;
            MUSIC_MUTE=1;
            STB_FLAG=1;
            Display("            ");//无显示
            Delay_ms(5000);
        }
     }
     else  standbytime=0;
}
 bit A_LOUT_EN_FLAG=0;
 bit B_LOUT_EN_FLAG=0;
 bit phonoDetectFlag=0;

void mute(bit muteflag)
{
    if(muteflag)
    {
        if(!phonoDetectFlag)
        {
            A_LOUT_EN_FLAG=A_LOUT_EN;
            B_LOUT_EN_FLAG=B_LOUT_EN;
        }        
        A_LOUT_EN=1;
        B_LOUT_EN=1;
        MUSIC_MUTE=1;
       // AP8248MuteSet(muteflag);
    }
    else 
    {
         if(!phonoDetectFlag)
         {
            A_LOUT_EN=A_LOUT_EN_FLAG;
            B_LOUT_EN=B_LOUT_EN_FLAG;
            //AP8248MuteSet(muteflag); 
         }
         MUSIC_MUTE=0;
    }
}
void phonoDetect()
{
//    static bit A_LOUT_EN_FLAG=0;
//    static bit B_LOUT_EN_FLAG=0;
    if(STB_FLAG)   return;
    if(!phonoDetectFlag)
    {
        if(!PHONO_DETECT)
        {
            Delay_ms(100);
            if(!PHONO_DETECT)
            {
                 Delay_ms(100);
                if(!PHONO_DETECT)
                {
                    if(!MUTE_FLAG)
                    {
                        A_LOUT_EN_FLAG=A_LOUT_EN;
                        B_LOUT_EN_FLAG=B_LOUT_EN;
                    }
                    phonoDetectFlag=1;
                    A_LOUT_EN=1;
                    B_LOUT_EN=1;
                   // LINEOUT_MUTE=1;
                }
            }
        }
    }
    else if(PHONO_DETECT)
    {
         Delay_ms(100);
         if(PHONO_DETECT)
         {
             phonoDetectFlag=0;
             if(!MUTE_FLAG)
             {
                 A_LOUT_EN=A_LOUT_EN_FLAG;
                 B_LOUT_EN=B_LOUT_EN_FLAG;
                 //LINEOUT_MUTE=0;
             }
         }
    }
}






/************************************************************************************************
函数名称：AP8248GetVersionInfo
函数功能：获取AP8248版本号
入口参数：
出口参数：无
************************************************************************************************/
void AP8248GetVersionInfo()
{
    u8  datax[7];
    datax[0] = 0x55;
    datax[1] = 0xAA;
    datax[2] = 0x00;
    datax[3] = 0x00;
    datax[4] = 0x01;
    datax[5] = 0x05;
    datax[6] = 0x16;
    wptr=0;
    Uart2SendStr(datax,7);
    //UART_RECEIVE_COMPLETE=0;
   // while(!UART_RECEIVE_COMPLETE);
    waitUartReceiveComplete();
    //Delay_ms(100);
    //UartSendStr(receiveBuffer,20);
	wptr=0;
}
	  
/************************************************************************************************
函数名称：AP8248LinkState
函数功能：AP8248连接状态
入口参数：
出口参数：无
************************************************************************************************/
u8 AP8248LinkState()
{
    u8  datax[7];
    datax[0] = 0x55;
    datax[1] = 0xAA;
    datax[2] = 0x00;
    datax[3] = 0x00;
    datax[4] = 0x01;
    datax[5] = 0x04;
    datax[6] = 0x16;
    wptr=0;
    Uart2SendStr(datax,7);
    //UART_RECEIVE_COMPLETE=0;
    //while(!UART_RECEIVE_COMPLETE);
    waitUartReceiveComplete();
    //Delay_ms(100);
    //UartSendStr(receiveBuffer,20);
	  wptr=0;
	  if(receiveBuffer[6]==0x05)  return (1);
	  else  return (0);    
}
/************************************************************************************************
函数名称：AP8248Upgrade()
函数功能：AP8248升级函数
入口参数：
            
出口参数：无
************************************************************************************************/
 void AP8248Upgrade()
 {
    Display("Upgrade...."); 
    AP8248LinkState();
    sendData[0] = 0x55;
    sendData[1] = 0xAA;
    sendData[2] = 0x00;
    sendData[3] = 0x01;
    sendData[4] = 0x01;
    sendData[5] = 0x02;
    sendData[6] = 0x03;
    sendData[7] = 0x16;
    wptr=0;
    Uart2SendStr(sendData,8);
    Delay_ms(1000);
    wptr=0;
    
    AP8248PlaySet(0x01);
    Delay_ms(1000);
    wptr=0;
    sendData[0] = 0x55;
    sendData[1] = 0xAA;
    sendData[2] = 0x00;
    sendData[3] = 0x00;
    sendData[4] = 0x05;
    sendData[5] = 0x01;
    sendData[6] = 0x16;
    wptr=0;
    Uart2SendStr(sendData,7);
    Delay_ms(15000);
    //UartSendStr(receiveBuffer,wptr);
    wptr=0;
    
    //获取版本号
    AP8248GetVersionInfo();
    Delay_ms(100);
    //UartSendStr(receiveBuffer,20);
   
     //Delay_ms(1000);
    
    displayData[0] =0X00;//""
    displayData[1] =0x56;//"V"
    displayData[2] =0x30+receiveBuffer[6];//
    displayData[3] =0X2E;
    displayData[4] =0x30+receiveBuffer[7];//
    displayData[5] =0x2E;//
    displayData[6] =0x30+receiveBuffer[8];//
    displayData[7] =0x00;//
    displayData[8] =0x00;//
    displayData[9] =0x00;//
    displayData[10]=0x00;
    displayData[11]=0x00;
    Display(displayData);
    wptr=0;
    Delay_ms(5000);
}  

/************************************************************************************************
函数名称：AP8248BalanceSet
函数功能：AP8248平衡设置函数
入口参数：balance_L   左声道音量动态调节(-12~12db) -12db:0x00    0db:0x0C   +12db:0x18
          balance_R   右声道音量动态调节(-12~12db) -12db:0x00    0db:0x0C   +12db:0x18  
出口参数：无
************************************************************************************************/
void AP8248BalanceSet(u8 balance_L,u8 balance_R)
{
    u8  datax[9];
    datax[0] = 0x55;
    datax[1] = 0xAA;
    datax[2] = 0x00;
    datax[3] = 0x02;
    datax[4] = 0x04;
    datax[5] = 0x07;
    datax[6] = balance_L;
    datax[7] = balance_R;
    datax[8] = 0x16;
    wptr=0;
    Uart2SendStr(datax,9);
    //UART_RECEIVE_COMPLETE=0;
    //while(!UART_RECEIVE_COMPLETE) ;
    waitUartReceiveComplete();
    wptr=0;
    //Delay_ms(100);
    //wptr=0;
}

/************************************************************************************************
函数名称：AP8248TuneSet
函数功能：AP8248高低音提升设置函数
入口参数：bass : 0-20    10:为0db
          treble : 0-20   10:为0db  
出口参数：无
************************************************************************************************/
void AP8248TuneSet(u8 bass,u8 treble)
{
    u8  datax[11];
    datax[0] = 0x55;
    datax[1] = 0xAA;
    datax[2] = 0x00;
    datax[3] = 0x04;
    datax[4] = 0x07;
    datax[5] = 0x03;
    if(bass<10)datax[6] = 0xf6+bass;
    else datax[6] = bass-10;
    datax[7] = 0;
    if(treble<10)datax[8] = 0xf6+treble;
    else datax[8] = treble-10;
    datax[9] = 0;
    datax[10] = 0x16;
    wptr=0;
    Uart2SendStr(datax,11);
    //UART_RECEIVE_COMPLETE=0;
    //while(!UART_RECEIVE_COMPLETE) ;
    waitUartReceiveComplete();
    wptr=0;
   // Delay_ms(100);
    //UartSendStr(receiveBuffer,wptr);
   // wptr=0;
}

/************************************************************************************************
函数名称：AP8248PlayCommand()
函数功能：AP8248播放指定的文件
入口参数：
出口参数：无
************************************************************************************************/
/*void AP8248PlaySelFile(u16 fileNum)
{
    u8  datax[9];
    datax[0] = 0x55;
    datax[1] = 0xAA;
    datax[2] = 0x00;
    datax[3] = 0x02;
    datax[4] = 0x03;
    datax[5] = 0x01;
    datax[6] = fileNum>>8;
    datax[7] = fileNum;
    datax[8] = 0x16;
    wptr=0;
    Uart2SendStr(datax,9);
    //UART_RECEIVE_COMPLETE=0;
    //while(!UART_RECEIVE_COMPLETE) ;
    waitUartReceiveComplete();
    wptr=0;
   // Delay_ms(100);
   // UartSendStr(receiveBuffer,wptr);
    //wptr=0;
    usbLastTuningFlag=2;  time_10ms=290; USB_DISPLAY_UPDATE_FLAG=1;
    
} */

/************************************************************************************************
函数名称：AP8248PlaySet
函数功能：AP8248播放设置函数
入口参数：value  0x02--暂停
                 0x03--停止  0x04--下曲
                 0x05--上一曲 0x06--播放/暂停
出口参数：无
************************************************************************************************/
void AP8248PlaySet(u8 value)
{
    u8  datax[7];
    datax[0] = 0x55;
    datax[1] = 0xAA;
    datax[2] = 0x00;
    datax[3] = 0x00;
    datax[4] = 0x03;
    datax[5] = value;
    datax[6] = 0x16;
    wptr=0;
    Uart2SendStr(datax,7);
   // UART_RECEIVE_COMPLETE=0;
    //while(!UART_RECEIVE_COMPLETE)Delay_ms(1);
    waitUartReceiveComplete();
    wptr=0;
    Delay_ms(20);
    wptr=0;
    if(value==0x04||value==0x05||value==0x06)
    {
         //UartSend(0xcc) ;
         usbLastTuningFlag=1;  time_10ms=280; 
        USB_DISPLAY_UPDATE_FLAG=1; 
        //Delay_ms(100);
       //displayUsbID3_TAGSetConfirm();
    }
}



/************************************************************************************************
函数名称：AP8248PlaySelFile(u16 filNum)
函数功能：AP8248播放选中的文件
入口参数：filNum---文件编号
出口参数：无
************************************************************************************************/
void AP8248PlaySelFile(u16 filNum)
{
    u8  datax[9];
    datax[0] = 0x55;
    datax[1] = 0xAA;
    datax[2] = 0x00;
    datax[3] = 0x02;
    datax[4] = 0x03;
    datax[5] = 0x01;
    datax[6] = filNum>>8;
    datax[7] = filNum;
    datax[8] = 0x16;
    wptr=0;
    Uart2SendStr(datax,9);
   // UART_RECEIVE_COMPLETE=0;
    //while(!UART_RECEIVE_COMPLETE)Delay_ms(1);
    waitUartReceiveComplete();
    wptr=0;
    usbLastTuningFlag=1;  time_10ms=280; 
    USB_DISPLAY_UPDATE_FLAG=1; 
}
/************************************************************************************************
函数名称：AP824GetPlayState()
函数功能：
入口参数：

出口参数：  0:IDLE
            1:PLAYING
            2:PAUSE

************************************************************************************************/

u8 AP824GetPlayState()
{
    u8  datax[7];
    datax[0] = 0x55;
    datax[1] = 0xAA;
    datax[2] = 0x00;
    datax[3] = 0x00;
    datax[4] = 0x03;
    datax[5] = 0x12;
    datax[6] = 0x16;
    wptr=0;
    Uart2SendStr(datax,7);
    waitUartReceiveComplete();
    wptr=0;
    //UART_RECEIVE_COMPLETE=0;
    //while(!UART_RECEIVE_COMPLETE) ;
    //songPlayTime=(u16)(receiveBuffer[10]<<8 )|receiveBuffer[11];
    //Delay_ms(100);
    //wptr=0;
    return receiveBuffer[7];
}
/************************************************************************************************
函数名称：AP824GetPlayTime()
函数功能：
入口参数：

出口参数：无
************************************************************************************************/
u16 xdata songPlayTime=0;
void AP824GetPlayTime()
{
    u8  datax[7];
    datax[0] = 0x55;
    datax[1] = 0xAA;
    datax[2] = 0x00;
    datax[3] = 0x00;
    datax[4] = 0x03;
    datax[5] = 0x13;
    datax[6] = 0x16;
    wptr=0;
    Uart2SendStr(datax,7);
   // UART_RECEIVE_COMPLETE=0;
   // while(!UART_RECEIVE_COMPLETE) Delay_ms(1);
    waitUartReceiveComplete();
    wptr=0;
    //Delay_ms(100);
   // UartSendStr(receiveBuffer,20);
    songPlayTime=(u16)(receiveBuffer[8]<<8 )|receiveBuffer[9];
    
   // return receiveBuffer[7];
}


/************************************************************************************************
函数名称：AP8248VolumeSet
函数功能：AP8248音量设置函数
入口参数：currentVol  //当前音量
出口参数：无
************************************************************************************************/
void AP8248VolumeSet(u8 currentVol)
{   
    u8  datax[8];
    datax[0] = 0x55;
    datax[1] = 0xAA;
    datax[2] = 0x00;
    datax[3] = 0x01;
    datax[4] = 0x04;
    datax[5] = 0x03;
    datax[6] = currentVol;
    datax[7] = 0x16;
    wptr=0;
    Uart2SendStr(datax,8);
    //UART_RECEIVE_COMPLETE=0;
    //while(!UART_RECEIVE_COMPLETE)Delay_ms(1) ;
    waitUartReceiveComplete();
    //Delay_ms(20);
    //UartSendStr(receiveBuffer,wptr);
    wptr=0;
//    if((receiveBuffer[6]!=currentVol) //未成功再发一次
//    {
//        Uart2SendStr(datax,8);
//        Delay_ms(10);
//        wptr=0;
//    }
}


/************************************************************************************************
函数名称：AP8248MicVolumeSet
函数功能：AP8248MIC音量设置函数
入口参数：currentVol  //当前音量
出口参数：无
************************************************************************************************/
void AP8248MicVolumeSet(u8 currentMicVol)
{   
    u8  datax[8];
    datax[0] = 0x55;
    datax[1] = 0xAA;
    datax[2] = 0x00;
    datax[3] = 0x01;
    datax[4] = 0x04;
    datax[5] = 0x10;
    datax[6] = currentMicVol;
    datax[7] = 0x16;
    wptr=0;
    Uart2SendStr(datax,8);
    //UART_RECEIVE_COMPLETE=0;
    //while(!UART_RECEIVE_COMPLETE) ;
    waitUartReceiveComplete();
    wptr=0;
    //Delay_ms(20);
    //UartSendStr(receiveBuffer,wptr);
    //wptr=0;
//    if((receiveBuffer[6]!=currentVol) //未成功再发一次
//    {
//        Uart2SendStr(datax,8);
//        Delay_ms(10);
//        wptr=0;
//    }
}


/************************************************************************************************
函数名称：AP8248MicEchoSet
函数功能：AP8248MICEcho设置函数
入口参数：u8 EchoEnable    1:ENABLE 0:DISABLE
          u16 Cutofffrequency    (0~24000 in HZ ,default : 2000)
          u8 Attenuation (0~-73db,default: -7db)
          u16 Delay(0~1000ms,default: 200)
          u8 Direct (直达声开关, default:1)
出口参数：无
************************************************************************************************/
void AP8248MicEchoSet(u8 EchoEnable,u16 Cutofffrequency,u8 Attenuation,u16 Delay,u8 Direct)
{   
    u8  xdata datax[14];
    datax[0] = 0x55;
    datax[1] = 0xAA;
    datax[2] = 0x00;
    datax[3] = 0x07;
    datax[4] = 0x07;
    datax[5] = 0x02;
    datax[6] = EchoEnable;
    datax[7] = Cutofffrequency>>8;
    datax[8] = Cutofffrequency;
    datax[9] = Attenuation;
    datax[10] = Delay>>8;
    datax[11] = Delay;
    datax[12] = Direct;
    datax[13] = 0x16;
    wptr=0;
    Uart2SendStr(datax,14);
    //UART_RECEIVE_COMPLETE=0;
    //while(!UART_RECEIVE_COMPLETE) ;
    waitUartReceiveComplete();
    wptr=0;
    //Delay_ms(20);
    //UartSendStr(receiveBuffer,wptr);
    //wptr=0;
}

/************************************************************************************************
函数名称：AP8248GetFileName()
函数功能：获取文件夹的信息
入口参数:
出口参数：无
************************************************************************************************/
void AP8248GetFileName()
{   
    u8  datax[8];
    datax[0] = 0x55;
    datax[1] = 0xAA;
    datax[2] = 0x00;
    datax[3] = 0x00;
    datax[4] = 0x02;
    datax[5] = 0x03;
    datax[6] = 0x16;
    wptr=0;
    Uart2SendStr(datax,7);
    //UART_RECEIVE_COMPLETE=0;
    //while(!UART_RECEIVE_COMPLETE) Delay_ms(1);
    waitUartReceiveComplete();
    wptr=0;
   // Delay_ms(100);
   // UartSendStr(receiveBuffer,wptr);
    usbCurrentSelFileNumInALL=(u16)(receiveBuffer[6]<<8 )|receiveBuffer[7];
    //wptr=0;
}

/************************************************************************************************
函数名称：AP8248GetFolderInfo(u16 folderNum)
函数功能：获取文件夹的信息
入口参数：folderNum  文件夹编号，为0时代表当前文件夹
出口参数：无
************************************************************************************************/
/*void AP8248GetFolderInfo(u16 folderNum)
{   
    u8  datax[8];
    datax[0] = 0x55;
    datax[1] = 0xAA;
    datax[2] = 0x00;
    datax[3] = 0x02;
    datax[4] = 0x02;
    datax[5] = 0x13;
    datax[6] = folderNum>>8;
    datax[7] = folderNum;
    datax[8] = 0x16;
    wptr=0;
    Uart2SendStr(datax,9);
    //UART_RECEIVE_COMPLETE=0;
    //while(!UART_RECEIVE_COMPLETE) Delay_ms(1);
    waitUartReceiveComplete();
    wptr=0;
    // Delay_ms(100);
   // UartSendStr(receiveBuffer,wptr);
    //wptr=0;
    
    usbCurrentSelFolderNum= (u16)(receiveBuffer[6]<<8 )|receiveBuffer[7];
    usbCurrentSelFolderFirstFileInALL=(u16)(receiveBuffer[8]<<8 )|receiveBuffer[9];
    usbCurrentSelFolderAllFileNum= (u16)(receiveBuffer[10]<<8 )|receiveBuffer[11];
        
    //UartSend(usbCurrentSelFolderNum);
   // UartSend(usbCurrentSelFolderAllFileNum);
    
}
    */


/************************************************************************************************
函数名称：AP8248GetRootFolderInfo()
函数功能：获取根目录文件夹的信息
入口参数：根目录的文件夹编号一直为1
出口参数：无
************************************************************************************************/
/*u16 xdata usbRootFolderAllFileNum;

void AP8248GetRootFolderInfo()
{   
    u8 xdata datax[8];
    datax[0] = 0x55;
    datax[1] = 0xAA;
    datax[2] = 0x00;
    datax[3] = 0x02;
    datax[4] = 0x02;
    datax[5] = 0x13;
    datax[6] = 0x00;
    datax[7] = 0x01;
    datax[8] = 0x16;
    wptr=0;
    Uart2SendStr(datax,9);
    //UART_RECEIVE_COMPLETE=0;
    //while(!UART_RECEIVE_COMPLETE) ;
    Delay_ms(100);
   // UartSendStr(receiveBuffer,wptr);
    wptr=0;
    
    // usbCurrentSelFolderNum= (u16)(receiveBuffer[6]<<8 )|receiveBuffer[7];
    //usbCurrentSelFolderFirstFileInALL=(u16)(receiveBuffer[8]<<8 )|receiveBuffer[9];
    usbRootFolderAllFileNum= (u16)(receiveBuffer[10]<<8 )|receiveBuffer[11];
    //UartSend(0xaa);
    //UartSend(usbRootFolderAllFileNum);
//    //UartSend(usbCurrentSelFolderAllFileNum);
    //UartSend(0xaa);
    
}
  */
/************************************************************************************************
函数名称：AP8248RepeatModeSet(u8 mode);
函数功能：设置重复模式
入口参数：  0: REPEAT_ALL(所有循环)
            1: RANDOM_ALL(全盘随机)
            2: REPEAT_ONE (单曲循环)
            3: REPEAT_ FOLDER (文件夹循环)
            4: RANDOM _FOLDER(文件夹随机)
            5: REPEAT_OFF(顺序模式)

出口参数：无
************************************************************************************************/
void AP8248RepeatModeSet(u8 mode)
{
    u8  datax[8];
    datax[0] = 0x55;
    datax[1] = 0xAA;
    datax[2] = 0x00;
    datax[3] = 0x01;
    datax[4] = 0x03;
    datax[5] = 0x07;
    datax[6] = mode;
    datax[7] = 0x16;
    wptr=0;
    Uart2SendStr(datax,8);
    //UART_RECEIVE_COMPLETE=0;
    //while(!UART_RECEIVE_COMPLETE) ;
    waitUartReceiveComplete();
    wptr=0;
    //Delay_ms(100);
    //UartSendStr(receiveBuffer,wptr);
    //wptr=0;
    
}
 
/************************************************************************************************
函数名称：AP8248GetCurrentFolderInfo()
函数功能：获取当前文件夹的信息
入口参数：无
出口参数：无
************************************************************************************************/
void AP8248GetCurrentFolderInfo()
{
    u8  datax[7];
    datax[0] = 0x55;
    datax[1] = 0xAA;
    datax[2] = 0x00;
    datax[3] = 0x00;
    datax[4] = 0x02;
    datax[5] = 0x14;
    datax[6] = 0x16;
    wptr=0;
    Uart2SendStr(datax,7);
    //UART_RECEIVE_COMPLETE=0;
    //while(!UART_RECEIVE_COMPLETE)Delay_ms(1) ;
    waitUartReceiveComplete();
    wptr=0;
    //Delay_ms(100);
   // UartSendStr(receiveBuffer,20);
//    if(wptr<13) 
//    {
//    }
//    else if(wptr>13)
    //{
         usbCurrentSelFolderNum= (u16)(receiveBuffer[6]<<8 )|receiveBuffer[7];
         usbCurrentSelFolderFirstFileInALL=(u16)(receiveBuffer[8]<<8 )|receiveBuffer[9];
         usbCurrentSelFolderAllFileNum= (u16)(receiveBuffer[10]<<8 )|receiveBuffer[11];
   // }
   // wptr=0;
}


/************************************************************************************************
函数名称：AP8248selectFile(u16 fileNum)
函数功能：选择指定的文件
入口参数：无
出口参数：无
************************************************************************************************/
/*void AP8248selectFile(u16 fileNum)
{
    u8 datax[9];
    datax[0] = 0x55;
    datax[1] = 0xAA;
    datax[2] = 0x00;
    datax[3] = 0x02;
    datax[4] = 0x02;
    datax[5] = 0x06;
    datax[6] = fileNum>>8;
    datax[7] = fileNum;
    datax[8] = 0x16;
    wptr=0;
    Uart2SendStr(datax,9);
    //UART_RECEIVE_COMPLETE=0;
    //while(!UART_RECEIVE_COMPLETE) Delay_ms(1);
    waitUartReceiveComplete();
    wptr=0;
    //Delay_ms(100);
   // UartSendStr(receiveBuffer,wptr);
    //wptr=0;   
} */

/************************************************************************************************
函数名称：AP8248selectFolder(u16 folderNum)
函数功能：选择指定的文件夹
入口参数：无
出口参数：无
************************************************************************************************/
void AP8248selectFolder(u16 folderNum)
{
    u8 datax[9];
   // AP8248PlaySet(0x03);  //先要停止状态
    datax[0] = 0x55;
    datax[1] = 0xAA;
    datax[2] = 0x00;
    datax[3] = 0x02;
    datax[4] = 0x02;
    datax[5] = 0x15;
    datax[6] = folderNum>>8;
    datax[7] = folderNum;
    datax[8] = 0x16;
    wptr=0;
    Uart2SendStr(datax,9);
    //UART_RECEIVE_COMPLETE=0;
    //while(!UART_RECEIVE_COMPLETE) Delay_ms(1);
    waitUartReceiveComplete();
    //wptr=0;
    //Delay_ms(100);
    //UartSendStr(receiveBuffer,20);
    wptr=0;
    usbCurrentSelFolderNum= (u16)(receiveBuffer[6]<<8)|receiveBuffer[7];
    usbCurrentSelFolderFirstFileInALL=(u16)(receiveBuffer[8]<<8)|receiveBuffer[9];
    usbCurrentSelFolderAllFileNum= (u16)(receiveBuffer[10]<<8 )|receiveBuffer[11];
   //AP8248PlaySet(0x06);
   
}   

/************************************************************************************************
函数名称：AP8248PreOrNextFolder(u8 temp)
函数功能：向上或者向下选 择文件夹
入口参数：temp  0x16,选择上一个文件夹。0X17选择下一个文件夹
出口参数：无
************************************************************************************************/
void AP8248PreOrNextFolder(u8 temp)
{
    u8  datax[7];
   // AP8248PlaySet(0x03);  //先要停止状态
    datax[0] = 0x55;
    datax[1] = 0xAA;
    datax[2] = 0x00;
    datax[3] = 0x00;
    datax[4] = 0x02;
    datax[5] = temp;
    datax[6] = 0x16;
    wptr=0;
    Uart2SendStr(datax,7);
    //UART_RECEIVE_COMPLETE=0;
    //while(!UART_RECEIVE_COMPLETE)Delay_ms(1);
    waitUartReceiveComplete();
   //Delay_ms(100);
    wptr=0;
    //UartSendStr(receiveBuffer,20);
   // wptr=0;
    usbCurrentSelFolderNum= (u16)(receiveBuffer[6]<<8)|receiveBuffer[7];
    usbCurrentSelFolderFirstFileInALL=(u16)(receiveBuffer[8]<<8)|receiveBuffer[9];
    usbCurrentSelFolderAllFileNum= (u16)(receiveBuffer[10]<<8 )|receiveBuffer[11];
   //AP8248PlaySet(0x06);
   
}




/************************************************************************************************
函数名称：AP8248GetCurrentSongInfo()
函数功能：获取当前歌曲时间长度信息
入口参数：无
出口参数：无
************************************************************************************************/
void AP8248GetCurrentSongInfo()
{   
    u8  datax[7];
    datax[0] = 0x55;
    datax[1] = 0xAA;
    datax[2] = 0x00;
    datax[3] = 0x00;
    datax[4] = 0x02;
    datax[5] = 0x07;
    datax[6] = 0x16;
    wptr=0;
    Uart2SendStr(datax,7);
   // UART_RECEIVE_COMPLETE=0;
    //while(!UART_RECEIVE_COMPLETE)Delay_ms(1);
    waitUartReceiveComplete();
    wptr=0;
   // Delay_ms(100);
    //UartSendStr(receiveBuffer,20);
    
    usbCurrentFileTotalPlayTime =(u16)(receiveBuffer[21]<<8 )|receiveBuffer[22];
    usbCurrentSelFileNumInALL =(u16)(receiveBuffer[7]<<8 )|receiveBuffer[8];
  //  UartSend(0xaa);
  //  UartSend(usbCurrentSelFileNumInALL);
//    UartSend(usbCurrentFileTotalPlayTime>>8);
//    UartSend(usbCurrentFileTotalPlayTime);
   // UartSend(0xbb);
    //wptr=0;
}



/************************************************************************************************
函数名称：AP8248GetFileID3(u8 id3mode);
函数功能：AP8248GetFileID3函数
入口参数：id3mode//USBID3模式选择 1:TITLE,2:ARTIST 3:ALBUM 
出口参数：无
************************************************************************************************/
void AP8248GetFileID3(u8 id3mode)
{   
    u8  datax[7];
    datax[0] = 0x55;
    datax[1] = 0xAA;
    datax[2] = 0x00;
    datax[3] = 0x00;
    datax[4] = 0x02;
    switch(id3mode)
    {
        case 1:
            datax[5] = 0x08;
        break;
        case 2:
            datax[5] = 0x09;
        break;
        case 3:
            datax[5] = 0x10;
        break;
        default:
            break;
    }
    datax[6] = 0x16;
    wptr=0;
    Uart2SendStr(datax,7);
    //UART_RECEIVE_COMPLETE=0;
    //while(!UART_RECEIVE_COMPLETE) Delay_ms(1);
    waitUartReceiveComplete();
    wptr=0;
   // Delay_ms(100);
    //UartSendStr(receiveBuffer,40);
   // wptr=0;
}


/************************************************************************************************
函数名称：AP8248MuteSet
函数功能：AP8248静音设置函数
入口参数：mute 0-静音关闭  1-静音打开
出口参数：无
************************************************************************************************/

void AP8248MuteSet(bit mute)
{
    u8  datax[7];
    datax[0] = 0x55;
    datax[1] = 0xAA;
    datax[2] = 0x00;
    datax[3] = 0x00;
    datax[4] = 0x04;
    datax[6] = 0x16;
    if(mute)
    {
        datax[5] = 0x05;   //AP8248静音打开
    }
    else  datax[5] = 0x06;//AP8248静音关闭
    wptr=0;
    Uart2SendStr(datax,7);
    //UART_RECEIVE_COMPLETE=0;
    //while(!UART_RECEIVE_COMPLETE)Delay_ms(1);
    waitUartReceiveComplete();
    wptr=0;
    //Delay_ms(50);
    //wptr=0;
//    if((receiveBuffer[4]&0x80)!=0x80) //未成功再发一次
//    {
//        wptr=0;
//        Uart2SendStr(datax,7);
//        Delay_ms(2);
//        wptr=0;
//    }
}


/************************************************************************************************
函数名称：AP8248InputModeSeL(u8 modeID);
函数功能：输入通道设置函数
入口参数：modeID ： 1:LINE INE ,2:sd, 3:usb 4:optical 5:COAXIAL
出口参数：无
************************************************************************************************/
void AP8248InputModeSeL(u8 modeID)
{
    u8  datax[8];
    datax[0] = 0x55;
    datax[1] = 0xAA;
    datax[2] = 0x00;
    datax[3] = 0x01;
    datax[4] = 0x01;
    datax[5] = 0x02;
    datax[6] = modeID;
    datax[7] = 0x16;
    wptr=0;
    Uart2SendStr(datax,8);
   // UART_RECEIVE_COMPLETE=0;
   // while(!UART_RECEIVE_COMPLETE) ;
    waitUartReceiveComplete();
    wptr=0;
    //Delay_ms(100);
}


/************************************************************************************************
函数名称：inputSet
函数功能：输入通道设置函数
入口参数：inputNum ： 0-9
出口参数：无
************************************************************************************************/

void inputSet(u8 inputNum)
{
   // u8  datax[8];
   // AP8248MuteSet(1);
   // wptr=0;
    //MUTE_MUTE=1;
    //Delay_ms(100);
    switch(inputNum)
    {
        case CH_FM:
            CD4052_A2   = 1 ;
            CD4052_B2   = 0 ; 
            AP8248InputModeSeL(1);
            break;
        case CH_USB:
            usbLoadTime=0;
            CD4052_A1   = 0 ;
            CD4052_B1   = 1 ;
            CD4052_A2   = 1 ;
            CD4052_B2   = 1 ;
            AP8248InputModeSeL(3);
            break;
        case CH_BT:
            CD4052_A2   = 0 ;
            CD4052_B2   = 1 ;
            AP8248InputModeSeL(1);
            break;
        case CH_AUX1:
            CD4052_A1   = 1 ;
            CD4052_B1   = 1 ;
            CD4052_A2   = 1 ;
            CD4052_B2   = 1 ;
           AP8248InputModeSeL(1);
            break;
        case CH_AUX2:
            CD4052_A1   = 0 ;
            CD4052_B1   = 0 ;
            CD4052_A2   = 1 ;
            CD4052_B2   = 1 ; 
            AP8248InputModeSeL(1);
            break;
        case CH_AUX3:
            CD4052_A1   = 1 ;
            CD4052_B1   = 0 ;
            CD4052_A2   = 1 ;
            CD4052_B2   = 1 ; 
            AP8248InputModeSeL(1);
            break;
        case CH_PHONO:
            CD4052_A2   = 0 ;
            CD4052_B2   = 0 ;
           AP8248InputModeSeL(1);
            break;
        case CH_OPTICAL:
            CD4052_A1   = 0 ;
            CD4052_B1   = 1 ;
            CD4052_A2   = 1 ;
            CD4052_B2   = 1 ;
           AP8248InputModeSeL(4);
            break;
        case CH_COAX:
            CD4052_A1   = 0 ;
            CD4052_B1   = 1 ;
            CD4052_A2   = 1 ;
            CD4052_B2   = 1 ;
            AP8248InputModeSeL(5);
            break;
        default:
            break;
            
    }
   // wptr=0;
    //Delay_ms(100);
    //MUTE_MUTE=0;
    //AP8248MuteSet(0);
}

/************************************************************************************************
函数名称：AP824MicSet(u8 open)
函数功能：MIC输入通道设置
入口参数：open ： 0-关闭  1-打开
出口参数：无
************************************************************************************************/
 /*
void AP824MicSet(u8 open)
{
    u8 xdata datax[8];
    datax[0] = 0x55;
    datax[1] = 0xAA;
    datax[2] = 0x00;
    datax[3] = 0x01;
    datax[4] = 0x06;
    datax[5] = 0x01;
    datax[6] = open;
    datax[7] = 0x16;
    wptr=0;
    Uart2SendStr(datax,8);
    //UART_RECEIVE_COMPLETE=0;
    //while(!UART_RECEIVE_COMPLETE) ;
    waitUartReceiveComplete();
    wptr=0;
    //Delay_ms(100);
    //wptr=0;
}
  */

/************************************************************************************************
函数名称：AP824GetFileNum(u8 fileSum)
函数功能：获得文件数量，或者文件夹的数量
入口参数：cmd ： 1-文件夹的数量  2-文件数量
出口参数：无
************************************************************************************************/

void AP824GetFileNum(u8 cmd)
{
    u8  datax[8];
    datax[0] = 0x55;
    datax[1] = 0xAA;
    datax[2] = 0x00;
    datax[3] = 0x00;
    datax[4] = 0x02;
    datax[5] = cmd;
    datax[6] = 0x16;
    wptr=0;
    Uart2SendStr(datax,7);
    //UART_RECEIVE_COMPLETE=0;
   // while(!UART_RECEIVE_COMPLETE) ;
    waitUartReceiveComplete();
    //Delay_ms(100);
    wptr=0;
   // if(wptr==9)
    {
         if(cmd==1)  
         {
             usbMaxFolderNum=(u16)(receiveBuffer[6]<<8 )|receiveBuffer[7];
            // UartSend(0xcc);
             //UartSend(usbMaxFolderNum);
             //UartSend(0xcc);
         }
         else if(cmd==2)  
         {
             usbMaxFileNum=(u16)(receiveBuffer[6]<<8 )|receiveBuffer[7];
            // UartSend(usbMaxFileNum);
         }
    }
}

/************************************************************************************************
函数名称：AP824EQSet(u8 eqNum)
函数功能：EQ设置
入口参数：eqNum ：
                1:CLSCL
                2:ROCK
                3:POP
                4:JAZZ
                5:BASS 1
                6:BASS 2
                7:L+ BASS
                8:R+ BASS
                9:EQ,OFF  （Flat(默认音场)）
出口参数：无
************************************************************************************************/
void AP824EQSet(u8 eqNum)
{
    u8 xdata datax[8];
    datax[0] = 0x55;
    datax[1] = 0xAA;
    datax[2] = 0x00;
    datax[3] = 0x01;
    datax[4] = 0x07;
    datax[5] = 0x01;
    datax[6] = eqNum;
    datax[7] = 0x16;
    wptr=0;
    Uart2SendStr(datax,8);
    //UART_RECEIVE_COMPLETE=0;
    //while(!UART_RECEIVE_COMPLETE) ;
    waitUartReceiveComplete();
   // Delay_ms(100);
    wptr=0;
}
void UsbRepeatModeSet(void)
{
    if(usbRandomSel==1) //关闭随机
    {
         // 1：OFF,2:ALL,3:FOLDER 4:ONE
         switch(usbRepeatMode)
        {
            case 1: //
                AP8248RepeatModeSet(5);// 顺序播放
                break;
            case 2:
                AP8248RepeatModeSet(0);//全盘循环
                break;
            case 3:
                AP8248RepeatModeSet(3);//文件夹循环
                break;
            case 4:
                AP8248RepeatModeSet(2); //单曲循环
                break;
            default:break;
        }
    }
    else if(usbRandomSel==2)
    {
         AP8248RepeatModeSet(1);//全盘随机
    }
    else if(usbRandomSel==3)
    {
        AP8248RepeatModeSet(4);//文件夹随机
    }
}

void Power_On(void)
{
    MUSIC_MUTE=1;
    POWER_EN=1;
    A_LOUT_EN=1;
    B_LOUT_EN=1;
    VOL_SEL_KNOB_A=1;        //PINA置高电平
    VOL_SEL_KNOB_B=1; 
    BASS_SEL_KNOB_A=1;        //BASS_SEL_KNOB_A高电平
    BASS_SEL_KNOB_B=1;        //BASS_SEL_KNOB_B高电平
    TREBLE_SEL_KNOB_A=1;        //BASS_SEL_KNOB_A高电平
    TREBLE_SEL_KNOB_B=1;        //BASS_SEL_KNOB_B高电平
    STB_LED=0;
    MUTE_FLAG=0;
    STB_FLAG=0;
    BT_REST=0;
    BT_PAIR  = 0;
    BT_PRE   = 0;
    BT_NEXT  = 0;
    BT_PP    = 0;
    //Delay_ms(500);
    POWER_EN=1;	
    VFD_RESET     =0;
    Delay_ms(200);
    VFD_RESET   =1;
    Delay_ms(200);
    VFDIC_init();
    displayPowerOn();
    Delay_ms(4000);
    AP8248MuteSet(1) ;
   // Delay_ms(4000);
    BT_REST=1;
    if(!MENU_SEL_KNOB)
    { 
       Delay_ms(2000); 
       if(!MENU_SEL_KNOB) 
       {
           AP8248Upgrade();
       }
    }
	//AP8248GetVersionInfo();
    Backup_Load();
    FMRadioStationLoad();
    si47xxFMRX_initialize();
    si47xx_getPartInformation();
    si47xxFMRX_set_volume(0X3F);
    inputSet(inPutChannel);
    displayInputMenu();
    usbLoadTime=0;
    //AP8248MuteSet(1);
    //AP824MicSet(1);
    AP8248MicEchoSet(0X01,2000,7,micEcho*20,0X01);
    AP8248MicVolumeSet(micVol);
    //if(maxVol <initVol) 
    if(masterVol>initVol){masterVol=initVol;AP8248VolumeSet(initVol);}else AP8248VolumeSet(masterVol);
   // AP8248MuteSet(0) ;
    //Delay_ms(100);
    Delay_ms(2000);
    if((inPutChannel==2)&USB_LINK_FLAG)InitData();
    
    UsbRepeatModeSet();  //必须放在延时后面，否则USB记忆可能会被覆盖掉
    
    AP8248TuneSet(bass,treble);
    if(balanceData==0) 
    {
        AP8248BalanceSet(22,12);
    }
    else if((balanceData<10)&& (balanceData>0))
    {
        AP8248BalanceSet(22-balanceData,12);
    }
    else if(balanceData==10)
    {
        AP8248BalanceSet(12,12);
    }
    else if((balanceData<20)&& (balanceData>10))
    {
        AP8248BalanceSet(12,balanceData+2);
    }
    else if(balanceData==20)
    {
        AP8248BalanceSet(12,22);
    }   
    AP824EQSet(usbEQNum);
    
    
    switch(groupState)
    {
        case 0:
            A_LOUT_EN   =0; 
            B_LOUT_EN   =1; 
            break;
        case 1:
            A_LOUT_EN   =1; 
            B_LOUT_EN   =0; 
            break;
        case 2:
            A_LOUT_EN   =0; 
            B_LOUT_EN   =0; 
            break;
        default:
            break;
    }
    MUSIC_MUTE=0;
    usbLoadTime=0;//USB必须加载完成后，再操作
    
}

void main()
{
    //POWER_EN=0;
    Timer6Init();
    UartInit();
    Uart2Init();
    cpuInit();
    Power_On();
    IR_Init();
    //Delay_ms(1000);
    while(1)
    {
        processing();
        EventPro();
        Backup();
        FMRadioStationBackup();
        phonoDetect();
        musicDetect();
    }
    	
}