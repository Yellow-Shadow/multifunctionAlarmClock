/*-----------------------------------------------

  名稱：DS1302時鐘數碼管顯示
  內容：DS1302實時時鐘數碼管顯示，只顯示時間。並通過6個按鍵加減小時、分鐘、秒鐘
        
------------------------------------------------*/
#include <reg52.h> 				//包含頭文件，一般情況不需要改動，頭文件包含特殊功能寄存器的定義
#include "ds1302.h"

#define DataPort P0 			//定義數據端口，程序中遇到DataPort，則用P0替換
#define ArrayKeyPort P1
#define KeyPort P3 				//定義按鍵端口

sbit LATCH1=P2^2;				//定義鎖存使能端口 段鎖存
sbit LATCH2=P2^3;				//                 位鎖存
sbit SPK=P2^4;  				//定義音樂輸出端口

bit ReadTimeFlag;				//定義讀時間標誌
bit SetFlag;     				//更新時間標誌位
unsigned char time_buf2[16];
unsigned char time_buf3[16];

code unsigned char HOURMUSIC[]=		// 情非得已
{    
5,2,1,	1,3,1,	2,3,2,  3,3,2,	2,3,2,  2,3,2,	1,3,1,  7,2,3,	7,2,2,	5,3,4,	5,3,3,
3,3,4,	5,3,2,  3,3,1,  3,3,1,	3,3,2,	2,2,2, 	3,3,2, 	4,3,2,	3,3,4,	3,3,3,
3,3,3,	2,3,3,  2,3,3,	1,3,3,	1,3,2, 	6,2,2,  1,3,2, 	6,2,2,	2,3,1,	5,3,3,
5,2,3,	5,2,3,  6,2,3,	1,3,3,	1,3,3, 	6,2,3, 	3,3,3, 	1,3,2,	1,3,3,	0,0,0
};
code unsigned char DAYMUSIC[]=		// Happy Birthday to You
{    
5,1,3,	5,1,3,	6,1,2,  5,1,2,	1,2,2,  7,1,2,
5,1,3,	5,1,3,	6,1,2,  5,1,2,	2,2,2,  1,2,2,
5,1,3,	5,1,3,	5,2,2,  3,2,2,	1,2,2,  7,1,2,  6,1,2,
4,2,3,	4,2,4,	3,2,2,  1,2,2,	2,2,2,  1,2,2,
};
code unsigned char AlARMMUSIC[]=	// 黑人抬棺
{
6,1,2,		6,1,3,		3,2,3,    	2,2,2,		1,2,2,    
7,1,2,		7,1,3,		7,1,3,    	2,2,2,		1,2,3,		7,1,3, 
6,1,2,		6,1,3,		1,3,3,    	7,2,3,		1,3,3,    	7,2,3,    	1,3,3,
6,1,2,		6,1,3,		1,3,3,    	7,2,3,		1,3,3,    	7,2,3,    	1,3,3,

1,2,3,		1,2,3,		1,2,3,    	1,2,3,		3,2,3,		3,2,3,		3,2,3,    	3,2,3,    
2,2,3,		2,2,3,		2,2,3,    	2,2,3,		5,2,3,		5,2,3,		5,2,3,    	5,2,3,
6,2,3,		6,2,3,		6,2,3,    	6,2,3,		6,2,3,		6,2,3,		6,2,3,    	6,2,3,
6,2,3,		6,2,3,		6,2,3,    	6,2,3,		2,2,3,		1,2,3,		7,1,3,    	5,1,3,

6,1,2,		6,1,3,		3,2,3,    	2,2,2,		1,2,2,    
7,1,2,		7,1,3,		7,1,3,    	2,2,2,		1,2,3,		7,1,3, 
6,1,2,		6,1,3,		1,3,3,    	7,2,3,		1,3,3,    	7,2,3,    	1,3,3,
6,1,2,		6,1,3,		1,3,3,    	7,2,3,		1,3,3,    	7,2,3,    	1,3,3,		0,0,0	
};
/*code unsigned char MUSIC[]=		// 世上只有媽媽好
{    
6,2,3,		5,2,1,		3,2,2,    	5,2,2,    1,3,2,    6,2,1,    5,2,1,
6,2,4,		3,2,2,      5,2,1,    	6,2,1, 	  5,2,2, 	3,2,2, 	  1,2,1,
6,1,1,		5,2,1,      3,2,1,		2,2,4, 	  2,2,3, 	3,2,1,    5,2,2,
5,2,1,		6,2,1,      3,2,2,		2,2,2,    1,2,4, 	5,2,3, 	  3,2,1,
2,2,1,		1,2,1,      6,1,1,		1,2,1, 	  5,1,6, 	0,0,0 
};*/
// 音階?率表 高八位
code unsigned char FREQH[]=
{ 	//1,2,3,4,5,6,7,8,i
	0xF2,0xF3,0xF5,0xF5,0xF6,0xF7,0xF8, 
	0xF9,0xF9,0xFA,0xFA,0xFB,0xFB,0xFC,
	0xFC,0xFC,0xFD,0xFD,0xFD,0xFD,0xFE,
	0xFE,0xFE,0xFE,0xFE,0xFE,0xFE,0xFF,
};

