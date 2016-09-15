/****************************************************************************  
 *  
 * Project: Interfacing Newhavens TFT LCD 2  
 *  
 ****************************************************************************  
 * Purpose: Project to Prepare a priliminary interface to the TFT LCD  
 *          with HX8347 controller with 16 Bit-Data Input (“BS2, BS1, BS0”=”000”)  
 *          65K Color  
 *          [NHD-2.4-240320YF-CTXI#-T-1 (2.4" Display with Touch Screen)]  
 ****************************************************************************  
 * FileName     : lcd2.c  
 * Dependencies :  
 * Config Bits  : In Global.h  
 * Prefetch     : On  
 * Freq-CPU     : 80MHz  
 * PBCLK        : 20MHz (1:4)  
 * Processor    : PIC32MX360F512L  
 ****************************************************************************  
 * Revision     :  
 * 0.0 - First Creation - 07,Sept,2008 Abhijit Bose(boseji@yahoo.com)  
 * 0.1 - Added enhanced routines 09-Nov-08  
 ****************************************************************************  
 * License      : GPL2  
 * Software License Agreement  
 *  
 * THIS SOFTWARE IS PROVIDED IN AN “AS IS” CONDITION. NO WARRANTIES,  
 * WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING, BUT NOT LIMITED  
 * TO, IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A  
 * PARTICULAR PURPOSE APPLY TO THIS SOFTWARE. THE COMPANY SHALL NOT,  
 * IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL OR  
 * CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.  
 *  
 ***************************************************************************/   
    
#include "Graphics\Graphics.h"   
#if !defined(b)   
#define b(X) (1<<X)   
#endif    
////////////////////////////////////////////////////////   
//Driver Internal Variables   
////////////////////////////////////////////////////////   
unsigned short cursor_pos_x=0,cursor_pos_y=0;   
unsigned char lcd_orient;   
#define LCD_RESET_OFF()     LATGSET=b(15)   
#define LCD_RESET_ON()      LATGCLR=b(15)   
#define LCD_CS_OFF()        LATDSET=b(15)   
#define LCD_CS_ON()         LATDCLR=b(15)   
#define LCD_DATA_ADDRS      1   
#define LCD_CMD_ADDRS       0   
#define LCD_CMD(X)          PMPSetAddress(LCD_CMD_ADDRS);PMPMasterWrite(X);   
#define LCD_DATA_ADDR()     PMPSetAddress(LCD_DATA_ADDRS);   
#define LCD_DATA(X)         PMPMasterWrite(X);   
#define LCD_DATA_READ()     PMPMasterRead()   
#define LCD_INIT_ALL()      lcd_io_config();lcd_init_seq()   
//Format Conversion Logic   
#define RGB565(RED,GREEN,BLUE)  ((((RED&0x03F)>>1)<<11)|((GREEN&0x03F)<<5)|((BLUE&0x03F)>>1))   
//Screen Size Definition   
//X axis   
#if !defined(SCREEN_HOR_SIZE)   
#define SCREEN_HOR_SIZE 320   
#endif   
//Y axis   
#if !defined(SCREEN_VER_SIZE)   
#define SCREEN_VER_SIZE 240   
#endif   
////////////////////////////////////////////////////////   
//Microchip Graphics API Variables   
////////////////////////////////////////////////////////   
// Color   
WORD_VAL    _color;   
// Clipping region control   
SHORT _clipRgn;   
// Clipping region borders   
SHORT _clipLeft;   
SHORT _clipTop;   
SHORT _clipRight;   
SHORT _clipBottom;   
////////////////////////////////////////////////////////   
//Internal Function Declrations   
////////////////////////////////////////////////////////   
//To write a Register in the HX8347   
void lcd_write_reg(unsigned short addr,unsigned short val);   
//To read a Register in the HX8347   
unsigned short lcd_read_reg(unsigned short addr);   
//To Enable the Data Write or Read in GRAM   
void lcd_enable_datawrite(void);   
//To Set the GRAM Address to 0   
void lcd_setGRAM_address_0();   
//To set the I/O config and the PMP   
void lcd_io_config();   
//To initialize the HX8347 Controller   
void lcd_init_seq();   
//To Draw some sort of Multicolored Display       
void rainbow(void);   
//Fill the Full display with one color and make the address 0,0   
void lcd_fulldisplay(unsigned short d1);   
//To Draw a Pixel on the Screen   
void putpixel(unsigned short x,unsigned short y,unsigned short color);   
//To Find out the color of a given location in the Screen   
unsigned short getpixel(unsigned short x,unsigned short y);   
   
   
/*********************************************************************  
* Function:  void  DelayMs(WORD time)  
*  
* PreCondition: none  
*  
* Input: time - delay in ms  
*  
* Output: none  
*  
* Side Effects: none  
*  
* Overview: delays execution on time specified in ms  
*  
* Note: delay is defined for 16MIPS  
*  
********************************************************************/   
#ifdef __PIC32MX   
void  DelayMs(WORD time)   
{   
    while(time--)   
    {   
        unsigned int int_status;   
   
        int_status = INTDisableInterrupts();   
        OpenCoreTimer(GetSystemClock() / 2000);     // core timer is at 1/2 system clock   
        INTRestoreInterrupts(int_status);   
   
        mCTClearIntFlag();   
   
        while(!mCTGetIntFlag());   
   
    }   
   
    mCTClearIntFlag();   
}   
#else   
#define DELAY_1MS 16000/5  // for 16MIPS   
void  DelayMs(WORD time){   
unsigned delay;   
    while(time--)   
        for(delay=0; delay<DELAY_1MS; delay++);     
}   
#endif   
   
