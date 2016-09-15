ILI9481?????:
void ili9481_init(void) 
{  
          write_cmd(0x11); 
          delay_ms(50); 
          write_cmd(0x13); 

           write_cmd(0xD0);      
          write_dat(0x07);    
          write_dat(0x40);    
          write_dat(0x11);    

          write_cmd(0xD1); 
          write_dat(0x00);    
          write_dat(0x18);    
          write_dat(0x1D);    

          write_cmd(0xD2); 
          write_dat(0x01);    
          write_dat(0x11);    

          write_cmd(0xC0); 
          write_dat(0x00); 
          write_dat(0x3B); 
          write_dat(0x00); 
          write_dat(0x02); 
          write_dat(0x11); 
  
          write_cmd(0xC1);
          write_dat(0x10);
          write_dat(0x0B);
          write_dat(0x88);

        write_cmd(0xC3);
        write_dat((1<<4)|0);
        write_dat(0x20);
        write_dat(0x08);
        
          write_cmd(0xC5);
          write_dat(0x01);
        
          write_cmd(0xC6);       
          write_dat((1<<4)|(1<<3)|(1<<1)|(1<<0)); 
    
        write_cmd(0xC8);       
        write_dat(0x00);     
        write_dat(0x30);     
        write_dat(0x36);     
        write_dat(0x45);    
        write_dat(0x04);     
        write_dat(0x16);     
        write_dat(0x37);     
        write_dat(0x75);     
        write_dat(0x77);     
        write_dat(0x54);     
        write_dat(0x0F);     
        write_dat(0x00);

          write_cmd(0xB4);
          write_dat(0x11);        //DPI
          delay_ms(50); 

          write_cmd(0x36); 
          write_dat((0<<7)|(0<<6)|(1<<5)|(1<<3)|(0<<2)|(0<<1)|(0<<0));
// write_dat(0x002b); 
   
          write_cmd(0x3A); //262K
          write_dat(0x66); 
           
  
          write_cmd(0x2A); 
          write_dat(0x00); 
          write_dat(0x00); 
          write_dat(LCD_XSIZE>>8); 
          write_dat(LCD_XSIZE); 

          write_cmd(0x2B); 
          write_dat(0x00); 
          write_dat(0x00); 
          write_dat(LCD_YSIZE>>8); 
          write_dat(LCD_YSIZE); 

          delay_ms(50); 
          write_cmd(0x29); 
          write_cmd(0x2C); 
  
}




