/*-----------------------  
Test mode:8 bit paraller interface 8080 MCU  
Program by:Fuwei  
Date:2006-04-18  
-------------------------*/   
#include <reg51.h>   
#include <intrins.h>   
#include <ctype.h>   

#define X_add 98   
#define Y_add 70   

//#define Data_port P1   

#define  nop _nop_()   

#define R 0   
#define G 1   
#define B 2   

/*sbit CS=P3^1;//31  
sbit RST=P3^2;//32  
sbit SDA=P1^6;//16  
sbit SCLK=P1^5;//15*/   

sbit CS=P3^1;//31   
sbit RST=P3^2;//32   
sbit SDA=P1^6;//16   
sbit SCLK=P1^5;//15   

//sbit RD_=P3^2;   

sbit KEY=P3^3;   
//sbit INC=P3^6;   
//sbit DEC=P3^7;   
//sbit OTP=P1^7;   
typedef unsigned char byte;   
typedef unsigned int word;   
byte code djn[];   

/*byte code gray_scal[][8][2]={  
                          {{0xf8,0x00},{0xf9,0xaa},{0xfa,0xaa},{0xfb,0x6d},{0xfc,0x92},{0xfd,0xb6},{0xfe,0xdb},{0xff,0xff}},  
                          {{0x07,0xe0},{0x27,0xe4},{0x4f,0xe9},{0x6f,0xed},{0x97,0xf2},{0xb7,0xf6},{0xdf,0xfb},{0xff,0xff}},  
                          {{0x00,0x1f},{0x21,0x3f},{0x4a,0x5f},{0x6b,0x7f},{0x94,0x9f},{0xb5,0xbf},{0xe7,0x1f},{0xff,0xff}},  
                          };*/   

void write_data(byte in_data);   
void write_com(byte in_com);   
//byte read(void);   
void delay(word time);   
void initial();   
//void clear();   
void wait_key();   
void display_griphic(byte *griphic);   
void test_patten(byte cl,byte ch);   
void test_grayscal(byte color);   
void display_char();   
//void address_set(byte addX1,addX2,addY1,addY2);   
//void EEPROM_program();   

void main(void)   
     {   
    CS=0;   
    RST=0;   
    delay(20);   
    RST=1;   
    initial();   
     while(1)   
     {   
    // initial();   
    display_griphic(djn);    
    wait_key();   
      test_patten(0xf8,0x00);   
     wait_key();   
     test_patten(0x07,0xe0);   
     wait_key();      
     test_patten(0x00,0x1f);   
     wait_key();   
    // test_patten(0xff,0xff);   
    // wait_key();   
 //test_patten(0x00,0x00);   
   //  wait_key();   
  // initial();   
   //  test_grayscal(R);wait_key();   
  //   test_grayscal(G);wait_key();   
  //   test_grayscal(B);wait_key();   
 //initial();   
     }   
    }   

void delay(word time)   
     {   
    word i;   
    while(time--)   
         for(i=0;i<10;i++);   
    }   
void wait_key()   
     {   
    //KEY=1;   
     //while(KEY);   
     delay(5000);   
     //while(!KEY);   
     //delay(20);   
     //delay(8000);   
    }   
   

void write_data(byte indata)   
     {   
     byte i;   
     //CS=0;   
     //CS=1;   
     SDA=1;   
     SCLK=0;   
     SCLK=1;   
     for(i=0;i<8;i++)    /*for serial port*/   
         {   
         SDA=((bit)(indata&0x80));   
         indata<<=1;   
         SCLK=0;   
         SCLK=1;   
            }   
  // CS=1;   
     }   
   
void write_com(byte in_com)   
    {   
     byte i;   
   // CS=0;   
     SDA=0;   
     SCLK=0;   
     SCLK=1;   
     for(i=0;i<8;i++)    /*for serial port*/   
         {   
         SDA=((bit)(in_com&0x80));   
         in_com<<=1;   
         SCLK=0;   
         SCLK=1;   
            }   
 // CS=1;   
    }   
void initial()   
     {   
   
   // WR_=1;RD_=1;   
    write_com(0x01);//reset   
    delay(5);   

    write_com(0x25);//Contrast set   
    write_data(0x39);//39   

    write_com(0x11);//Sleep out   
    write_com(0x13);   
    write_com(0x20);//INV off   
    write_com(0x38);//IDM off   
    write_com(0x36);//MAD set   
    write_data(0x80);   
   // write_com(0x3a);//Interface format   
    //write_data(0x05);   
    //write_com(0x2d);//RGB set   
    //write_data(0x00);   
  //  write_com(0x2a);//Col add   
   // write_data(0x00);   
  //  write_data(0x61);   
  //  write_com(0x2b);//PAGE add   
  //  write_data(0x00);   
   // write_data(0x45);   
  //  write_com(0x2c);   
    write_com(0x29);//display ON   
       
    //delay(20);   
   
    }   

void display_griphic(byte *griphic)   
     {   
    word i;   
   write_com(0x2a);//Col add   
   write_data(0x00);   
    write_data(0x62);   
   
   write_com(0x2c);   
    for(i=0;i<13860;i++)   
      // for(j=0;j<99;j++)   
         // for(k=0;k<2;k++)   
          // {   
           write_data(*griphic++);   
           //write_data(*griphic++);   
           //griphic++;   
          //delay(200);   
          //}   
    }   

void test_patten(byte cl,byte ch)   
    {   
    word i,j;   
     write_com(0x2a);//Col add   
   write_data(0x00);   
   write_data(0x61);   
   // address_set(0x10,0x00,0x72,0x60);   
    write_com(0x2c);   
    for(i=0;i<Y_add;i++)   
       for(j=0;j<X_add;j++)   
         {   
          //for(k=0;k<2;k++)   
          write_data(cl);   
          write_data(ch);   
         }   
         }   
/*void test_grayscal(byte color)  
    {  
     word i,j,k,l;  
    // address_set(0x10,0x00,0x72,0x60);  
       write_com(0x2a);//Col add  
   write_data(0x00);  
    write_data(0x67);  
     write_com(0x2c);  
     for(i=0;i<Y_add;i++)  
        for(l=0;l<8;l++)  
           for(j=0;j<13;j++)  
  
               for(k=0;k<2;k++)  
                   write_data(gray_scal[color][l][k]);  
    }*/   

/*
void EEPROM_program()  
//  }  
    {  
    word con_temp;  
    RST=0;  
    delay(10);  
    RST=1;  
    con_temp=0;   
    DEC=1;INC=1;  
    while(OTP)  
    {  
        if(INC)  
        {     
        con_temp++;  
        write_com(0x25);  
        write_data(0x39+con_temp);  
            }  
    }  
    //write_com(0x25);//Contrast set  
   // write_data(0x39);//39  
    write_com(0xbb);  
    write_data(con_temp);  
    write_data(0x00);  
    write_com(0xd9);  
    write_data(0x00);  
    delay(10);  
    write_com(0x10);  
    delay(1000);  
    write_com(0xd1);  
     delay(5000);  
     delay(5000);  
     delay(5000);  
     delay(5000);  
     delay(5000);  
     delay(5000);  
     delay(5000);  
     delay(5000);  
    write_com(0xd0);  
   delay(1000);  
  
    }*/   