// 音階?率表 低八位
code unsigned char FREQL[]=
{ 	//1,2,3,4,5,6,7,8,i
	0x3D,0xBD,0x14,0xB1,0xD0,0xD1,0xB5,
	0x1E,0xDE,0x8A,0xD8,0x68,0xE8,0x5A,
	0x8F,0xEE,0x45,0x6C,0xB4,0xF4,0x2D, 
	0x47,0x77,0xA2,0xB6,0xDA,0xFA,0x16,
};

unsigned char code freq[][2]=
{
	0xF2,0x3D,			//  1 ( 262 Hz L_Do)
	0xF3,0xBD,			//  2 ( 294 Hz L_Re)   
	0xF5,0x14,			//  3 ( 329 Hz L_Mi)    
	0xF5,0xB1,			//  4 ( 349 Hz L_Fa     
	0xF6,0xD0,			//  5 ( 392 Hz L_So)     
	0xF7,0xD1,			//  6 ( 440 Hz L_La)      
	0xF8,0xB5,			//  7 ( 493 Hz L_Si)
	0xF9,0x1E,			//  8 ( 523 Hz M_Do) 

	0xF9,0x1E,			//  9 ( 523 Hz M_Do)
	0xF9,0xDE,			// 10 ( 587 Hz M_Re)   
	0xFA,0x8A,			// 11 ( 659 Hz M_Mi)    
	0xFA,0xD8,			// 12 ( 698 Hz M_Fa     
	0xFB,0x68,			// 13 ( 784 Hz M_So)     
	0xFB,0xE8,			// 14 ( 880 Hz M_La)      
	0xFC,0x5A,			// 15 ( 988 Hz M_Si)
	0xFC,0x8F,			// 16 (1047 Hz H_Do) 

	0xFC,0x8F,			// 17 (1047 Hz H_Do)
	0xFC,0xEF,			// 18 (1175 Hz H_Re)   
	0xFD,0x45,			// 19 (1319 Hz H_Mi)    
	0xFD,0x6C,			// 20 (1397 Hz H_Fa     
	0xFD,0xB4,			// 21 (1568 Hz H_So)     
	0xFD,0xF4,			// 22 (1760 Hz H_La)      
	0xFE,0x2D,			// 23 (1976 Hz H_Si)
	0xFE,0x47,			// 24 (2093 Hz HH_Do) 
};

unsigned char code dofly_DuanMa[10]={0x3f,0x06,0x5b,0x4f,0x66,0x6d,0x7d,0x07,0x7f,0x6f};// ?示段?值0~9
unsigned char code dofly_WeiMa[]={0xfe,0xfd,0xfb,0xf7,0xef,0xdf,0xbf,0x7f};//分???相?的??管?亮,即位?
unsigned char TempData[8];		//儲存顯示值的全局變量
unsigned char Timer0_H, Timer0_L, Time;

