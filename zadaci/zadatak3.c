// primer rada sa PCF8563 koji ispisuje tekuce vreme 
// u terminalu svakih 5 sekundi 
#include <wiringPi.h>
#include <wiringPiI2C.h>
#include <stdio.h>
#include <unistd.h>  
#include <lcd.h>
#include <errno.h>

//za rad sa LCDom
const int RS = 3;
const int EN = 14;
const int D0 = 4;
const int D1 = 12;
const int D2 = 13;
const int D3 = 6;


#define changeHexToInt(hex)((((hex)>>4) *10 ) + ((hex)%16) )  

//adrese registara 

#define SEK    0x02 
#define MIN    0x03  
#define SAT    0x04 
#define DAY    0x05  
#define MON    0x07
#define YRS    0x08      

unsigned char WriteBuf[2];    
unsigned char ReadBuf;  
unsigned char g8563_Store[6];   

// sec,min,sat  

unsigned char init8563_Store[5]={0x00,0x59,0x08,0x31,0x03,0x23}; // inicijalizacija pocetnog vremena

void P8563_settime() 
{      
    WriteBuf[0] = SEK;      
    WriteBuf[1] = g8563_Store[0]; 
    wiringPiI2CWriteReg8(fd,SEK,g8563_Store[0]);  

    WriteBuf[0] = MIN;  
    WriteBuf[1] = g8563_Store[1];      
    wiringPiI2CWriteReg8(fd,MIN,g8563_Store[1]);

    WriteBuf[0] = SAT;  
    WriteBuf[1] = g8563_Store[2];      
    wiringPiI2CWriteReg8(fd,SAT,g8563_Store[2]);

    WriteBuf[0] = DAY;  
    WriteBuf[1] = g8563_Store[3];      
    wiringPiI2CWriteReg8(fd,DAY,g8563_Store[3]);

    WriteBuf[0] = MON;  
    WriteBuf[1] = g8563_Store[4];      
    wiringPiI2CWriteReg8(fd,MON,g8563_Store[4]);

    WriteBuf[0] = YRS;  
    WriteBuf[1] = g8563_Store[5];      
    wiringPiI2CWriteReg8(fd,YRS,g8563_Store[5]);} 

void P8563_init() 
{     
    unsigned char i;      
    for(i=0;i<=5;i++)          
        g8563_Store[i]=init8563_Store[i];             
    P8563_settime();     
    printf("Postavi početno tekuće vreme\n");           
    
    //inicijalizacija RTC-a 
                        //WriteBuf[0] = 0x0;  
                        //WriteBuf[1] = 0x00;   
    wiringPiI2CWriteReg8(fd, 0x0, 0x00); 
} 

void P8563_Readtime() 
{       
    unsigned char time[7];       
    WriteBuf[0] = SEK;      
    wiringPiI2CWrite(fd, WriteBuf,1);    
    wiringPiI2CReadReg8(fd, SEK);
    wiringPiI2CReadReg8(fd, MIN);
    wiringPiI2CReadReg8(fd, SAT);
    wiringPiI2CReadReg8(fd, DAY);
    wiringPiI2CReadReg8(fd, MON);
    wiringPiI2CReadReg8(fd, YRS);


    g8563_Store[0] = time[0] & 0x7f;     //sekunde
    g8563_Store[1] = time[1] & 0x7f;     //minuti
    g8563_Store[2] = time[2] & 0x3f;     //sati
    g8563_Store[2] = time[3] & 0x3f;     //dani
    g8563_Store[2] = time[4] & 0x1f;     //meseci
    g8563_Store[2] = time[5] & 0xff;     //godine

    g8563_Store[0] = changeHexToInt(g8563_Store[0]);    
    g8563_Store[1] = changeHexToInt(g8563_Store[1]);    
    g8563_Store[2] = changeHexToInt(g8563_Store[2]); 
    g8563_Store[3] = changeHexToInt(g8563_Store[3]);    
    g8563_Store[4] = changeHexToInt(g8563_Store[4]);    
    g8563_Store[5] = changeHexToInt(g8563_Store[5]); 
}

int main(int argc, char **argv)  
{  
    int lcd_h;
    wiringPiSetup();
    
    int fd = wiringPiI2CSetup(0x51);
    if ( fd == -1 )
    {
        printf("Nije uspelo!!!\n");
        return -1;
    }
    
    lcd_h = lcdInit(2,16,4,RS,EN,D0,D1,D2,D3,D0,D1,D2,D3);
           
    while(1)       
    {         
        P8563_Readtime();
        lcdPosition(lcd_h,0,1);
        lcdPrintf(lcd_h,"Sati:%d Minuti:%d Sekunde:%d\n", g8563_Store[2],g8563_Store[1], g8563_Store[0]);
	    lcdPosition(lcd_h, 0,0);
        lcdPrintf(lcd_h,"Datum:%d/%d/%d\n", g8563_Store[3],g8563_Store[4], g8563_Store[5]);       
        delay(5000);
        lcdClear(lcd_h);
    }   

    return 0;  
}