S3C6410 LCD?????:
static        int        lcd_init(void)
{

#if 1

// s3c2450 AT070TN 7.0"    (800x480)
/*
#define VBPD                    (23)        
#define VFPD                    (23)        
#define VSPW                    (23)

#define HBPD                    (46)
#define HFPD                    (46)        
#define HSPW                    (46)        
*/

#define VBPD                    (20)        
#define VFPD                    (20)        
#define VSPW                    (20)

#define HBPD                    (40)
#define HFPD                    (40)        
#define HSPW                    (40)        


#define FRAME_RATE        30

#define LCD_XSIZE_TFT     (LCD_XSIZE)   
#define LCD_YSIZE_TFT     (LCD_YSIZE)
#define LCD_SCR_XSIZE     (LCD_XSIZE)           // virtual screen  
#define LCD_SCR_YSIZE     (LCD_YSIZE)
#define LCD_SCR_XSIZE_TFT (LCD_XSIZE)           // virtual screen  
#define LCD_SCR_YSIZE_TFT (LCD_YSIZE)



//* VIDCON0        

#define        VIDOUT                (0)
#define        L1_DATA16        (4)
#define        L0_DATA16        (4)
#define        PNRMODE                (0)        
#define        CLKVALUP        (1)
#define        CLKVAL_F        (6)
#define        CLKDIR                (1)
#define        CLKSEL_F        (0)


//* VIDCON1
#define VCLK_INVERTED                        (0)
#define HSYNC_INVERTED                        (0)
#define VSYNC_INVERTED                        (0)
#define VDEN_INVERTED                        (0)


//* WINCON1to4
#if(LCD_BPP==16)
#define BITSWP                (0)        //shift on basis of half-word swap
#define BYTSWP                (0)        //shift on basis of half-word swap
#define HAWSWP                (1)        //shift on basis of half-word swap
#endif

#if(LCD_BPP==24)
#define BITSWP                (0)        //shift on basis of half-word swap
#define BYTSWP                (0)        //shift on basis of half-word swap
#define HAWSWP                (0)        //shift on basis of half-word swap
#endif

#define WINCONx_4WORD_BURST                        (2)
#define WINCONx_8WORD_BURST                        (1)
#define WINCONx_16WORD_BURST                (0)
#define WINCONx_PLANE_BLENDING                (0)
#define WINCONx_PIXEL_BLENDING                (1)
#define WINCONx_1BPP_PALLET                        (0)
#define WINCONx_2BPP_PALLET                        (1)
#define WINCONx_4BPP_PALLET                        (2)
#define WINCONx_8BPP_PALLET                        (3)
#define WINCONx_8BPP_NO_PALLET                (4)
#define WINCONx_16BPP_565                        (5)
#define WINCONx_16BPP_A555                        (6)
#define WINCONx_16BPP_1555                        (7)
#define WINCONx_18BPP_666                        (8)
#define WINCONx_18BPP_A665                        (9)
#define WINCONx_19BPP_A666                        (10)
#define WINCONx_24BPP_888                        (11)
#define WINCONx_24BPP_A887                        (12)
#define WINCONx_25BPP_A888                        (13)
#define WINCONx_ALPHA_MODE_0                (0)
#define WINCONx_ALPHA_MODE_1                (1)






//#define    LCD_HOZVAL_TFT          (LCD_YSIZE_TFT-1)
//#define    LCD_LINEVAL_TFT         (LCD_XSIZE_TFT-1)
        int x,y,m,n;
        U8 pagewidth_in_byte=0,offsize_in_byte=0;        
        U8 clkval=0;
        U16 hsync_cnt,vclk_cnt;
        U16 lcd_horizon_value,lcd_line_value;
        U8 lcd_vbpd,lcd_vfpd,lcd_vspw,lcd_hbpd,lcd_hfpd,lcd_hspw;
        U8 lcd_frame_rate;        
        int i;




        //// LCD Port
        rGPICON   = 0xAAAAAAAA;
        rGPIPUD   = 0xAAAAAAAA;
        
        rGPJCON   = 0xAAAAAAAA;
        rGPJPUD   = 0xAAAAAAAA;
        
        
        rMIFPCON        &=~(1<<3);
        
        rSPCON                &=~(3<<0);
        rSPCON                |=(1<<0);
        

        lcd_horizon_value = LCD_XSIZE;
        lcd_line_value = LCD_YSIZE;
        lcd_vbpd = VBPD;
        lcd_vfpd = VFPD;
        lcd_vspw = VSPW;
        lcd_hbpd = HBPD;
        lcd_hfpd = HFPD;
        lcd_hspw = HSPW;
        lcd_frame_rate = FRAME_RATE;



        pagewidth_in_byte = lcd_horizon_value/8*16;                
        offsize_in_byte = 0;
        
        hsync_cnt         = (lcd_vbpd+lcd_vfpd+lcd_vspw+lcd_line_value);
        vclk_cnt          = (lcd_hbpd+lcd_hfpd+lcd_hspw+lcd_horizon_value);
        //clkval = (U8)(((float)CPU_GetHclkHZ()/(float)(hsync_cnt*vclk_cnt*lcd_frame_rate*2))+0.5)-1;        
        //clkval                = 7;        
        clkval                = 8;        

        
        rVIDCON0 &= ~(3<<0); // ENVID Off using Per Frame method
        rWINCON0 &= ~(1<<0);

        rVIDCON0 = (VIDOUT<<26)|(L1_DATA16<<23)|(L0_DATA16<<20)|(PNRMODE<<17)|(CLKVALUP<<16)|(clkval<<6)|(CLKDIR<<4)|(CLKSEL_F<<2)|(0<<0);
  
        
        rVIDCON1 = (VCLK_INVERTED<<7)|(HSYNC_INVERTED<<6)|(VSYNC_INVERTED<<5)|(VDEN_INVERTED<<4);

        rVIDTCON0 = ((lcd_vbpd-1)<<16)|((lcd_vfpd-1)<<8)|(lcd_vspw-1);
        rVIDTCON1 = ((lcd_hbpd-1)<<16)|((lcd_hfpd-1)<<8)|(lcd_hspw-1);
        rVIDTCON2 = ((lcd_line_value-1)<<11)|(lcd_horizon_value-1);

        #if(LCD_BPP==16)
        rWINCON0 = (BITSWP<<18)|(BYTSWP<<17)|(HAWSWP<<16)|(WINCONx_8WORD_BURST<<9)|(WINCONx_16BPP_565<<2); // 16word burst, 16bpp, 
        #endif
        
        #if(LCD_BPP==24)
        rWINCON0 = (BITSWP<<18)|(BYTSWP<<17)|(HAWSWP<<16)|(WINCONx_8WORD_BURST<<9)|(WINCONx_24BPP_888<<2); // 16word burst, 24bpp, 
        #endif
        
        rVIDOSD0A = (0<<11)|(0);
        rVIDOSD0B = (lcd_horizon_value-1)<<11|(lcd_line_value-1);        
        
        rVIDW00ADD0B0 = (U32)LCD_Buffer;                
        rVIDW00ADD1B0 = (U32)LCD_Buffer + LCD_BUF_SIZE;
        
        rVIDW00ADD2 = (offsize_in_byte<<13)|(LCD_XSIZE*sizeof(LCD_COLOR));
        
        //rDITHMODE = (1<<5)|(1<<3)|(1<<1)|(1<<0);
        //rDITHMODE = 0;
        
        rVIDCON0 |= (3<<0);
        rWINCON0 |= (1<<0);
        
//?        rINTSUBMSK |= (0xf << IRQ_SUB_LCD1);        // MASK all LCD Sub Interrupt
        
        DebugPrintf("LCDInit End(%dx%d,%dbpp)\n",LCD_XSIZE,LCD_YSIZE,LCD_BPP);

#endif

}