void DelayUs2x(unsigned char t);//us級延時函數聲明 
void DelayMs(unsigned char t); 	//ms級延時
void Display(unsigned char FirstBit,unsigned char Num);//數據管顯示函數
unsigned char KeyScan(void);	//鍵盤掃描
void Init_Timer0(void);			//定時器初始化
void Init_Timer1(void);			//定時器初始化
void UART_Init(void);			//UART初始化
void Song(void);
void DaysReportMusic(void);
void HoursReportMusic(void);
void AlarmClockMusic(void);
/*------------------------------------------------
                    主函數
------------------------------------------------*/
void main (void)
{
	unsigned char i,num,displaynum;                 

	Init_Timer0();
	Init_Timer1();
	Ds1302_Init();
	//UART_Init();
	while (1)         			//主循環
	{
		if(SetFlag)     		//如果接收到串口信息則更新時鐘
		{
			for(i = 0; i < 8; i++)
			{
				time_buf1[i] = time_buf2[2*i]*10 + time_buf2[2*i+1];
			}					//數據整合，如兩個數字，1和5整合成15
			Ds1302_Write_Time();
			SetFlag = 0;       	//時鐘信息更新後標誌位清零
		}

		/*if(ClockSetFlag)     		//如果接收到串口信息則更新時鬧鐘
		{
			for(i = 0; i < 8; i++)
			{
				time_buf1[i] = time_buf2[2*i]*10 + time_buf2[2*i+1];
			}					//數據整合，如兩個數字，1和5整合成15
			Ds1302_Write_Time();
			SetFlag = 0;       	//時鐘信息更新後標誌位清零
		}*/

		if(time_buf1[4] == 0 && time_buf1[5] == 0 && time_buf1[6] == 0)
		{
			DaysReportMusic();
		}
		else if(time_buf1[5] == 0 && time_buf1[6] == 0)
		{
			HoursReportMusic();
		}
		else if(time_buf1[4] == 21 && time_buf1[5] == 38 && time_buf1[6] == 49)
		{
			AlarmClockMusic();
		}

		//if(time_buf1[4] == time_buf3[4] && time_buf1[5] == time_buf3[5] && time_buf1[6] == time_buf3[6])
		//{
		//	AlarmClockMusic();
		//}

		num = KeyScan();

		if(displaynum == 0)		//時鐘模式
		{	
		 	switch(num)
		    {
				case 1: time_buf1[4]++;					// HOUR++
						if(time_buf1[4] == 24)
						{
							time_buf1[4] = 0;
						}    
				        Ds1302_Write_Time();
						break; 		
	
				case 2: time_buf1[4]--; 				// HOUR--
						if(time_buf1[4] == 255)
						{
							time_buf1[4] = 23;  
						}
				        Ds1302_Write_Time();
						break;
	
				case 3: time_buf1[5]++;					// MINUTE++
						if(time_buf1[5] == 60)
						{
							time_buf1[5] = 0;
							time_buf1[4]++;
							//HoursReportMusic();
						} 
   
				        Ds1302_Write_Time();
						break;
	
				case 4: time_buf1[5]--;					// MINUTE--
						if(time_buf1[5] == 255)
						{
							time_buf1[5] = 59;
							time_buf1[4]--;
						}     
				        Ds1302_Write_Time();
						break;
	
				case 5: time_buf1[6]++;					// SECOND++
						if(time_buf1[6] == 60)
						{
							time_buf1[6] = 0;
							time_buf1[5]++;
							if(time_buf1[5] == 60)
							{
								time_buf1[5] = 0;
								time_buf1[4]++;
								//HoursReportMusic();
							}
						}     
				        Ds1302_Write_Time();
						break;
	
				case 6: time_buf1[6]--;					// SECOND--
						if(time_buf1[6] == 255)
						{
							time_buf1[6] = 59;
							time_buf1[5]--;
							if(time_buf1[5] == -1)
							{
								time_buf1[5] = 59;
								time_buf1[4]--;
							}
						}     
				        Ds1302_Write_Time();
						break;
	
				case 7: displaynum++;
						if(displaynum == 4)
							displaynum = 0;
						break;

				case 8: time_buf3[4] = 21;
						time_buf3[5] = 38;
						time_buf3[6] = 49;
						break;
	
				default:break;
			}
		}
		else if(displaynum == 1)		//日期模式
		{
		 	switch(num)
		    {
				case 1: time_buf1[1]++;					// YEAR++
						if(time_buf1[1] == 99)
						{
							time_buf1[1] = 0;
						}    
				        Ds1302_Write_Time();
						break; 		
	
				case 2: time_buf1[1]--; 				// YEAR--
						if(time_buf1[1] == 255)
						{
							time_buf1[1] = 99;  
						}
				        Ds1302_Write_Time();
						break;
	
				case 3: time_buf1[2]++;					// MONTH++
						if(time_buf1[2] == 13)
						{
							time_buf1[2] = 1;
							time_buf1[1]++;
						}     
				        Ds1302_Write_Time();
						break;
	
				case 4: time_buf1[2]--;					// MONTH--
						if(time_buf1[2] == 0)
						{
							time_buf1[2] = 12;
							time_buf1[1]--;
						}     
				        Ds1302_Write_Time();
						break;
	
				case 5: time_buf1[3]++;					// DATE++
						time_buf1[7]++;
						if(time_buf1[3] == 32)
						{
							time_buf1[3] = 1;
							time_buf1[2]++;
							if(time_buf1[2] == 13)
							{
								time_buf1[2] = 1;
								time_buf1[1]++;
							}
						}
						if(time_buf1[7] == 8)
						{
							time_buf1[7] = 1;
						}
				        Ds1302_Write_Time();
						break;
	
				case 6: time_buf1[3]--;					// DATE--
						time_buf1[7]--;
						if(time_buf1[3] == 0)
						{
							time_buf1[3] = 31;
							time_buf1[2]--;
							if(time_buf1[2] == 0)
							{
								time_buf1[2] = 12;
								time_buf1[1]--;
							}
						}
						if(time_buf1[7] == 0)
						{
							time_buf1[7] = 7;
						}        
				        Ds1302_Write_Time();
						break;
	
				case 7: displaynum++;
						if(displaynum == 4)
							displaynum = 0;
						break;
	
				default:break;
			}
		}
		else if(displaynum == 2)		//星期幾模式
		{
			switch(num)
		    {
				case 5: time_buf1[7]++;					// DATE++
						if(time_buf1[7] == 8)
						{
							time_buf1[7] = 1;
						}     
				        Ds1302_Write_Time();
						break;
	
				case 6: time_buf1[7]--;					// DATE--
						if(time_buf1[7] == 0)
						{
							time_buf1[7] = 7;
						}     
				        Ds1302_Write_Time();
						break;
	
				case 7: displaynum++;
						if(displaynum == 4)
							displaynum = 0;
						break;
	
				default:break;
			}
			
		}
		else if(displaynum == 3)						
		{
			if(num == 7)
			{
				displaynum++;
				if(displaynum == 4)
					displaynum = 0;
			}
			
		}
	
		if(ReadTimeFlag == 1)
		{
			ReadTimeFlag = 0;
			Ds1302_Read_Time();
			
			switch(displaynum)
			{
				case 0: TempData[0]=dofly_DuanMa[time_buf1[4]/10];//?	//?据的??，
						TempData[1]=dofly_DuanMa[time_buf1[4]%10];//因我?采用??管0~9的?示,??据分?
						TempData[2]=0x40;					    	        //加入"-"
						TempData[3]=dofly_DuanMa[time_buf1[5]/10];//分
						TempData[4]=dofly_DuanMa[time_buf1[5]%10];
						TempData[5]=0x40;
						TempData[6]=dofly_DuanMa[time_buf1[6]/10];//秒
						TempData[7]=dofly_DuanMa[time_buf1[6]%10];
						break;

				case 1: TempData[0]=dofly_DuanMa[time_buf1[1]/10];//年			
						TempData[1]=dofly_DuanMa[time_buf1[1]%10];
						TempData[2]=0x40;					    	        //加入"-"
						TempData[3]=dofly_DuanMa[time_buf1[2]/10];//月
						TempData[4]=dofly_DuanMa[time_buf1[2]%10];
						TempData[5]=0x40;
						TempData[6]=dofly_DuanMa[time_buf1[3]/10];//日
						TempData[7]=dofly_DuanMa[time_buf1[3]%10];
						break;

				case 2: TempData[0]=0x40;				
						TempData[1]=dofly_DuanMa[time_buf1[7]%10];//周
						TempData[2]=0x40;	//加入"-"
						TempData[3]=0;
						TempData[4]=0;
						TempData[5]=0;
						TempData[6]=dofly_DuanMa[time_buf1[6]/10];//秒
						TempData[7]=dofly_DuanMa[time_buf1[6]%10];	
						break;
				case 3: TempData[0]=0;				
						TempData[1]=0;
						TempData[2]=0;
						TempData[3]=0;
						TempData[4]=0;
						TempData[5]=0;
						TempData[6]=0;
						TempData[7]=0;	
						break;			   
			}

		}	
  }
}
/*------------------------------------------------
 uS延?函?，含有?入?? unsigned char t，?返回值
 unsigned char 是定??符?字符?量，其值的范?是
 0~255 ?里使用晶振12M，精确延??使用??,大致延?
 ?度如下 T=tx2+5 uS 
------------------------------------------------*/
void DelayUs2x(unsigned char t)
{   
	while(--t);
}
/*------------------------------------------------
 mS延?函?，含有?入?? unsigned char t，?返回值
 unsigned char 是定??符?字符?量，其值的范?是
 0~255 ?里使用晶振12M，精确延??使用??
------------------------------------------------*/
void DelayMs(unsigned char t)
{
	while(t--)
	{
		//大致延遲1mS
		DelayUs2x(245);
		DelayUs2x(245);
	}
}
/*------------------------------------------------
 ?示函?，用于???描??管
 ?入?? FirstBit 表示需要?示的第一位，如?值2表示?第三???管?始?示
 如?入0表示?第一??示。
 Num表示需要?示的位?，如需要?示99?位?值??值?入2
------------------------------------------------*/
void Display(unsigned char FirstBit,unsigned char Num)
{
	static unsigned char i=0;
	  

	DataPort=0;   //清空?据，防止有交替重影
    LATCH1=1;     //段?存
    LATCH1=0;

    DataPort=dofly_WeiMa[i+FirstBit]; //取位? 
    LATCH2=1;     //位?存
    LATCH2=0;

    DataPort=TempData[i]; //取?示?据，段?
    LATCH1=1;     //段?存
    LATCH1=0;
       
	i++;
    if(i==Num)
		i=0;


}
/*------------------------------------------------
                	歌曲處理函數
------------------------------------------------*/
void delayDay(unsigned char t)
{
    unsigned char i;
	for(i=0;i<t;i++)
	    DelayMs(150);
    TR0 = 0;
	//ET1 = 1;           		// 定時器中斷打開
}

