/*
                             
#fuses INTRC_IO,NOWDT,NOPROTECT,NOLVP,MCLR,WDT 
#use delay(clock=8000000) 
*/

/*
As C libraries use Radien for math equations they 
will be used for angles unless otherwise specified. 
Common conversions for reference:
    
    deg     rad
    360     2pi 
    180     pi
    90      2/pi
    0       0

*/
//#include <16f819.h>   

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

unsigned int xStep = 0;
unsigned int yStep = 0;

//_bif void output_b(int8 value);

int d = 1;

float pi = 3.141592654;

unsigned int HalfStep[8]={0x1,0x3,0x2,0x6,0x4,0xC,0x8,0x9};

    // Binary and hex values for the 8 half step positions for both motors
    /////////////////////////////////////////////////////////////////////////////////////////////
    //     0b0001,    0b0011,    0b0010,    0b0110,    0b0100,    0b1100,    0b1000,    0b1001 //
    //        0x1,       0x3,       0x2,       0x6,       0x4,       0xC,       0x8,       0x9 //
    // 0b00010000,0b00110000,0b00100000,0b01100000,0b01000000,0b11000000,0b10000000,0b10010000 //
    //       0x10,      0x30,      0x20,      0x60,      0x40,      0xC0,      0x80,      0x90 //
    /////////////////////////////////////////////////////////////////////////////////////////////
// Pic functions
    void output_b (int foo){
        printf("Stp: %x\n", foo);
    }

    void delay_ms (int foo){
        sleep (foo);
    }

    void delay_us (int foo){
        sleep (foo);
    }

void twoStep(float lALT, float LAZ);

int main(){
    while(1){

        twoStep(10,3);
        printf("Task completed!\n");
        sleep(60);
/*        
    output_b(HalfStep[yStep&0x7]<<4|HalfStep[xStep&0x7]);   
    delay_ms(d);
    
    xStep ++;
    yStep ++;
    */
    }
}
   
void twoStep(float lALT, float lAZ){                        // ALT == y    AZ == x  in Rad and what the new cordinates need to be
        // We need to convert the Rad value to step equivilent
    float y = (float)lALT*(float)(6144/(float)(pi/2));      // ALT we have 0 - pi/2 rad at 6144step  360/(5.625/64) = 4096 steps per 2pi; Gear ratio of 10:60 -> (60/4)/10 = 1.5 turns for 2/pi rad 4096*1.5 = 6144 steps for 90 deg or 2/pirad 
    float x = (float)lAZ*(float)((6144*4)/(float)(2*pi));   // AZ  we have 0 - 2pi rad at (ALTsteps*4)       // at this step speed one revolution should take 24576ms or 24.6 seconds this means the longest if shoul take to a point the in the sky is 12.3 seconds
    
    // Lets convert the input radien and the current psition form step values to find the diffrence. 
    // max stp num for Alt is: 6144             6144/(pi/2) = 15645.56 So Alt Has 1
    // max stp num for AZ  is: 6144*4 = 24576
/*    int xStepLeft = 6144/(pi/2)
    int yStepLeft = */
    


        // Now we need to find the step requiered to get there
    int yLft = y - yStep;
    int xLft = x - xStep;
    
    int yDir = 1;   if (lALT < 0)   yDir = -1;              //Assigning a value to know what direction to move the motors
    int xDir = 1;   if (lAZ < 0)    xDir = -1;              //Using the nested if statment. 

    
        // Now we have if statments for the cases where one number is higher then the other and where they are equal
    if (abs(yLft)>abs(xLft)){                               // IN this case Y is higher
        int dif = yLft/xLft;
        float mod = yLft%xLft;
        int usDelay = mod * 1000;
        printf("We made it this far!\n");

        for (int *z = &xLft; *z > 0; *z--){                             // Here we itterate for the value of x as it lower 
            //delay_us(1000-usDelay);
        printf("Vals;    xStep:%d           yStep%d\n", xStep, yStep);
        printf("Vals;    xLeft:%d           yLeft%d\n", xLft, yLft);    
            printf("Main For loop\n");
/*            printf("UsDelay is %d\n", usDelay);
            printf("xLft is %d\n", xLft);*/
            for (int *i = &dif; *i > 0; *i--){                  // Inside the x itteration we have the more frequent y itteration
                printf("Fast loop -> ");
                output_b(HalfStep[yStep&0x7]<<4);
                yStep+=yDir;
                delay_ms(d);
            }
            printf("Slow loop -> ");
            output_b(HalfStep[xStep&0x7]);
            xStep+=xDir;
            //delay_us(usDelay);
        }
        printf("We made it this far!\n");
        exit;
    }

    else if (abs(xLft)>abs(yLft)){                          // IN this case X is higher
        int dif = xLft/yLft;
        float mod = xLft%yLft;
        int usDelay = mod * 1000;

        for (int z = yLft; z > 0; z--){                             // Here we itterate for the value of Y as it lower 
            for (int i = dif; i > 0; i--){                  // Inside theYx itteration we have the more frequent X itteration
                output_b(HalfStep[xStep&0x7]);
                xStep+=xDir;
                delay_ms(d);
            }
            output_b(HalfStep[yStep&0x7]<<4);
            yStep+=yDir;
            delay_us(usDelay);
        }
        exit;
    }


    else 
        for (int z = abs(yLft); z > 0; z--){
            output_b(HalfStep[yStep&0x7]<<4|HalfStep[xStep&0x7]);
            delay_ms (d);
            xStep ++;
            yStep ++;
        }
        exit;
}






/*

    float div = 0.0;
    div = (float)lALT / (float)lAZ;
    float mdiv = lALT%lAZ;
    if (lALT == 0||lAZ == 0||lALT == lAZ) break;
    int *m = &xStep;
    
    if (div > 1){                       // we have motor Y as the leading
        for (lAZ > xStep; xStep++;){
            for (mdiv > 0; mdiv--;){
                yStep++;
                HalfStep[yStep&0x7]<<4;    
            } 
            HalfStep[xStep&0x7];
        }
    }
                 
    if (div < 1){                       // we have motor X as the leading
        div = 1/div;
          for (lALT > yStep; yStep++;){
            for (mdiv > 0; mdi--;){
                yStep++;
                HalfStep[xStep&0x7];    
            } 
            HalfStep[yStep&0x7]<<4;
        }
    }
        for (lAZ > xStep; xStep++;){
            for (mdiv > 0; mdi--;){
                yStep++;
                HalfStep[yStep&0x7]<<4;    
            } 
            HalfStep[*m&0x7]<<b;
        }
}   
 */   
//    return = HalfStep[dir&0x7];     // motor X AZ
//    return = HalfStep[dir&0x7]<<4;  // motor y ALT
    
    //int i_div = (int) div;
    //int i_div = floor(div);
    // div - i_div

/*
int xstep(int dir){ //dir should be 1 or -1
    xStep += dir;
    return = HalfStep[dir&0x7];
}
int ystep(int dir){ //dir should be 1 or -1
    yStep += dir;
    return = HalfStep[dir&0x7]<<4;  
}
*/