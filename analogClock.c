
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <math.h>
//Glcd display connections
char GLCD_Dataport at PORTD;             //DATA pins connection
sbit GLCD_CS1 at LATA0_bit;              //CS1 ---->  RA0
sbit GLCD_CS2 at LATA1_bit;              //CS2 ---->  RA1
sbit GLCD_RS  at LATA4_bit;              //RS  ---->  RA4
sbit GLCD_RW  at LATB3_bit;              //RW ---->   RB3
sbit GLCD_EN  at LATB4_bit;              //EN  ---->  RB4
sbit GLCD_RST at LATB5_bit;              //RST ---->  RB5

sbit GLCD_CS1_Direction at TRISA0_bit;
sbit GLCD_CS2_Direction at TRISA1_bit;
sbit GLCD_RS_Direction  at TRISA4_bit;
sbit GLCD_RW_Direction  at TRISB3_bit;
sbit GLCD_EN_Direction  at TRISB4_bit;
sbit GLCD_RST_Direction at TRISB5_bit;



unsigned int millisecond = 0;
unsigned int timestamp = 0,oldSecond=0,oldHour=0,oldMinute=0;

unsigned int adjusmentIndex=0,oneSecActive=0,oneHourActive=0;
unsigned char *userHourTxt="00",*userMinuteTxt="00",initHour=0,initMinute=0,initSecond=0;


void timerOperations(void){
//1ms timer
    T0CON = 0x88;       //No prescale 16-bit enabled timer
    TMR0H = 0xFC;
    TMR0L = 0x17;
    INTCON.GIE=1;
    INTCON.TMR0IE=1;
    INTCON.TMR0IF=0;
}

void drawClockInterface(void){
      int tick=0;
      int xPos1,xPos2,yPos1,yPos2;
      Glcd_Circle(96,32,30,1);
      Glcd_Circle(96,32,2,1);
      for( tick=0; tick<360; tick=tick+30 ){
           float angle=tick;
           angle = (angle/57.29577951) ;         //Convert degrees to radians
           xPos1 = (96 + (sin(angle)*30));
           yPos1 = (32 - (cos(angle)*30));
           xPos2 = (96 + (sin(angle)*(30-5)));
           yPos2 = (32 - (cos(angle)*(30-5)));
           Glcd_Line(xPos1, yPos1, xPos2, yPos2, 1);
      
      }
}

void clockComponents(int hour, int minute,int second){
    float angle=0.0;
    unsigned int temp=0,xEndPoint,yEndPoint;

    //for hour
    angle=hour*30+((minute*6)/12);
    angle=(angle/57.29577951) ;             //Convert degrees to radians
    xEndPoint = ( 96 + (sin(angle)*(15)) );
    yEndPoint = ( 32 - (cos(angle)*(15)) );
    Glcd_Line(96, 32, xEndPoint, yEndPoint, 1);
    //for minute
    temp=(minute*6);
    angle=(temp/57.29577951) ;             //Convert degrees to radians
    xEndPoint = ( 96 + (sin(angle)*(20)) );
    yEndPoint = ( 32 - (cos(angle)*(20)) );
    Glcd_Line(96, 32, xEndPoint, yEndPoint, 1);
    //for second
    temp=(second*6);

    angle=(temp/57.29577951) ;             //Convert degrees to radians

    xEndPoint = ( 96 + (sin(angle)*(23)) );
    yEndPoint = ( 32 - (cos(angle)*(23)) );

    Glcd_Line(96, 32, xEndPoint, yEndPoint, 1);

}
void editSecond(int second){
    float angle=0.0;
    unsigned int temp=0,xEndPoint,yEndPoint;
    temp=(second*6);
    angle=(temp/57.29577951) ;             //Convert degrees to radians
    xEndPoint = ( 96 + (sin(angle)*(23)) );
    yEndPoint = ( 32 - (cos(angle)*(23)) );
    Glcd_Line(96, 32, xEndPoint, yEndPoint, 0);
}
void editMinute(int minute){
     float angle=0.0;
    unsigned int temp=0,xEndPoint,yEndPoint;
    temp=(minute*6);
    angle=(temp/57.29577951) ;             //Convert degrees to radians
    xEndPoint = ( 96 + (sin(angle)*(20)) );
    yEndPoint = ( 32 - (cos(angle)*(20)) );
    Glcd_Line(96, 32, xEndPoint, yEndPoint, 0);
}
void editHour(int hour,int minute){
    float angle=0.0;
    unsigned int temp=0,xEndPoint,yEndPoint;
    angle=hour*30+((minute*6)/12);
    angle=(angle/57.29577951) ;             //Convert degrees to radians
    xEndPoint = ( 96 + (sin(angle)*(15)) );
    yEndPoint = ( 32 - (cos(angle)*(15)) );
    Glcd_Line(96, 32, xEndPoint, yEndPoint, 0);


}
//Create interrupt
void interrupt(){
     if(INTCON.TMR0IF){
         millisecond++;
          TMR0H = 0xFC;
          TMR0L = 0x19;

          if(millisecond==1000){
               initSecond++;
               millisecond=0;

          }

          if(initSecond==60){
               initMinute++;
               initSecond=0;
               oneSecActive=1;
          }
          if(initMinute==60){
               initMinute=0;
               initHour++;
               oneHourActive=1;
          }
          if(initHour>=23){
                initHour=0;
          }
          INTCON.TMR0IF=0;
     
     }


}