void delayHour(unsigned char t)
{
    unsigned char i;
	for(i=0;i<t;i++)
	    DelayMs(80);
    TR0 = 0;
	//ET1 = 1;           		// 定時器中斷打開
}

void delayAlarm(unsigned char t)
{
    unsigned char i;
	for(i=0;i<t;i++)
	    DelayMs(70);
    TR0 = 0;
	//ET1 = 1;           		// 定時器中斷打開
}

void Song()
{
	TH0 = Timer0_H;//?值定?器??，?定?率
	TL0 = Timer0_L;
	TR0 = 1;       //打?定?器
	//ET1 = 0;           		// 定時器中斷打開
	//delay(Time); //延?所需要的?拍                      
}

void DaysReportMusic()
{
	unsigned char i, k;

	i = 0;  
	while(i < 73)
    {         //音????度 ，唱完??再?        
	    k = DAYMUSIC[i] + 7 * DAYMUSIC[i+1] - 1;//去音符振??率所需?据
	    Timer0_H = FREQH[k];
	    Timer0_L = FREQL[k];
	    Time = DAYMUSIC[i+2];          //?拍??
	    i = i + 3;
	    Song();
		delayDay(Time);
   }
}

void HoursReportMusic()
{
	unsigned char i, k;

	i = 0;  
	while(i < 121)
    {         //音????度 ，唱完??再?        
	    k = HOURMUSIC[i] + 7 * HOURMUSIC[i+1] - 1;//去音符振??率所需?据
	    Timer0_H = FREQH[k];
	    Timer0_L = FREQL[k];
	    Time = HOURMUSIC[i+2];          //?拍??
	    i = i + 3;
	    Song();
		delayHour(Time);
   }
}