////////////////////////////////////////////////////////   
//Internal Function Definitions   
////////////////////////////////////////////////////////   
//Generate Delay In milliseconds but not accurate   
 void lcddelayms(unsigned t)   
{   
    unsigned int i,j;   
    while(t--)   
    {   
        //t x 1mS loop   
         for(i=0;i< ((GetPeripheralClock()/10000)-1);i++);   
    }   
}//end lcddelayms    
   
//To write a Register in the HX8347   
void lcd_write_reg(unsigned short addr,unsigned short val)   
{   
    LCD_CMD(addr);LCD_DATA_ADDR();LCD_DATA(val);   
}//end lcd_write_reg   
   
//To read a Register in the HX8347   
unsigned short lcd_read_reg(unsigned short addr)   
{   
  LCD_CMD(addr);LCD_DATA_ADDR();   
  LCD_DATA_READ();   
  return LCD_DATA_READ();     
}//end lcd_read_reg   
   
//To Enable the Data Write or Read in GRAM   
void lcd_enable_datawrite(void)   
{   
    LCD_CMD(0x22);LCD_DATA_ADDR();   
}           
//To Set the GRAM Address to 0   
void lcd_setGRAM_address_0()   
{   
    //Make the Disply Full Screen   
    lcd_write_reg(0x02,0x0000);   
    lcd_write_reg(0x03,0x0000);   
    lcd_write_reg(0x04,0x0000);   
    lcd_write_reg(0x05,0x00EF);   
    lcd_write_reg(0x06,0x0000);   
    lcd_write_reg(0x07,0x0000);   
    lcd_write_reg(0x08,0x0001);   
    lcd_write_reg(0x09,0x003F);   
    lcd_enable_datawrite();    
}//end lcd_setGRAM_address_0        
   
//To set the I/O config and the PMP   
void lcd_io_config()   
{   
    /*Pin Config  
        RE0-RE7 -> LCD_D0-LCD_D7    (PMP)   Low  
        RG0     -> LCD_D8           (PMP)   Low  
        RG1     -> LCD_D9           (PMP)   Low  
        RF1     -> LCD_D10          (PMP)   Low  
        RF0     -> LCD_D11          (PMP)   Low  
        RD12    -> LCD_D12          (PMP)   Low  
        RD13    -> LCD_D13          (PMP)   Low  
        RD6     -> LCD_D14          (PMP)   Low  
        RD7     -> LCD_D15          (PMP)   Low  
          
        RB15    -> LCD_RS           (PMP)   Low [Config the Port Pin for Digital Operation]  
        RD4     -> LCD_WR           (PMP)   High  
        RD5     -> LCD_RD           (PMP)   High  
        RD11    -> LCD_CS           o/p     High  
          
        RG15    -> LCD_RESET        o/p     High         
    */   
    /*=========*/   
    /* IO Init */   
    /*=========*/   
    //Reset Init   
    LATGCLR=b(15);   
    TRISGCLR = b(15);    
       
    //Chip Select   
    LATDCLR =b(4)|b(5)|b(11);   
    TRISDCLR =b(4)|b(5)|b(11);   
       
    /*==========*/   
    /* PMP Init */   
    /*==========*/   
    mPMPClose(); //Close the Previously open module   
    mPMPOpen(PMP_ON|PMP_READ_WRITE_EN/*|PMP_CS2_CS1_EN*/,   
             PMP_DATA_BUS_16|PMP_MODE_MASTER2/*|PMP_WAIT_BEG_4|PMP_WAIT_MID_15|PMP_WAIT_END_4*/,   
             PMP_PEN_0,   
             PMP_INT_OFF);   
}//end lcd_io_config   
   