void main() {
  ANSELD=0;                   // Configure AN pins as digital
  ANSELC=0;                    // Configure AN pins as digital
  TRISD = 255;


  Glcd_Init();                // Call GLCDpinout settings
  Glcd_Fill(0);



  while(1){
       Glcd_Write_Text("Clock", 12,2,1);
       Glcd_Write_Text(userHourTxt, 12,3,1);

       if(adjusmentIndex==0){
              Delay_ms(250) ;
             Glcd_Write_Text(userHourTxt, 12,3,2);
             Delay_ms(250) ;
       }
       Glcd_Write_Text(":", 24,3,1);
       Glcd_Write_Text(userMinuteTxt, 29,3,1);
       if(adjusmentIndex==1){
              Delay_ms(250) ;
             Glcd_Write_Text(userMinuteTxt, 29,3,2);
             Delay_ms(250) ;
       }
      //INCREASE
       if( PORTC.F1==1){
            if(adjusmentIndex==0){
                 initHour++;
                 userHourTxt[0] = initHour/10 + 48;
                 userHourTxt[1] = initHour%10 + 48;
                 if(initHour>=23){
                      initHour=-1;
                 }
            }
            if(adjusmentIndex==1){
                 initMinute++;
                 userMinuteTxt[0] = initMinute/10 + 48;
                 userMinuteTxt[1] = initMinute%10 + 48;
                 if(initMinute>=59){
                      initMinute=-1;
                 }
            }
       }
       if(PORTC.F0==1 && adjusmentIndex==0){
            adjusmentIndex=1;
            Delay_ms(250) ;
       }
       if(PORTC.F0==1 && adjusmentIndex==1){
            adjusmentIndex=0;
            Delay_ms(250) ;
       }
       if(PORTC.F2){
            adjusmentIndex=2;
       }
       if(adjusmentIndex==2){
            drawClockInterface();
            Glcd_Write_Text(userHourTxt, 12,3,1);
            Glcd_Write_Text(userMinuteTxt, 29,3,1);

            timerOperations();

            clockComponents(initHour,initMinute,initSecond);

            userHourTxt[0] = initHour/10 + 48;
            userHourTxt[1] = initHour%10 + 48;
             userMinuteTxt[0] = initMinute/10 + 48;
             userMinuteTxt[1] = initMinute%10 + 48;

            oldSecond=initSecond ;
            oldHour=initHour;
            oldMinute=initMinute;

            editSecond(--oldSecond);
            if(oneSecActive){
                oneSecActive=0;
                editMinute(--oldMinute);
                editSecond(59);
            }
            if(oneHourActive){
                 oneHourActive=0;
                 editHour(--oldHour,--oldMinute);
                 editMinute(59);
            }
        }

  }
}