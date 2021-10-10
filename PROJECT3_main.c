/*-----------------------------------------------

  �W�١GDS1302�����ƽX�����
  ���e�GDS1302��ɮ����ƽX����ܡA�u��ܮɶ��C�óq�L6�ӫ���[��p�ɡB�����B����
        
------------------------------------------------*/
#include <reg52.h> 				//�]�t�Y���A�@�뱡�p���ݭn��ʡA�Y���]�t�S��\��H�s�����w�q
#include "ds1302.h"

#define DataPort P0 			//�w�q�ƾںݤf�A�{�Ǥ��J��DataPort�A�h��P0����
#define ArrayKeyPort P1
#define KeyPort P3 				//�w�q����ݤf

sbit LATCH1=P2^2;				//�w�q��s�ϯ�ݤf �q��s
sbit LATCH2=P2^3;				//                 ����s
sbit SPK=P2^4;  				//�w�q���ֿ�X�ݤf

bit ReadTimeFlag;				//�w�qŪ�ɶ��лx
bit SetFlag;     				//��s�ɶ��лx��
unsigned char time_buf2[16];
unsigned char time_buf3[16];

code unsigned char HOURMUSIC[]=		// ���D�o�w
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
code unsigned char AlARMMUSIC[]=	// �¤H���
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
/*code unsigned char MUSIC[]=		// �@�W�u�������n
{    
6,2,3,		5,2,1,		3,2,2,    	5,2,2,    1,3,2,    6,2,1,    5,2,1,
6,2,4,		3,2,2,      5,2,1,    	6,2,1, 	  5,2,2, 	3,2,2, 	  1,2,1,
6,1,1,		5,2,1,      3,2,1,		2,2,4, 	  2,2,3, 	3,2,1,    5,2,2,
5,2,1,		6,2,1,      3,2,2,		2,2,2,    1,2,4, 	5,2,3, 	  3,2,1,
2,2,1,		1,2,1,      6,1,1,		1,2,1, 	  5,1,6, 	0,0,0 
};*/
// ����?�v�� ���K��
code unsigned char FREQH[]=
{ 	//1,2,3,4,5,6,7,8,i
	0xF2,0xF3,0xF5,0xF5,0xF6,0xF7,0xF8, 
	0xF9,0xF9,0xFA,0xFA,0xFB,0xFB,0xFC,
	0xFC,0xFC,0xFD,0xFD,0xFD,0xFD,0xFE,
	0xFE,0xFE,0xFE,0xFE,0xFE,0xFE,0xFF,
};

// ����?�v�� �C�K��
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

unsigned char code dofly_DuanMa[10]={0x3f,0x06,0x5b,0x4f,0x66,0x6d,0x7d,0x07,0x7f,0x6f};// ?�ܬq?��0~9
unsigned char code dofly_WeiMa[]={0xfe,0xfd,0xfb,0xf7,0xef,0xdf,0xbf,0x7f};//��???��?��??��?�G,�Y��?
unsigned char TempData[8];		//�x�s��ܭȪ������ܶq
unsigned char Timer0_H, Timer0_L, Time;

