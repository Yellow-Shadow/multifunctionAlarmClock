
#include "ds1302.h"

unsigned char time_buf1[8] = {20,9,3,13,18,51,00,6};
unsigned char time_buf[8] ;                         
/*------------------------------------------------
           向DS1302寫入一字節數據
------------------------------------------------*/
void Ds1302_Write_Byte(unsigned char addr, unsigned char d)
{// 先寫再寫

	unsigned char i;
	RST_SET;	
	
	//寫入目標地址：addr
	addr = addr & 0xFE;		// "RD/W"位元設為"0" 取用Write (1111 1110)
	for (i = 0; i < 8; i ++) 
	    { 
		if (addr & 0x01) 
		    {
			IO_SET;
			}
		else 
		    {
			IO_CLR;
			}
		SCK_SET;
		SCK_CLR;
		addr = addr >> 1;	// 右移
		}
	
	//寫入數據：d
	for (i = 0; i < 8; i ++) 
	   {
		if (d & 0x01) 
		    {
			IO_SET;
			}
		else 
		    {
			IO_CLR;
			}
		SCK_SET;
		SCK_CLR;
		d = d >> 1;			// 右移
		}
	RST_CLR;				//停止DS1302總線
}
/*------------------------------------------------
           從DS1302讀出一字節數據
------------------------------------------------*/

unsigned char Ds1302_Read_Byte(unsigned char addr) 
{// 先寫再讀

	unsigned char i;
	unsigned char temp;
	RST_SET;	

	//寫入目標地址：addr
	addr = addr | 0x01;		// "RD/W"位元設為"1" 取用Read
	for (i = 0; i < 8; i ++) 
	    {
	     
		if (addr & 0x01) 
		   {
			IO_SET;
			}
		else 
		    {
			IO_CLR;
			}
		SCK_SET;
		SCK_CLR;
		addr = addr >> 1;	// 右移
		}
	
	//輸出數據：temp
	for (i = 0; i < 8; i ++) 
	    {
		temp = temp >> 1;	// 右移
		if (IO_R) 
		   {
			temp |= 0x80;
			}
		else 
		   {
			temp &= 0x7F;
			}
		SCK_SET;
		SCK_CLR;
		}
	
	RST_CLR;				//停止DS1302總線
	return temp;
}

/*------------------------------------------------
           向DS1302寫入時鐘數據
------------------------------------------------*/
void Ds1302_Write_Time(void) 
{
     
    unsigned char i,tmp;
	for(i=0;i<8;i++)
	    {                  //BCD處理
		tmp=time_buf1[i]/10;
		time_buf[i]=time_buf1[i]%10;
		time_buf[i]=time_buf[i]+tmp*16;
	    }
	Ds1302_Write_Byte(ds1302_control_add,0x00);			//關閉 寫保護鎖 
	Ds1302_Write_Byte(ds1302_sec_add,0x80);				//暫停時時鐘 
	//Ds1302_Write_Byte(ds1302_charger_add,0xa9);			//涓流充電
	Ds1302_Write_Byte(ds1302_year_add,time_buf[1]);		//年 
	Ds1302_Write_Byte(ds1302_month_add,time_buf[2]);	//月 
	Ds1302_Write_Byte(ds1302_date_add,time_buf[3]);		//日��  
	Ds1302_Write_Byte(ds1302_hr_add,time_buf[4]);		//時 
	Ds1302_Write_Byte(ds1302_min_add,time_buf[5]);		//分
	Ds1302_Write_Byte(ds1302_sec_add,time_buf[6]);		//秒
	Ds1302_Write_Byte(ds1302_day_add,time_buf[7]);		//星期幾 
	Ds1302_Write_Byte(ds1302_control_add,0x80);			//開啟 寫保護鎖 
}

/*------------------------------------------------
           從DS1302讀出時鐘數據
------------------------------------------------*/
void Ds1302_Read_Time(void)  
{ 
   	    unsigned char i,tmp;
	time_buf[1]=Ds1302_Read_Byte(ds1302_year_add);		//年
	time_buf[2]=Ds1302_Read_Byte(ds1302_month_add);		//月 
	time_buf[3]=Ds1302_Read_Byte(ds1302_date_add);		//日 
	time_buf[4]=Ds1302_Read_Byte(ds1302_hr_add);		//時 
	time_buf[5]=Ds1302_Read_Byte(ds1302_min_add);		//分 
	time_buf[6]=(Ds1302_Read_Byte(ds1302_sec_add))&0x7F;//秒 
	time_buf[7]=Ds1302_Read_Byte(ds1302_day_add);		//星期幾 


	for(i=0;i<8;i++)
	   {           //BCD處理
		tmp=time_buf[i]/16;
		time_buf1[i]=time_buf[i]%16;
		time_buf1[i]=time_buf1[i]+tmp*10;
	   }
}

/*------------------------------------------------
                DS1302初始化
------------------------------------------------*/
void Ds1302_Init(void)
{
	
	RST_CLR;			//RST低電位
	SCK_CLR;			//SCK低電位
    Ds1302_Write_Byte(ds1302_sec_add,0x00);				 
}
