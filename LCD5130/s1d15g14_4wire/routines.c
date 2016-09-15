// maglagay ng "dot" na color "pigment" sa pixel na located sa position "x"-"y"
void dot(unsigned char pigment, unsigned char x, unsigned char y)
{
     
   send(0x2B, CMD);      // page address set
   send(y, DATA);
   nokia_cs();

   send(0x2A, CMD);      // column address set
   send(x, DATA);
   nokia_cs();

   send(0x2C, CMD);      // Memory Write
   send(pigment, DATA);
   
   nokia_cs();
}

// i-erase yung pixel (gawing kapareho ng color ng background)
void erase(unsigned char x, unsigned char y)
{
     
   send(0x2B, CMD);      // page address set
   send(y, DATA);
   nokia_cs();

   send(0x2A, CMD);      // column address set
   send(x, DATA);
   nokia_cs();

   send(0x2C, CMD);      // Memory Write
   send(bgcolor, DATA);
   
   nokia_cs();
}

// lagyan ng plain color yung block dectitated by coordinates x1, x2, y1, & y2
void color_fill(unsigned char fondo, unsigned char x1, unsigned char x2, unsigned char y1, unsigned char y2)
{
   unsigned int16 i, j;
   j=((unsigned long)(x2-x1+1)*(y2-y1+1));
   
   send(0x2A, CMD);           // column address set
   send(x1, DATA);
   send(x2, DATA);           // Each pixel 97 passes to the next column
   nokia_cs();

   send(0x2B, CMD);           // page address set
   send(y1, DATA);
   send(y2, DATA);           // Each pixel 66 passes to the next page
   nokia_cs();
   
   send(0x2C, CMD);           // Memory Write
   for(i=0; i<j; i++)
   {
   nokia_senddata(fondo);             // clear screen
   }
   nokia_cs();

}