void DelayUs2x(unsigned char t);//us�ũ��ɨ���n�� 
void DelayMs(unsigned char t); 	//ms�ũ���
void Display(unsigned char FirstBit,unsigned char Num);//�ƾں���ܨ��
unsigned char KeyScan(void);	//��L���y
void Init_Timer0(void);			//�w�ɾ���l��
void Init_Timer1(void);			//�w�ɾ���l��
void UART_Init(void);			//UART��l��
void Song(void);
void DaysReportMusic(void);
void HoursReportMusic(void);
void AlarmClockMusic(void);
/*------------------------------------------------
                    �D���
------------------------------------------------*/
void main (void)
{
	unsigned char i,num,displaynum;                 

	Init_Timer0();
	Init_Timer1();
	Ds1302_Init();
	//UART_Init();
	while (1)         			//�D�`��
	{
		if(SetFlag)     		//�p�G�������f�H���h��s����
		{
			for(i = 0; i < 8; i++)
			{
				time_buf1[i] = time_buf2[2*i]*10 + time_buf2[2*i+1];
			}					//�ƾھ�X�A�p��ӼƦr�A1�M5��X��15
			Ds1302_Write_Time();
			SetFlag = 0;       	//�����H����s��лx��M�s
		}

		/*if(ClockSetFlag)     		//�p�G�������f�H���h��s�ɾx��
		{
			for(i = 0; i < 8; i++)
			{
				time_buf1[i] = time_buf2[2*i]*10 + time_buf2[2*i+1];
			}					//�ƾھ�X�A�p��ӼƦr�A1�M5��X��15
			Ds1302_Write_Time();
			SetFlag = 0;       	//�����H����s��лx��M�s
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

		if(displaynum == 0)		//�����Ҧ�
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
		else if(displaynum == 1)		//����Ҧ�
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
		else if(displaynum == 2)		//�P���X�Ҧ�
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
				case 0: TempData[0]=dofly_DuanMa[time_buf1[4]/10];//?	//?�u��??�A
						TempData[1]=dofly_DuanMa[time_buf1[4]%10];//�]��?����??��0~9��?��,??�u��?
						TempData[2]=0x40;					    	        //�[�J"-"
						TempData[3]=dofly_DuanMa[time_buf1[5]/10];//��
						TempData[4]=dofly_DuanMa[time_buf1[5]%10];
						TempData[5]=0x40;
						TempData[6]=dofly_DuanMa[time_buf1[6]/10];//��
						TempData[7]=dofly_DuanMa[time_buf1[6]%10];
						break;

				case 1: TempData[0]=dofly_DuanMa[time_buf1[1]/10];//�~			
						TempData[1]=dofly_DuanMa[time_buf1[1]%10];
						TempData[2]=0x40;					    	        //�[�J"-"
						TempData[3]=dofly_DuanMa[time_buf1[2]/10];//��
						TempData[4]=dofly_DuanMa[time_buf1[2]%10];
						TempData[5]=0x40;
						TempData[6]=dofly_DuanMa[time_buf1[3]/10];//��
						TempData[7]=dofly_DuanMa[time_buf1[3]%10];
						break;

				case 2: TempData[0]=0x40;				
						TempData[1]=dofly_DuanMa[time_buf1[7]%10];//�P
						TempData[2]=0x40;	//�[�J"-"
						TempData[3]=0;
						TempData[4]=0;
						TempData[5]=0;
						TempData[6]=dofly_DuanMa[time_buf1[6]/10];//��
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
 uS��?��?�A�t��?�J?? unsigned char t�A?��^��
 unsigned char �O�w??��?�r��?�q�A��Ȫ��S?�O
 0~255 ?���ϥδ���12M�A���̩�??�ϥ�??,�j�P��?
 ?�צp�U T=tx2+5 uS 
------------------------------------------------*/
void DelayUs2x(unsigned char t)
{   
	while(--t);
}
/*------------------------------------------------
 mS��?��?�A�t��?�J?? unsigned char t�A?��^��
 unsigned char �O�w??��?�r��?�q�A��Ȫ��S?�O
 0~255 ?���ϥδ���12M�A���̩�??�ϥ�??
------------------------------------------------*/
void DelayMs(unsigned char t)
{
	while(t--)
	{
		//�j�P����1mS
		DelayUs2x(245);
		DelayUs2x(245);
	}
}
/*------------------------------------------------
 ?�ܨ�?�A�Τ_???�y??��
 ?�J?? FirstBit ��ܻݭn?�ܪ��Ĥ@��A�p?��2���?�ĤT???��?�l?��
 �p?�J0���?�Ĥ@??�ܡC
 Num��ܻݭn?�ܪ���?�A�p�ݭn?��99?��?��??��?�J2
------------------------------------------------*/
void Display(unsigned char FirstBit,unsigned char Num)
{
	static unsigned char i=0;
	  

	DataPort=0;   //�M��?�u�A���������v
    LATCH1=1;     //�q?�s
    LATCH1=0;

    DataPort=dofly_WeiMa[i+FirstBit]; //����? 
    LATCH2=1;     //��?�s
    LATCH2=0;

    DataPort=TempData[i]; //��?��?�u�A�q?
    LATCH1=1;     //�q?�s
    LATCH1=0;
       
	i++;
    if(i==Num)
		i=0;


}
/*------------------------------------------------
                	�q���B�z���
------------------------------------------------*/
void delayDay(unsigned char t)
{
    unsigned char i;
	for(i=0;i<t;i++)
	    DelayMs(150);
    TR0 = 0;
	//ET1 = 1;           		// �w�ɾ����_���}
}

void delayHour(unsigned char t)
{
    unsigned char i;
	for(i=0;i<t;i++)
	    DelayMs(80);
    TR0 = 0;
	//ET1 = 1;           		// �w�ɾ����_���}
}

void delayAlarm(unsigned char t)
{
    unsigned char i;
	for(i=0;i<t;i++)
	    DelayMs(70);
    TR0 = 0;
	//ET1 = 1;           		// �w�ɾ����_���}
}

void Song()
{
	TH0 = Timer0_H;//?�ȩw?��??�A?�w?�v
	TL0 = Timer0_L;
	TR0 = 1;       //��?�w?��
	//ET1 = 0;           		// �w�ɾ����_���}
	//delay(Time); //��?�һݭn��?��                      
}

void DaysReportMusic()
{
	unsigned char i, k;

	i = 0;  
	while(i < 73)
    {         //��????�� �A�ۧ�??�A?        
	    k = DAYMUSIC[i] + 7 * DAYMUSIC[i+1] - 1;//�h���Ů�??�v�һ�?�u
	    Timer0_H = FREQH[k];
	    Timer0_L = FREQL[k];
	    Time = DAYMUSIC[i+2];          //?��??
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
    {         //��????�� �A�ۧ�??�A?        
	    k = HOURMUSIC[i] + 7 * HOURMUSIC[i+1] - 1;//�h���Ů�??�v�һ�?�u
	    Timer0_H = FREQH[k];
	    Timer0_L = FREQL[k];
	    Time = HOURMUSIC[i+2];          //?��??
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
    {         //��????�� �A�ۧ�??�A?        
	    k = AlARMMUSIC[i] + 7 * AlARMMUSIC[i+1] - 1;//�h���Ů�??�v�һ�?�u
	    Timer0_H = FREQH[k];
	    Timer0_L = FREQL[k];
	    Time = AlARMMUSIC[i+2];          //?��??
	    i = i + 3;
	    Song();
		delayAlarm(Time);
   }
}
/*------------------------------------------------
                    �w�ɾ���l�Ƥl�{��
------------------------------------------------*/
void Init_Timer0(void)
{
	TMOD |= 0x01;	  //��ϥμҦ?2 (8 bits, �� TH0 �P TL0 �@�P����)�A8��w�ɾ��A�ϥ�"|"�Ÿ��i�H�b�ϥΦh�өw�ɾ��ɤ����v�T
	//TH0=0x00;	      //?�w���
	//TL0=0x00;
	EA=1;            //?��?��?
	ET0=1;           //�w?����?��?
	//TR0=1;           //�w?��??��?
}
void Init_Timer1(void)
{
	TMOD |= 0x10;	  		// �ϥμҦ� 1 (16 bits, �U�� TH0 �P TL0 ����)�A16��w�ɾ��A�ϥ�"|"�Ÿ��i�H�b�ϥΦh�өw�ɾ��ɤ����v�T	     
	//TH1=256-200;	      //?�w���
	//TL1=256-200;	 
	EA=1;            		// �`���_�}��
	ET1=1;           		// �w�ɾ����_���}
	TR1=1;           		// �w�ɾ��}�����}
	PT1=1;
}
/*------------------------------------------------
              		��f�q�T��l��
------------------------------------------------*/
void UART_Init(void)
{
	SCON = 0x50;			// �ϥμҦ� 1�A8 bits UART�A�ϯ౵��
	//PCON = 0x00;			// �� SMOD = 0, but default is zero.

	TMOD = TMOD | 0x20;		// �ϥμҦ� 2 (8 bits Auto Reload, �� TH1 ����)�A8��w�ɾ��A�ϥ�"|"�Ÿ��i�H�b�ϥΦh�өw�ɾ��ɤ����v�T
	TH1  = 0xFD;			// 0xFD = 253�A�j�v 9600�A���� 11.0592M Hz
	TR1  = 1;				// �w�ɾ��}�����}

	RI   = 0;				// �i���g�A�]�� SCON = 0x50 �w���t
	TI   = 0;	

	EA	 = 1;            	// �`���_�}��
	ES	 = 1;				// UART���_�}��
}
/*------------------------------------------------
                 	�w�ɾ����_�l�{��
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
	TH1=(65536-2000)/256;		  //���s?�� 2ms
	TL1=(65536-2000)%256;
	
	Display(0,8);       // ?��??��?�y
	num++;
	if(num==50)        //�j�P100ms
	{
		num=0;
		ReadTimeFlag=1; //??�Ӧ�m1
	}
}
/*------------------------------------------------
                 	UART���_�l�{��
------------------------------------------------*/
void UART_isr(void) interrupt 4 using 2	// UART interrupt
{
	unsigned char Temp;          	//�w�q�{���ܶq 
	unsigned char i;

	if(RI == 1)                     //�P�_�O�������_����
	{
		RI = 0;                     //�лx��M�s

		Temp = SBUF;                //Ū�J�w�İϪ���

		if(Temp == 'C')
		{
			time_buf3[4] = 12;
			time_buf3[5] = 34;
			time_buf3[6] = 56;
		}
		else
		{
			time_buf2[i] = Temp & 0x0F;	//�uŪ��ASCII�X����|�Ӧ줸
			i++;
			if(i == 16)                 //�s�򱵦�16�Ӧr�ūH��
			{
				i = 0;
				SetFlag = 1;            //���������лx��m1
			}
			SBUF = Temp; 				//�Ⱶ���쪺�ȦA�Ǧ^�q����
		}
		
	}
	if(TI)  						//�p�G�O�o�e�лx��h�M�s
		TI=0;	
}
/*------------------------------------------------
					���䱽�y��ơA��^���y���
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

