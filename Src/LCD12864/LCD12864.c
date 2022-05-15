/*
 * LCD12864.c
 *
 *  Created on: 2015-6-23
 *      Author: Administrator
 */

/*
 * 2022.5.15��ǣ����ڵ��־������룬���Բ������ע�;������ݣ�����
 *                  ����ͨ����������������ȷ������
 */

#include <msp430f5529.h>
#include <math.h>
#include <stdio.h>
/*ע�⣡*/
#define P_six
//#define P_eight

#define uchar unsigned char
#define uint  unsigned int
#define ulong unsigned long

#define CPU_F ((double)16000000)
#define delay_us(x) __delay_cycles((long)(CPU_F*(double)x/1000000.0))
#define Delay_Nms(x) __delay_cycles((long)(CPU_F*(double)x/1000.0))

char tab_SPI1[16];
char tab_SPI2[16];
char tab_SPI3[16];
char tab_SPI4[16];


/*12864����Ӧ��ָ��*/
#define CLEAR_SCREEN        0x01                    //����ָ�������ACֵΪ00H
#define AC_INIT                0x02                    //��AC����Ϊ00H�����α��Ƶ�ԭ��λ��
#define CURSE_ADD        0x06                    //�趨�α��Ƶ�����ͼ�������ƶ�����Ĭ���α����ƣ�ͼ�����岻����
#define FUN_MODE        0x30                    //����ģʽ��8λ����ָ�
#define DISPLAY_ON        0x0c                    //��ʾ��,��ʾ�α꣬���α�λ�÷���
#define DISPLAY_OFF        0x08                    //��ʾ��
#define CURSE_DIR        0x14                    //�α������ƶ�:AC=AC+1
#define SET_CG_AC        0x40                    //����AC����ΧΪ��00H~3FH
#define SET_DD_AC        0x80
/*12864��չӦ��ָ��*/
#define SP_MODE      0x34	// 8λ���ݣ�����ָ�����?
#define GRAPON    0x36	// ����ָ�ͼ����ʾ��
#define	gdbas     0x80
//*******************************************************************
//     12864 ��ͼ�����?
//

#define LCD_DataIn    P3DIR=0x00    //���ݿڷ�������Ϊ����
#define LCD_DataOut   P3DIR=0xff    //���ݿڷ�������Ϊ���?
#define LCD2MCU_Data  P3IN
#define MCU2LCD_Data  P3OUT

#ifdef P_eight
#define LCD_CMDOut    P8DIR |= (BIT1+BIT2);P7DIR |=BIT0     //P3�ڵĵ���λ����Ϊ���?
#define LCD_RS_H      P8OUT|=BIT1      //8.1��
#define LCD_RS_L      P8OUT&=~BIT1     //P8.1��
#define LCD_RW_H      P8OUT|=BIT2      //P8.2��
#define LCD_RW_L      P8OUT&=~BIT2     //P8.2��
#define LCD_EN_H      P7OUT|=BIT0      //7.0��
#define LCD_EN_L      P7OUT&=~BIT0     //P7.0
#endif

#ifdef P_six
#define LCD_CMDOut    P6DIR|=0x70      //P6�� 0111 0000��λ����Ϊ���?
#define LCD_RS_H      P6OUT|=BIT6      //P6.6��
#define LCD_RS_L      P6OUT&=~BIT6     //P6.6��
#define LCD_RW_H      P6OUT|=BIT5      //P6.5��
#define LCD_RW_L      P6OUT&=~BIT5     //P6.5��
#define LCD_EN_H      P6OUT|=BIT4      //P6.4��
#define LCD_EN_L      P6OUT&=~BIT4     //P6.4��
#endif