void AlarmClockMusic()
{
	unsigned char i, k;

	i = 0;  
	while(i < 247)
    {         //音????度 ，唱完??再?        
	    k = AlARMMUSIC[i] + 7 * AlARMMUSIC[i+1] - 1;//去音符振??率所需?据
	    Timer0_H = FREQH[k];
	    Timer0_L = FREQL[k];
	    Time = AlARMMUSIC[i+2];          //?拍??
	    i = i + 3;
	    Song();
		delayAlarm(Time);
   }
}
/*------------------------------------------------
                    定時器初始化子程序
------------------------------------------------*/
void Init_Timer0(void)
{
	TMOD |= 0x01;	  //豕洏弮狾?2 (8 bits, 由 TH0 與 TL0 共同控制)，8位定時器，使用"|"符號可以在使用多個定時器時不受影響
	//TH0=0x00;	      //?定初值
	//TL0=0x00;
	EA=1;            //?中?打?
	ET0=1;           //定?器中?打?
	//TR0=1;           //定?器??打?
}
void Init_Timer1(void)
{
	TMOD |= 0x10;	  		// 使用模式 1 (16 bits, 各由 TH0 與 TL0 控制)，16位定時器，使用"|"符號可以在使用多個定時器時不受影響	     
	//TH1=256-200;	      //?定初值
	//TL1=256-200;	 
	EA=1;            		// 總中斷開關
	ET1=1;           		// 定時器中斷打開
	TR1=1;           		// 定時器開關打開
	PT1=1;
}
/*------------------------------------------------
              		串口通訊初始化
------------------------------------------------*/
void UART_Init(void)
{
	SCON = 0x50;			// 使用模式 1，8 bits UART，使能接收
	//PCON = 0x00;			// 使 SMOD = 0, but default is zero.

	TMOD = TMOD | 0x20;		// 使用模式 2 (8 bits Auto Reload, 由 TH1 控制)，8位定時器，使用"|"符號可以在使用多個定時器時不受影響
	TH1  = 0xFD;			// 0xFD = 253，鮑率 9600，晶振 11.0592M Hz
	TR1  = 1;				// 定時器開關打開

	RI   = 0;				// 可不寫，因為 SCON = 0x50 已內含
	TI   = 0;	

	EA	 = 1;            	// 總中斷開關
	ES	 = 1;				// UART中斷開關
}
/*------------------------------------------------
                 	定時器中斷子程序
------------------------------------------------*/
void Timer0_isr(void) interrupt 1
{
	//TR0 = 0;      
	TH0 = Timer0_H;
	TL0 = Timer0_L;

	SPK = !SPK;
	//TR0 = 1;
}

