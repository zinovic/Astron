/* 
 * File:   pic24main.c
 * Author: Nicholas
 *
 * Created on 06 February 2016, 13:09
 */

//#include <stdio.h>
//#include <stdlib.h>


// Notes
    // As C libraries use Radien for math equations they 
    // will be used for angles unless otherwise specified. 
    // Common conversions for reference:
    //    
    //     deg     rad
    //     360     2pi 
    //     180     pi
    //     90      2/pi
    //     0       0

    // #fuses INTRC_IO,NOWDT,NOPROTECT,NOLVP,MCLR,WDT 
    // #use delay(clock=8000000) 
    // #include <16f819.h>   

#include <24FJ128GA202.h>

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
//#include <unistd.h>
#include <time.h>

//#fuses INTRC_IO,NOWDT,NOPROTECT,WDT 
#use delay(clock=4000000) 
#use rs232(baud=9600, XMIT=PIN_A2, RCV=PIN_A3, stream=PC, ERRORS)


// Shared ints
//    float pi = 3.141592654;

// Start ints
    float RA = 0;   //4.370740779;
    float DEC = 0;  //0.636463406;

    float LAT = 0.916297857;
    float LNG = -0.033452145;

    float LST = 0;
    float HA = 0.949155775;

    float ALT = 0;
    float AZ = 0;

    int year = 2016;
    int months = 2;
    int day = 10;

    int hour = 21;
    int minute = 40;
    int second = 30;

    float dUT = 0;
    float JSK = 0;

    int xStep = 0;
    int yStep = 0;

// Stepper ints
    int d = 1;
    float radStep = 0.0002556634;
    unsigned int HalfStep[8]={0x1,0x3,0x2,0x6,0x4,0xC,0x8,0x9};

        // Binary and hex values for the 8 half step positions for both motors
        /////////////////////////////////////////////////////////////////////////////////////////////
        //     0b0001,    0b0011,    0b0010,    0b0110,    0b0100,    0b1100,    0b1000,    0b1001 //
        //        0x1,       0x3,       0x2,       0x6,       0x4,       0xC,       0x8,       0x9 //
        // 0b00010000,0b00110000,0b00100000,0b01100000,0b01000000,0b11000000,0b10000000,0b10010000 //
        //       0x10,      0x30,      0x20,      0x60,      0x40,      0xC0,      0x80,      0x90 //
        /////////////////////////////////////////////////////////////////////////////////////////////

// Pic functions
//    void output_b (int foo){
//        //printf("Stp: %x\n", foo);
//    }
//
//    void delay_ms (int foo){
//        sleep (foo/1000);
//    }
//
//    void delay_us (int foo){
//        sleep (foo/1000000);
//    }

// Stars
    // struct Alkaid {
    //     float ra  = 13.79237392; 
    //     float dec = 49.31330288;
    // }r,d;

    // struct Mizar {
    //     float ra  = 13.39872773; 
    //     float dec = 54.92541525;
    // }r,d;




    // struct Megrez {
    //     float ra  = 12.25706919; 
    //     float dec = 57.03259792;
    // }r,d;

    // struct Phad {
    //     float ra  = 11.89715035; 
    //     float dec = 53.69473296;
    // }r,d;

    // struct Merak {
    //     float ra  = 11.0306641; 
    //     float dec = 56.38234478;
    // }r,d;

    // struct Dubhe {
    //     float ra  = 11.06217691; 
    //     float dec = 61.75111888;
    // }r,d;

    typedef struct {
        float ra; 
        float dec;
    } Star;


// // Consteations
//    char UrsaMajor[6] = {Alkaid,Mizar,Alioth,Megrez,Phad,Merak,Dubhe};

////////////////////////////////////////////     Function Defs     //////////////////////////////////////////////////////////
// Star funcs
    float Altitude (char lDEC, char lLAT, char lHA);
    float Azimuth (char lDEC, char lALT, char lLAT);

    float radfrom(float x);
    float degfrom(float x);

    float J2kfrom( int y, int m, int d, float t);
    
    float HourAngle (char lLST, char lRA);

    float LocalSdrTime (float lJSK, float lLNG, int lUT);
    
    float decimalday(int h, int m, int s);

    int inputfloat (char x);

//    void DumpPrint ();

// Step funcs
    void twoStep(float lALT, float LAZ);

