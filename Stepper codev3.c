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

#include <time.h>

int xStep = 0;
int yStep = 0;

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
        //printf("Stp: %x\n", foo);
    }

    void delay_ms (int foo){
        sleep (foo/1000);
    }

    void delay_us (int foo){
        sleep (foo/1000000);
    }

void twoStep(float lALT, float LAZ);

int main(){
    while(1){

        // Generaging random numbers to test the function we have to set a range to ensure ALT & AZ > pi/2 & 2pi 
        srand(time(NULL));
        float a = 1.0;
        float rALT = (float)rand()/(float)RAND_MAX/1.57079;
        float rAZ  = (float)rand()/(float)RAND_MAX/6.28318;


        twoStep(rALT,rAZ);                          // values i know work 1.351567,0.57234 // rALT,rAZ
        
        printf("Task completed!\n\n\n\n");
        sleep(1);
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
    float yStepsTobe = (float)lALT/(float)((float)(pi/2)/6144);      // ALT we have 0 - pi/2 rad at 6144step  360/(5.625/64) = 4096 steps per 2pi; Gear ratio of 10:60 -> (60/4)/10 = 1.5 turns for 2/pi rad 4096*1.5 = 6144 steps for 90 deg or 2/pirad 
    float xStepsTobe = (float)lAZ/(float)((float)(2*pi)/(6144*4));   // AZ  we have 0 - 2pi rad at (ALTsteps*4)       // at this step speed one revolution should take 24576ms or 24.6 seconds this means the longest if shoul take to a point the in the sky is 12.3 seconds

    printf("Inout;        AZ:%f      ALT:%f\n", lAZ, lALT);
    printf("to be;        yStepsTobe:%f      xStepsTobe:%f\n", yStepsTobe, xStepsTobe);

    printf("yStep: %d xStep: %d\n", yStep, xStep );
    // Lets convert the input radien and the current psition form step values to find the diffrence. 
    // max stp num for Alt is: 6144             6144/(pi/2) = 15645.56 So Alt Has 1
    // max stp num for AZ  is: 6144*4 = 24576


        // Now we need to find the step requiered to get there
    signed int yToDo = yStepsTobe - yStep;
    signed int xToDo = xStepsTobe - xStep;

    printf("Y to do: %d     X to do: %d\n", yToDo, xToDo);
    //float difs = (float)yToDo / (float)xToDo;
    //printf("Mis Vals;                         dif:%f yToDo:%d xToDo:%d \n",difs,yToDo,xToDo);    
   
    int yDir = 1;   if (yToDo < 0)   yDir = -1;              //Assigning a value to know what direction to move the motors
    int xDir = 1;   if (xToDo < 0)   xDir = -1;              //Using the nested if statment. 

    
        int hToDo;
        int lToDo;

        signed int *hStep;
        signed int *lStep;

        int hDir;
        int lDir;

        char hSft;
        char lSft;


    if (abs(yToDo)>abs(xToDo)){
        //
        hToDo = abs(yToDo);
        lToDo = abs(xToDo);

        hStep = &yStep;
        lStep = &xStep;

        hDir = yDir;
        lDir = xDir;  

        hSft = 0;
        lSft = 4;

        if (yToDo == 0){
            yToDo = 1;             //This is to ensure we dont ge th stuck in a loop with no end, not the best compromise in terms of accurecy but i has to do 
            printf("If within the first if %d\n", yToDo);
        } 

        printf("Debug first if\n");
        
    }
    else if (abs(yToDo)<abs(xToDo)){
        //
        hToDo = abs(xToDo);
        lToDo = abs(yToDo);

        hStep = &xStep;
        lStep = &yStep;

        hDir = xDir;
        lDir = yDir; 

        
        hSft = 4;
        lSft = 0;

        if (xToDo == 0){
            xToDo = 1;             //Same reasons as
            printf("If within the second if %d\n", xToDo);
        } 

        printf("Debug second if\n");
    }

    else                             // intended for the cases where the x and y change values are the same so we run both at once
        while (abs(yToDo) > 0){
            output_b(HalfStep[yStep&0x7]<<4|HalfStep[xStep&0x7]);
            delay_ms (d);
            yToDo --;
            xToDo --;
            xStep ++;
            yStep ++;
        }

/*    while (*lStep == 0 && abs(*hStep) > 0){
        output_b(HalfStep[yStep&0x7]<<hSft);
        *hStep+=hDir;
            printf("One axis loop lToDo is %d\n", lToDo);
            printf("One axis loop hToDo is %d\n", hToDo);

    }
*/
    ///////////////////////// Main Stepper Code Loop ///////////////////////////
    while (hToDo>lToDo){

            printf("hToDo is %d\n", hToDo);
            printf("lToDo is %d\n", lToDo);
            printf("yDir  %d  xDir: %d\n",yDir,xDir);

        while ( lToDo > 0 ){           
            printf("while loop hToDo is %d\n", hToDo);
            printf("while loop lToDo is %d\n", lToDo);
            
            for ( int i = (int)(hToDo/(lToDo)); i > 0; i--){                  // Inside the x itteration we have the more frequent y itteration
                //printf("xToDo is %d\n", xToDo);

                    //printf("Fast loop -> ");
                delay_ms(d);
                output_b(HalfStep[yStep&0x7]<<lSft);
                *hStep+=hDir;
                hToDo--;
            }

            // Debugging prints/*
               // printf("hToDo is %d\n", hToDo);
               // printf("lToDo is %d\n", lToDo);

               // printf("Vals;    xStep:%d           yStep%d\n", xStep, yStep);
               // printf("Vals;    xLeft:%d           yLeft%d\n", xToDo, yToDo);
               // printf("Loop ");   
               // printf("Slow loop -> ");

            output_b(HalfStep[xStep&0x7]<<hSft | HalfStep[yStep&0x7]<<lSft);
                
            *lStep+=lDir;
            lToDo--; 
            *hStep+=hDir;

            delay_ms(d);
                //printf("H Step: %d\n", *hStep);
                //printf("L Step: %d\n", *lStep);
        }

        // printf("Done With this loop\n");
    }

        // after action report
        printf("Inout;        AZ:%f      ALT:%f\n", lAZ, lALT);
        printf("Output;       AZ:%f      ALT:%f\n", (float)xStep*((float)(2*pi)/24576), (float)yStep*((float)(pi/2)/6144));

        printf("Vals;    xStep:%d      xLeft%d\n", *lStep, lToDo);
        printf("Vals;    yStep:%d      yLeft%d\n", *hStep, hToDo);



}

    // We can use modulo to ajust the loop 

    ////////////////////////////////////////////  OLD CODE  //////////////////////////////////////////////////////////
/*B
        // Now we have if statments for the cases where one number is higher then the other and where they are equal
    if (abs(yToDo)>abs(xToDo)){                               // IN this case Y is higher
        
        float dif = (float)yToDo / (float)xToDo;
        //printf("The fuking diffrence! %4f\n",dif);
        int intdif  = (int)dif;
        float decpart = dif - intdif;

        int usDelay = decpart * 1000;
        //printf("Alt > AZ for loop\n");
        
        //printf("Mis Vals;                         decpart:%f usDelay:%d dif:%f yDir:%d xDir%d yToDo:%d xToDo:%d \n",decpart,usDelay,dif,yDir,xDir,yToDo,xToDo);    





        for ( ; xToDo > 0; xToDo--){                             // Here we itterate for the value of x as it lower 
            //delay_us(1000-usDelay);
        
        //printf("Main For loop\n");
        //printf("Vals;    xStep:%d           yStep%d\n", xStep, yStep);
        //printf("Vals;    xLeft:%d           yLeft%d\n", xToDo, yToDo);

        //printf("Mis Vals;                         decpart:%f usDelay:%d dif:%f yDir:%d xDir%d yToDo:%d xToDo:%d \n",decpart,usDelay,dif,yDir,xDir,yToDo,xToDo);    
        
            printf("UsDelay is %d\n", usDelay);
            printf("xToDo is %d\n", xToDo);
            for (int i = (int)(yToDo/xToDo); i > 0; i--){                  // Inside the x itteration we have the more frequent y itteration
                printf("Fast loop -> ");
                delay_ms(d);
                output_b(HalfStep[yStep&0x7]<<4);
                yStep+=yDir;
                yToDo-=yDir;
                
            }
            printf("Slow loop -> ");
            output_b(HalfStep[xStep&0x7]);
            xStep+=xDir;
            delay_ms(d);
            //delay_us(usDelay);
        }
        printf("Done With this loop\n");
        exit;



        //WTF I dont understand any of this code
        // so maybe i do get it:
        // here is some theory
        // Say we have 27 to 5 
        // 27/5 = 5.4
        // 27%5 = 2
        // 5*5+2 = 27
        // :. we run 5:1 untill the last 2 where its 6:1
        // ::. (5-2)*5+6+6 = 27
        // It would be good if these extra 2 got distributed evenly and not all at the end 
        // Possible this will work but now i must sleep
        //
    }

    else if (abs(xToDo)>abs(yToDo)){                          // IN this case X is higher
        printf("We have not yet figured out how to have AZ greater then ALT\n\n");
        exit;

    }


    else 
        for (int z = abs(yToDo); z > 0; z--){
            output_b(HalfStep[yStep&0x7]<<4|HalfStep[xStep&0x7]);
            delay_ms (d);
            xStep ++;
            yStep ++;
        }
        //Report on the steps taken, final s any y step count as well
        printf("Step report: \n");


        
        printf("Inout;        AZ:%f      ALT:%f\n", lAZ, lALT);
        printf("Output;       AZ:%f      ALT:%f\n", (float)xStep*((float)(2*pi)/24576), (float)yStep*((float)(pi/2)/6144));

        printf("Vals;    xStep:%d      xLeft%d\n", xStep, xToDo);
        printf("Vals;    yStep:%d      yLeft%d\n", yStep, yToDo);
        exit;
}



*/


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