void Timer1_isr(void) interrupt 3
{
	static unsigned int num;
	TH1=(65536-2000)/256;		  //重新?值 2ms
	TL1=(65536-2000)%256;
	
	Display(0,8);       // ?用??管?描
	num++;
	if(num==50)        //大致100ms
	{
		num=0;
		ReadTimeFlag=1; //??志位置1
	}
}
/*------------------------------------------------
                 	UART中斷子程序
------------------------------------------------*/
void UART_isr(void) interrupt 4 using 2	// UART interrupt
{
	unsigned char Temp;          	//定義臨時變量 
	unsigned char i;

	if(RI == 1)                     //判斷是接收中斷產生
	{
		RI = 0;                     //標誌位清零

		Temp = SBUF;                //讀入緩衝區的值

		if(Temp == 'C')
		{
			time_buf3[4] = 12;
			time_buf3[5] = 34;
			time_buf3[6] = 56;
		}
		else
		{
			time_buf2[i] = Temp & 0x0F;	//只讀取ASCII碼的後四個位元
			i++;
			if(i == 16)                 //連續接收16個字符信息
			{
				i = 0;
				SetFlag = 1;            //接收完成標誌位置1
			}
			SBUF = Temp; 				//把接收到的值再傳回電腦端
		}
		
	}
	if(TI)  						//如果是發送標誌位則清零
		TI=0;	
}
/*------------------------------------------------
					按鍵掃描函數，返回掃描鍵值
------------------------------------------------*/
unsigned char KeyScan(void)
{
	unsigned char keyvalue;

	if(KeyPort!=0xff)
	{
		DelayMs(50);

		if(KeyPort!=0xff)
		{
			keyvalue = KeyPort;
			//while(KeyPort!=0xff);			// For Push Bottom Once
			switch(keyvalue)
			{
				case 0xfe:return 1;break;
				case 0xfd:return 2;break;
				case 0xfb:return 3;break;
				case 0xf7:return 4;break;
				case 0xef:return 5;break;
				case 0xdf:return 6;break;
				case 0xbf:return 7;break;
				case 0x7f:return 8;break;
				default:return 0;break;
			}
			while(KeyPort!=0xff);
		}
	}
	return 0;
}