void main(){
//    while(1){

        // Generaging random numbers to test the function we have to set a range to ensure ALT & AZ > pi/2 & 2pi 
//        srand(time(NULL));
//        float a = 1.0;
//        float rALT = (float)rand()/(float)RAND_MAX/1.57079;
//        float rAZ  = (float)rand()/(float)RAND_MAX/6.28318;

        Star Alioth;

        Alioth.ra = 12.9004536; 
        Alioth.dec = 55.95984301;

        dUT = decimalday(hour,minute,second);
        JSK = J2kfrom(year,months,day,dUT);
        
        LST = LocalSdrTime (JSK,LNG,dUT);
        HA = HourAngle(LST,RA);
        
        ALT = Altitude(Alioth.dec,LAT,HA);
        AZ = Azimuth(Alioth.dec,ALT,LAT);



        twoStep(ALT,AZ);                          // values i know work 1.351567,0.57234 // rALT,rAZ

        
        printf("Task completed!\n\n\n\n");
        delay_ms(6000);
//    }
        
}

////////////////////////////////////////////     Functions      //////////////////////////////////////////////////////////

//Star Stuff 
    float Altitude (char lDEC, char lLAT, char lHA){
        float x = sin(lDEC)*sin(lLAT)+cos(lDEC)*cos(lLAT)*cos(HA);
        return asin(x);
    }

    float Azimuth (char lDEC, char lALT, char lLAT){
        float x = (sin(lDEC) - sin(lALT)*sin(lLAT))/(cos(lALT)*cos(lLAT));
      return acos(x);
    }

    float HourAngle (char lLST, char lRA){
        return lLST - lRA;
    }

    float LocalSdrTime ( float lJSK, float lLNG, int lUT){
        return 100.46 + 0.985647 * lJSK + lLNG + 15*lUT;
    }
    //float SidrielTime (char){}

    float J2kfrom( int y, int m, int d, float t){
        float JD = d-32075+1461*(y+4800+(m-14)/12)/4+367*(m-2-(m-14)/12*12)/12-3*((y+4900+(m-14)/12)/100)/4;
        return JD -2451545.0 + (double)t-0.5;     // julian day for epoh 2000
    }

    float decimalday(int h, int m, int s){
        return (h+m/(double)60)/(double)24;
    }

    float degfrom(float x){
        return x * ((double)180/PI);
    }
    float radfrom(float x){
        return x * (PI/(double)180);
    }

    /*
    int inputfloat(char c){
      scanf("%f", &c);
    }
    */

    //void DumpPrint (){
    //  printf("RA: %f\n", RA);
    //  printf("DEC: %f\n", DEC );
    //  printf("LAT: %f\n", LAT);
    //  printf("LNG: %f\n", LNG);
    //  printf("HA: %f\n", HA);
    //  printf("ALT: %f\n", ALT);
    //  printf("AZ: %f\n", AZ);
    //}

    //  Two step   
    void twoStep(float lALT, float lAZ){                        // ALT == y    AZ == x  in Rad and what the new cordinates need to be
            // We need to convert the Rad value to step equivilent
        float yStepsTobe = (float)lALT/(float)radStep;          // ALT we have 0 - pi/2 rad at 6144step  360/(5.625/64) = 4096 steps per 2pi; Gear ratio of 10:60 -> (60/4)/10 = 1.5 turns for 2/pi rad 4096*1.5 = 6144 steps for 90 deg or 2/pirad 
        float xStepsTobe = (float)lAZ/(float)radStep;           // AZ  we have 0 - 2pi rad at (ALTsteps*4)       // at this step speed one revolution should take 24576ms or 24.6 seconds this means the longest if shoul take to a point the in the sky is 12.3 seconds
                                                            
                                                            // Explenation
                                                                // lAZ/((2*pi)/(6144*4)) = 0.00025566346464760687161968126491532
                                                                // Lets convert the input radien and the current psition form step values to find the diffrence. 
                                                                // max stp num for Alt is: 6144             6144/(pi/2) = 15645.56 So Alt Has 1
                                                                // max stp num for AZ  is: 6144*4 = 24576
            // Now we need to find the step requiered to get there
        signed int yToDo = yStepsTobe - yStep;
        signed int xToDo = xStepsTobe - xStep;

            // Report printfs 
        printf("AZ : %f     xStepsTobe: %f\n", lAZ, xStepsTobe);
        printf("ALT: %f     yStepsTobe: %f\n", lALT,yStepsTobe);

        printf("xStep: %d xToDO: %d\n", xStep, yToDo);
        printf("yStep: %d yToDo: %d\n", yStep, xToDo);

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

        if (yToDo == 0 && xToDo == 0){                          // This is to prevent goinginto the loop if no movement happens. To stop in crashing inside the loop. 
            printf("ERORO: You told me to go noWHERE\n");
            return;}

        if (abs(yToDo)>abs(xToDo)){
            
            hToDo = abs(yToDo);
            lToDo = abs(xToDo);

            hStep = &yStep;
            lStep = &xStep;

            hDir = yDir;
            lDir = xDir;  

            hSft = 0;
            lSft = 4;

            if (xToDo == 0){
                lToDo = 1;                                      //This is to ensure we dont ge th stuck in a loop with no end, not the best compromise in terms of accurecy but i has to do 
                // printf("If within the first if %d\n", yToDo);
            } 
            // printf("Debug first if\n");     
        }

        else if (abs(xToDo)>abs(yToDo)){
            
            hToDo = abs(xToDo);
            lToDo = abs(yToDo);

            hStep = &xStep;
            lStep = &yStep;

            hDir = xDir;
            lDir = yDir; 

            
            hSft = 4;
            lSft = 0;

            if (yToDo == 0){
                lToDo = 1;                                      //Same reasons as above
                // printf("If within the second if %d\n", xToDo);
            } 
            // printf("Debug second if\n");
        }

        else                                                    // intended for the cases where the x and y change values are the same so we run both at once
            while (abs(yToDo) > 0){
                output_b(HalfStep[yStep&0x7]<<4|HalfStep[xStep&0x7]);
                delay_ms (d);
                yToDo --;
                xToDo --;
                xStep ++;
                yStep ++;
            }

        ///////////////////////// Main Stepper Code Loop ///////////////////////////
        while (hToDo>lToDo){
            // Debugging prints
                // printf("hToDo is %d\n", hToDo);
                // printf("lToDo is %d\n", lToDo);
                // printf("yDir  %d  xDir: %d\n",yDir,xDir);
            //
            while ( lToDo > 0 ){
                // Debugging prints
                    //printf("while loop hToDo is %d\n", hToDo);
                    //printf("while loop lToDo is %d\n", lToDo);
                // Print statment for accurecy checks
                    printf("%d,%d\n",lToDo,hToDo );
                    //    
                for ( unsigned int i = (unsigned int)((hToDo/(lToDo))+0.5); i > 0; i--){                  // Inside the x itteration we have the more frequent y itteration
                    // Debugging prints
                        //printf("xToDo is %d\n", xToDo);
                        //printf("Fast loop -> ");
                    //
                    delay_ms(d);
                    output_b(HalfStep[yStep&0x7]<<lSft);
                    *hStep+=hDir;
                    hToDo--;
                }

                // Debugging prints
                   // printf("hToDo is %d\n", hToDo);
                   // printf("lToDo is %d\n", lToDo);

                   // printf("Vals;    xStep:%d           yStep%d\n", xStep, yStep);
                   // printf("Vals;    xLeft:%d           yLeft%d\n", xToDo, yToDo);
                   // printf("Loop ");   
                   // printf("Slow loop -> ");
                //
                output_b(HalfStep[xStep&0x7]<<hSft); //| HalfStep[yStep&0x7]<<lSft);
                    
                *lStep+=lDir;
                lToDo--;
                //*hStep+=hDir;

                delay_ms(d);
                // Debugging prints
                    //printf("H Step: %d\n", *hStep);
                    //printf("L Step: %d\n", *lStep);
                //
            }

            // printf("Done With this loop\n");
        }

            // after action report
            printf("Inout;        AZ:%f      ALT:%f\n", degfrom(lAZ), degfrom(lALT));
            printf("Output;       AZ:%f      ALT:%f\n", degfrom((float)xStep*(double)radStep), degfrom((float)yStep*(double)radStep));

            printf("diffrence;    AZ:%f      ALT:%f\n\n", degfrom((float)lAZ-(float)xStep*(float)radStep), degfrom((float)lALT-(float)yStep*(float)radStep));

            printf("Vals;    xStep:%d      xLeft%d\n", *lStep, lToDo);
            printf("Vals;    yStep:%d      yLeft%d\n", *hStep, hToDo);
    }

////////////////////////////////////////////  OLD CODE & Notes  //////////////////////////////////////////////////////////

    // We can use modulo to ajust the loop 

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
    // How to distribute the extra steps evenly 
    // Some theory
    // 
    // say we have 213 & 75
    // 213/75 = 2.84
    // 214%75 = 63
    // 
    // 75/63 = 1.19047
    // 75%63 = 12
    // 
    // 75/12 = 6.25
    // 75%12 = 3
    // 
    // 75/3 = 25
    // 75%3 = 0
    //
    // :. We run the motor @ 2:1
    // ::. bah this does not work! 
    // 
    // 
    // 
    // 
    // 
    // 
    // 
    // 
    // 
    // 
    // 