//To initialize the HX8347 Controller   
void lcd_init_seq()   
{   
    /*================*/   
    /* Reset Sequence */   
    /*================*/   
    //Engage Reset   
    lcddelayms(100);       
    //Release Reset   
    LATGSET=b(15);   
    lcddelayms(100);       
       
    /*==========================*/   
    /* INIT Commands for HX8347 */   
    /*==========================*/       
    lcd_write_reg(0x46,0x0094);   
    lcd_write_reg(0x47,0x0041);   
    lcd_write_reg(0x48,0x0000);   
    lcd_write_reg(0x49,0x0033);   
    lcd_write_reg(0x4A,0x0023);   
    lcd_write_reg(0x4B,0x0045);   
    lcd_write_reg(0x4C,0x0044);   
    lcd_write_reg(0x4D,0x0077);   
    lcd_write_reg(0x4E,0x0012);   
    lcd_write_reg(0x4F,0x00CC);   
       
    lcd_write_reg(0x50,0x0046);   
    lcd_write_reg(0x51,0x0082);   
       
    lcd_write_reg(0x02,0x0000);   
    lcd_write_reg(0x03,0x0000);   
    lcd_write_reg(0x04,0x0000);   
    lcd_write_reg(0x05,0x00EF);   
    lcd_write_reg(0x06,0x0000);   
    lcd_write_reg(0x07,0x0000);   
    lcd_write_reg(0x08,0x0001);   
    lcd_write_reg(0x09,0x003F);   
       
    lcd_write_reg(0x01,0x0006);   
    lcd_write_reg(0x16,0x0048);//For RGB 0x-0  Write Data: [15-11]-blue(MSB copy) [10-5]-green [4-0]-red(MSB copy)   
                               //              Read Data: dummy [7-2]green [7-2]blue [7-2]red (8bit mode)   
                               //For BGR 0x-8  Write Data: [15-11]-red(MSB copy) [10-5]-green [4-0]-blue(MSB copy)   
                               // (Followed)   Read Data: dummy [7-2]green [7-2]red [7-2]blue (8bit mode)   
    lcd_write_reg(0x23,0x0095);   
    lcd_write_reg(0x24,0x0095);   
    lcd_write_reg(0x25,0x00FF);   
    lcd_write_reg(0x27,0x0002);   
    lcd_write_reg(0x28,0x0002);   
    lcd_write_reg(0x29,0x0002);   
    lcd_write_reg(0x2A,0x0002);   
    lcd_write_reg(0x2C,0x0002);   
    lcd_write_reg(0x2D,0x0002);   
       
    lcd_write_reg(0x3A,0x0001);   
    lcd_write_reg(0x3B,0x0001);   
    lcd_write_reg(0x3C,0x00F0);   
    lcd_write_reg(0x3D,0x0000);   
    lcddelayms(80);   
    lcd_write_reg(0x35,0x0038);   
    lcd_write_reg(0x36,0x0078);   
    lcd_write_reg(0x3E,0x0038);   
    lcd_write_reg(0x40,0x000F);   
    lcd_write_reg(0x41,0x00F0);   
       
    lcd_write_reg(0x19,0x002D);   
    lcd_write_reg(0x93,0x0006);   
    lcddelayms(80);   
    lcd_write_reg(0x20,0x0040);   
    lcd_write_reg(0x1D,0x0007);   
    lcd_write_reg(0x1E,0x0000);   
    lcd_write_reg(0x1F,0x0004);   
       
    lcd_write_reg(0x44,0x003C);   
    lcd_write_reg(0x45,0x0012);   
    lcddelayms(80);   
    lcd_write_reg(0x1C,0x0004);   
    lcddelayms(80);   
    lcd_write_reg(0x43,0x0080);   
    lcddelayms(80);   
    lcd_write_reg(0x1B,0x0008);   
    lcddelayms(80);   
    lcd_write_reg(0x1B,0x0010);   
    lcddelayms(80);   
    lcd_write_reg(0x90,0x007F);   
    lcd_write_reg(0x26,0x0004);   
    lcddelayms(80);   
    lcd_write_reg(0x26,0x0024);   
    lcd_write_reg(0x26,0x002C);   
    lcddelayms(80);   
    lcd_write_reg(0x26,0x003C);   
    lcd_write_reg(0x57,0x0002);   
    lcd_write_reg(0x55,0x0000);   
    lcd_write_reg(0x57,0x0000);   
       
    lcd_enable_datawrite();   
}//end lcd_init_seq   
   