/*******************************************
�������ƣ�Write_Cmd
��    �ܣ���Һ����д��������
��    ����cmd--��������
����ֵ  ����
********************************************/
void Write_Cmd(uchar cmd)
{
    uchar lcdtemp = 0;

    LCD_RS_L;
    LCD_RW_H;
    LCD_DataIn;
    do                       //��æ
    {
        LCD_EN_H;
//       	_NOP();
       	lcdtemp = LCD2MCU_Data;
       	LCD_EN_L;

    }
    while(lcdtemp & 0x80);

    LCD_DataOut;
    LCD_RW_L;
    MCU2LCD_Data = cmd;
    LCD_EN_H;
//    __delay_cycles(8);
    LCD_EN_L;
}
/*******************************************
�������ƣ�Write_Data
��    �ܣ���Һ����д��ʾ����
��    ����dat--��ʾ����
����ֵ  ����
********************************************/
void  Write_Data(uchar dat)
{
    uchar lcdtemp = 0;

    LCD_RS_L;
    LCD_RW_H;
    LCD_DataIn;
    do                       //��æ
    {
        LCD_EN_H;
//        _NOP();
        lcdtemp = LCD2MCU_Data;
        LCD_EN_L;
    }
    while(lcdtemp & 0x80);

    LCD_DataOut;
    LCD_RS_H;
    LCD_RW_L;

    MCU2LCD_Data = dat;
    LCD_EN_H;
//    __delay_cycles(8);
    LCD_EN_L;
}
/*******************************************
//        ������
********************************************/
uchar read_data()
{
    uint readvalue;
    uchar lcdtemp = 0;
    LCD_DataOut;
    MCU2LCD_Data |=0xff;
    LCD_RS_L;
        LCD_RW_H;
        LCD_DataIn;
        do                       //��æ
        {
            LCD_EN_H;
//            _NOP();
            lcdtemp = LCD2MCU_Data;
            LCD_EN_L;
        }
        while(lcdtemp & 0x80);

    LCD_RS_H;
    LCD_RW_H;
    LCD_DataIn;
    LCD_EN_L;

    LCD_EN_H;
    readvalue=LCD2MCU_Data;
//    Delay_Nms(1);
    LCD_EN_L;
    return readvalue;

}
/********************************************************/
//��    �ƣ�LCD_SetXY()**************************
//��    �ܣ���λ����,����DDRAM����ʾ����***********************
//��ڲ�������?/xȡֵ��Χ(byte)��1��8,��/yȡֵ��Χ(byte)**************
 /*************************************/
void LCD_SetXY(uchar X, uchar Y)
{
   switch(Y) {
		case 1: Write_Cmd(0x7F + X);break;
		case 2: Write_Cmd(0x8F + X);break;
		case 3: Write_Cmd(0x87 + X);break;
		case 4: Write_Cmd(0x97 + X);break;
                default:break;
	    }
}
//*******************************************************
//* ��    �ƣ�disp_char()
//* ��    �ܣ���ָ����λ����ʾĳ���ַ�
//* ��ڲ�������?/xȡֵ��Χ(byte)��1��8,��/yȡֵ��Χ(byte)
//* ���ز�����void
/********************************/
void disp_char(uchar X, uchar Y, uchar wdata)
{
    LCD_SetXY(X, Y);
    Write_Data(wdata);
}

//***************************************************************
//* �������ܣ���ָ����λ����ʾ�ַ���
//* ��    �ƣ�lcd12864_disp_str()
//* ��    �ܣ�����Ļ����ʾһ���ַ���
//* ��ڲ�������?/xȡֵ��Χ(byte)��1��8,��/yȡֵ��Χ(byte)��1��4,Str---�ַ������׵�ַ
//* ���ز�����void
/******************************************************************/
void lcd12864_disp_str(uchar X, uchar Y, char*Str)
{
    uchar temp;
    LCD_SetXY(X, Y);
    temp = *Str;
    while(temp != 0)
    {
        Write_Data(temp);
        temp = *(++Str);
    }
}