//To Draw some sort of Multicolored Display       
void rainbow(void)   
{   
    unsigned short i,j;   
    unsigned short r,g,b;   
    unsigned short k;   
   
    //write_GRAM_address_0();       //Set GRAM start address 0   
   
    r = 31;   
    g = 0;   
    b = 0;   
    for(j=0;j<320;j++)   
    {   
        k = b + (g << 6) + (r << 11);   
           
        if(j<20)   
        {   
            r = 31;   
            g = 0;   
            b = 0;   
        }   
        else    
        {   
            if(j<51)   
            {   
                r -= 1;   
                g += 1;   
            }   
            else    
            {   
                if(j < 90)   
                {   
           
                    r = 0;   
                    g = 31;   
                    b = 0;   
           
                }   
                else    
                {   
                    if(j < 121)   
                    {   
                        r = 0;   
                        g -= 1;   
                        b += 1;   
                    }   
               
                    else    
                    {   
                        if(j < 160)   
                        {   
                            r = 0;   
                            g = 0;   
                            b = 31;   
                        }   
                   
                        else    
                        {      
                            if(j < 191)   
                            {   
                                r += 1;   
                                g = 0;   
                                b -= 1;   
                            }   
                       
                            else    
                            {   
                                if(j < 320)   
                                {   
                                    r = 31;   
                                    g = 0;   
                                    b = 0;   
                                }   
                            }   
                        }          
                    }              
                }              
            }          
        }           
           
        for(i=0;i<240;i++)   
        {               
            LCD_DATA(k);   
        }   
    }       
}//end rainbow   
   
//Fill the Full display with one color and make the address 0,0   
void lcd_fulldisplay(unsigned short d1)   
{   
    unsigned short i,j;   
           
    lcd_setGRAM_address_0();   
       
    for(i=0;i<320;i++)   
    {   
       for(j=0;j<240;j++)   
          {   
             LCD_DATA(d1);   
          }   
    }   
}//end lcd_fulldisplay   
   
//To Draw a Pixel on the Screen   
void putpixel(unsigned short x,unsigned short y,unsigned short color)   
{   
    lcd_write_reg(0x02,(y>>8));     //High Column Start   
    lcd_write_reg(0x03,(y&0x0FF));  //Low Column Start   
       
    lcd_write_reg(0x06,(x>>8));     //High Row Start   
    lcd_write_reg(0x07,(x&0x0FF));  //Low Row Start   
       
    cursor_pos_x = x;cursor_pos_y = y;   
       
    lcd_enable_datawrite();    
       
    LCD_DATA(color); //Draw the Pixel   
}//end putpixel   
   
//To Find out the color of a given location in the Screen   
unsigned short getpixel(unsigned short x,unsigned short y)   
{   
    unsigned short r,g,b;   
    lcd_write_reg(0x02,(y>>8));     //High Column Start   
    lcd_write_reg(0x03,(y&0x0FF));  //Low Column Start   
       
    lcd_write_reg(0x06,(x>>8));     //High Row Start   
    lcd_write_reg(0x07,(x&0x0FF));  //Low Row Start   
       
    cursor_pos_x = x;cursor_pos_y = y;   
       
    lcd_enable_datawrite();   
    
    LCD_DATA_READ();   
    LCD_DATA_READ();//Dummy Read   
       
    //Color pattern As per 16H -> 0xC8 Write RGB Read GRB(8bit comm mode)   
    LCD_DATA_READ(); //Green   
    g=LCD_DATA_READ()>>2;   
       
    LCD_DATA_READ(); //Red   
    r=LCD_DATA_READ()>>2;   
       
    LCD_DATA_READ(); //Blue   
    b=LCD_DATA_READ()>>2;   
       
    return RGB565(r,g,b);    
}//end getpixel   

////////////////////////////////////////////////////////////////////////////////////////////////   
/// Microchip Graphics API   
////////////////////////////////////////////////////////////////////////////////////////////////   
//This function clears the screen with the current color and    
//sets the graphic cursor position to (0, 0). Clipping is NOT supported by ClearDevice().   

void ClearDevice(void)   
{   
  lcd_fulldisplay(_color.Val);   
}//end ClearDevice   
   
//Initializes the display device.    
void ResetDevice()   
{   
lcd_io_config();   
lcd_init_seq();   
}//ResetDevice   
   
//Puts pixel with the given x,y coordinate position.   
void PutPixel(SHORT x,SHORT y)   
{   
    if(_clipRgn){   
            if(x<_clipLeft)   
                return;   
            if(x>_clipRight)   
                return;   
            if(y<_clipTop)   
                return;   
            if(y>_clipBottom)   
                return;   
        }   
    putpixel(x,y,_color.Val);   
}//end PutPixel   

//Gets the Pixel Color at the given X,Y Location   
WORD GetPixel(SHORT x, SHORT y)   
{   
    return (WORD)getpixel(x,y);   
}//end GetPixel   
//Function to Reach the origin Point again   
void MoveToOrigin(){ lcd_setGRAM_address_0();}//end MoveToOrigin   
   
   
   