//********************************************************
//* ��    �ƣ�GUI_Point()
//* ��    �ܣ���Һ������ָ�����괦��һ���㣬������������
//* ��ڲ�����y--����(0~63)��x--����(0~127), color=1�õ����?,color=0���õ����?
//* ���ز�����void
//********************************************************
void GUI_Point(unsigned char x,unsigned char y,unsigned char color)
{
  unsigned char x_Dyte,x_byte; //�����е�ַ���ֽ�λ�������ֽ��е���1 λ
  unsigned char y_Dyte,y_byte; //����Ϊ����������(ȡֵΪ0��1)���е�ַ(ȡֵΪ0~31)
  unsigned char GDRAM_hbit,GDRAM_lbit;

  Write_Cmd(0x36); //��չָ������
/***X,Y ���껥��������ͨ��X,Y ����***/
  x_Dyte=x/16; //������16 ���ֽ��е���һ��
  x_byte=x&0x0f; //�����ڸ��ֽ��е���һλ
  y_Dyte=y/32; //0 Ϊ�ϰ�����1 Ϊ�°���
  y_byte=y&0x1f; //������0~31 ���е���һ��
  Write_Cmd(0x80+y_byte); //�趨�е�ַ(y ����),���Ǵ�ֱ��ַ
  Write_Cmd(0x80+x_Dyte+8*y_Dyte); //�趨�е�ַ(x ����)����ͨ��8*y_Dyte ѡ��������������ˮƽ��ַ
  read_data(); //Ԥ��ȡ����
  GDRAM_hbit=read_data(); //��ȡ��ǰ��ʾ��8 λ����
  GDRAM_lbit=read_data(); //��ȡ��ǰ��ʾ��8 λ����
//  Delay_Nms(1);
  Write_Cmd(0x80+y_byte); //�趨�е�ַ(y ����)
  Write_Cmd(0x80+x_Dyte+8*y_Dyte); //�趨�е�ַ(x ����)����ͨ��8*y_Dyte ѡ��������
//  Delay_Nms(1);
 if(x_byte<8) //�ж����ڸ�8 λ�������ڵ�8 λ
 {
   if(color==1)
   {
   Write_Data(GDRAM_hbit|(0x01<<(7-x_byte))); //��λGDRAM ����8 λ��������Ӧ�ĵ�
   }
   else
    Write_Data(GDRAM_hbit&(~(0x01<<(7-x_byte)))); //���GDRAM ����8 λ��������Ӧ�ĵ�

   Write_Data(GDRAM_lbit); //��ʾGDRAM ����8 λ����
 }
 else
  {
  Write_Data(GDRAM_hbit);         //д��8λ����
  if(color==1)
   Write_Data(GDRAM_lbit|(0x01<<(15-x_byte))); //��λGDRAM ����8 λ��������Ӧ�ĵ�
  else
   Write_Data(GDRAM_lbit&(~(0x01<<(15-x_byte))));//���GDRAM����8λ��������Ӧ�ĵ�
  }
  Write_Cmd(0x30); //�ָ�������ָ�
}
//********************************************************
//* ��    �ƣ�GUI_Fill_GDRAM()
//* ��    �ܣ���GDRAM���������lcd_data
//* ��ڲ�������dat Ϊ��������,ȡֵΪ0��1
//* ���ز�����void
//********************************************************
void GUI_Fill_GDRAM(unsigned char dat)
{
 uchar i,j,k;

 uchar bGDRAMAddrX = 0x80; //GDRAM ˮƽ��ַ
 uchar bGDRAMAddrY = 0x80; //GDRAM ��ֱ��ַ
 for(i=0;i<2;i++)
 {
 for(j=0;j<32;j++)
  {
  for(k=0;k<8;k++)
   {
    Write_Cmd(0x34); //����Ϊ8 λMPU �ӿڣ�����ָ�,��ͼģʽ��
    Write_Cmd(bGDRAMAddrY+j); //��ֱ��ַY
    Write_Cmd(bGDRAMAddrX+k); //ˮƽ��ַX
    Write_Data(dat*0xff);
    Write_Data(dat*0xff);
     }
   }
  bGDRAMAddrX = 0x88;
 }
  Write_Cmd(0x36); //�򿪻�ͼģʽ
  Write_Cmd(0x30); //�ָ�����ָ����رջ�ͼģʽ
}
//********************************************************************************
//* ��    �ƣ�clrgdram()
//* ��    �ܣ���GDRAM�е�����ȫ����Ϊ0
//* ��ڲ�����?
//* ���ز�����void
//********************************************************************************
void clrgdram()
{
	GUI_Fill_GDRAM(0);
}
//********************************************************************************
//* ��    �ƣ�clrddram()
//* ��    �ܣ�����
//* ��ڲ�����?
//* ���ز�����void
//********************************************************************************
void clrddram()
{
	 Write_Cmd(0x01);
//	 Write_Cmd(0x34);
//	 Write_Cmd(0x30);
}
//++++
//********************************************************
//
/*********************************************************/
/******************��ˮƽ��*********************/
void LCD_Draw_Line_x( uchar X0, uchar X1, uchar Y, uchar Color )
{
    uchar Temp ;
    if( X0 > X1 )
    {
        Temp = X1 ;
        X1 = X0 ;
        X0 = Temp ;
    }
    for( ; X0 <= X1 ; X0++ )
    GUI_Point( X0, Y, Color );
}
//*************����ֱ��  ��x��  �ӵ�y0�л�����y1��  ��ɫ color **************************/
void LCD_Draw_Line_Y( uchar X, uchar Y0, uchar Y1, uchar Color )
{
    uchar Temp ;
    if( Y0 > Y1 )
    {
        Temp = Y1 ;
        Y1 = Y0 ;
        Y0 = Temp ;
    }
    for(; Y0 <= Y1 ; Y0++)
    GUI_Point( X, Y0, Color);
}
//**************��б��(x,y)��(end x,end y)***********************/
void LCD_Draw_Line( uchar StartX, uchar StartY, uchar EndX, uchar EndY, uchar Color )
{
    int distance;      /*������Ļ��С�ı��������?(���Ϊint��)*/
    int delta_x, delta_y ;
    int incx, incy ;
do{
    delta_x = EndX - StartX ;
    delta_y = EndY - StartY ;
    if( delta_x > 0 )
    {
        incx = 1;
    }
    else if( delta_x == 0 )
    {
       incx = 0;
    }
    else
    {
        incx = -1 ;
    }
    if( delta_y > 0 )
    {
        incy = 1 ;
    }
    else if(delta_y == 0 )
    {
       incy = 0 ;
    }
    else
    {
        incy = -1 ;
    }
    delta_x = incx * delta_x ;
    delta_y = incy * delta_y ;

    if( delta_x > delta_y )
   {
     distance = delta_x ;
     StartX+=incx;
     StartY+=incy;
   }
   else
   {
     distance = delta_y;
     StartX+=incx;
     StartY+=incy;
   }
   GUI_Point( StartX, StartY, Color );
}while(distance!=0);
}
/*******************************************
//  sin����
********************************************/
void fsin2()
{
 	float x,y;
 	uchar x1,y1;
 	for(x=0;x<(4*3.14159);x+=0.1)
  	{
   	y=1*sin(3*x);
   	x1=10*x;
   	y1=31-(10*y+0.5);			  //��yֵ������������
   	GUI_Point(x1,y1,1);
  	}
}
/*******************************************
�������ƣ�Ini_Lcd
��    �ܣ���ʼ��Һ��ģ��
��    ������
����ֵ  ����
********************************************/
void Lcd12864_Init(void)
{
    LCD_CMDOut;    //Һ�����ƶ˿�����Ϊ���?
    Write_Cmd(0x30);   //����ָ�
    Write_Cmd(0x02);   // ��ַ��λ
	Write_Cmd(0x0c);   //������ʾ��,�α�ر�?
	Write_Cmd(0x01);   //������?
	Write_Cmd(0x06);   //�α�����
	Write_Cmd(0x80);   //�趨��ʾ����ʼ��ַ
	Write_Cmd(0x01);	//����
	GUI_Fill_GDRAM(0);//* ��    �ܣ���GDRAM�е�����ȫ����Ϊ0
}
/****************************************
//        ��Բ
*****************************************/
/*void GUI_Circle(uchar x0,uchar y0,uchar r,uchar color)
{
  char a,b;
  char di;
  if(r>31 ||r==0) return; //�������ˣ���Һ����ʾ�����Բ�뾶�?31
  a=0;
  b=r;
  di=3-2*r; //�ж��¸���λ�õı�־
  while(a<=b)
  {
   GUI_Point(x0-b,y0-a,color); //3
   GUI_Point(x0+b,y0-a,color); //0
   GUI_Point(x0-a,y0+b,color); //1
   GUI_Point(x0-b,y0-a,color); //7
   GUI_Point(x0-a,y0-b,color); //2
   GUI_Point(x0+b,y0+a,color); //4
   GUI_Point(x0+a,y0-b,color); //5
   GUI_Point(x0+a,y0+b,color); //6
   GUI_Point(x0-b,y0+a,color);
   a++;
   if(di<0) di+=4*a+6;
   else
   {
    di +=10+4*(a-b);
    b--;
   }
  GUI_Point(x0+a,y0+b,color);
  }
}
*/

void Picture_shows(unsigned char *picture) {
	uchar i,j,k;

	 uchar AddrX = 0x80; //GDRAM ˮƽ��ַ
	 uchar AddrY = 0x80; //GDRAM ��ֱ��ַ
	 for(i=0;i<2;i++)
	 {
	 for(j=0;j<32;j++)
	  {
	  for(k=0;k<8;k++)
	   {
	    Write_Cmd(0x34); //����Ϊ8 λMPU �ӿڣ�����ָ�,��ͼģʽ��
	    Write_Cmd(AddrY+j); //��ֱ��ַY
	    Write_Cmd(AddrX+k); //ˮƽ��ַX
	    Write_Data(picture[16*j+k*2]);
	    Write_Data(picture[16*j+k*2+1]);
	     }
	   }
	  AddrX = 0x88;
	 }
	  Write_Cmd(0x36); //�򿪻�ͼģʽ
	  Write_Cmd(0x30); //�ָ�����ָ����رջ�ͼģʽ
}

