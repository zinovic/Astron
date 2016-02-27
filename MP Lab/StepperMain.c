#define PORT_S PIN_B3

#if !defined(__PCD__)
#error This example will only compile for the PCD 24 bit compiler
#endif

#include <24HJ128GP502.h>

#fuses HS,NOWDT,noprotect
//#fuses FRC,FRC_PLL,NOWDT,NOPROTECT,
#use delay(clock=7350000)                   //7.212Mhz is apparently optimal for 0% error with baud 9600. 
#use rs232(baud=9600, RCV=PIN_B13, stream=GPSMODULE, ERRORS)
#use rs232(baud=9600, XMIT=PIN_B14, RCV=PIN_B15, stream=PC, ERRORS,parity=N,bits=8)

    //     deg     rad
    //     360     2pi 
    //     180     pi
    //     90      2/pi
    //     0       0

#include <stdio.h>
#include <stdlib.h>
#include <math.h>    
    //float pi = 3.141592654;

    float radfrom(float x);
    float degfrom(float x);

    int d = 1;
    float radStep = 0.0002556634;
    unsigned int HalfStep[8]={0x1,0x3,0x2,0x6,0x4,0xC,0x8,0x9};
    
    void twoStep(float lALT, float LAZ);
    
    int xStep = 0;
    int yStep = 0;
    
void main() {
    while (1){
        
        printf("\n\n\n\rFrom the top\n\r");

        printf("\n\nAZ 180deg\n\r");
        twoStep(2*pi,2/pi);
        twoStep(0,0);
//        twoStep(0,0);
//        delay_ms(2000);        
//        printf("\n\nALT motor\n\r");
//        twoStep(0.2,0);
//        delay_ms(500);
//        printf("\n\r");
//        twoStep(0,0);
//        delay_ms(2000);
//        
//        printf("\n\nAZ motor\n\r");
//        twoStep(0,0.2);
//        delay_ms(500);
//        printf("\n\r");
//        twoStep(0,0);
//        delay_ms(2000);
        
//        printf("\n\nBoth motors\n\r");
//        twoStep(0.2,0.2);
//        delay_ms(500);
//        printf("\n\r");
//        twoStep(0,0);
//        delay_ms(2000);
        
//        printf("\nRandom values \n\r");
//        twoStep(0.5,1);
//        delay_ms(500);
//        printf("\n\r");
//        twoStep(1.5,0.1);
//        delay_ms(2000);
        
//        printf("\n more Random values \n\r");
//        twoStep(0.9,0.7);
//        delay_ms(500);
//        printf("\n\r");
//        twoStep(0,1.2);
//        delay_ms(2000);
//    
//        twoStep(0,0);
//        delay_ms(2000);
    }
        
    
    return;
}

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
        printf("AZ : %f     xStepsTobe: %f\r\n", lAZ, xStepsTobe);
        printf("ALT: %f     yStepsTobe: %f\r\n", lALT,yStepsTobe);

        printf("xStep: %d xToDO: %d\r\n", xStep, xToDo);
        printf("yStep: %d yToDo: %d\r\n", yStep, yToDo);

            //printf("Mis Vals;                         dif:%f yToDo:%d xToDo:%d \n",difs,yToDo,xToDo);    
       
        int yDir = 1;   if (yToDo < 0)   yDir = -1;              //Assigning a value to know what direction to move the motors
        int xDir = 1;   if (xToDo < 0)   xDir = -1;              //Using the nested if statment. 


        
            int hToDo;
            int lToDo;

            signed int *hStep_ptr;
            signed int *lStep_ptr;

            int hDir;
            int lDir;

            char hSft;
            char lSft;

        if (yToDo == 0 && xToDo == 0){                          // This is to prevent goinginto the loop if no movement happens. To stop in crashing inside the loop. 
            printf("ERORO: You told me to go noWHERE\r\n");
            return;}

        if (abs(yToDo)>abs(xToDo)){                             // This is where ALT is larger ALT is Y 
            
            hToDo = abs(yToDo);
            lToDo = abs(xToDo);

            hStep_ptr = &yStep;
            lStep_ptr = &xStep;

            hDir = yDir;
            lDir = xDir;  

            hSft = 0;
            lSft = 4;

//            if (xToDo == 0){
//                lToDo = 1;                                      //This is to ensure we dont ge th stuck in a loop with no end, not the best compromise in terms of accurecy but i has to do 
//                // printf("If within the first if %d\n", yToDo);
//            } 
            // printf("Debug first if\n");     
        }

        else if (abs(xToDo)>abs(yToDo)){                        // This is where AZ is larger AZ is X 
            
            hToDo = abs(xToDo);
            lToDo = abs(yToDo);

            hStep_ptr = &xStep;
            lStep_ptr = &yStep;

            hDir = xDir;
            lDir = yDir; 

            
            hSft = 4;
            lSft = 0;

//            if (yToDo == 0){
//                lToDo = 1;                                      //Same reasons as above
//                // printf("If within the second if %d\n", xToDo);
//            } 
            // printf("Debug second if\n");
        }

        else                                                    // intended for the cases where the x and y change values are the same so we run both at once
            while (abs(yToDo) > 0){
                yToDo = abs(yToDo);
                xToDo = abs(xToDo);
                output_b(HalfStep[yStep&0x7]<<4|HalfStep[xStep&0x7]);
                delay_ms (d);
                yToDo --;
                xToDo --;
                xStep += xDir;
                yStep += yDir;
                output_b(0x000);
            }
            
        if (xToDo == 0 || yToDo == 0){                          // If one motor moves and the other does not, to avoid devision by 0    
            printf("in the one move fore loop\n\r");
            hDir = xDir;
            lDir = yDir;
            yToDo = abs(ytoDo);
            xToDo = abs(xToDo);
            while (hToDo > 0){
                output_b(HalfStep[*hStep_ptr&0x7]<<lSft);
                *hStep_ptr+=hDir;
                hToDo--;
                delay_ms(d);
            }
            output_b(0x000);
        }
            
        ///////////////////////// Main Stepper Code Loop ///////////////////////////
        while (hToDo>lToDo){
            // Debugging prints
                // printf("hToDo is %d\n", hToDo);
                // printf("lToDo is %d\n", lToDo);
                // printf("yDir  %d  xDir: %d\n",yDir,xDir);
            //
            
            int mod = (int) hToDo/(hToDo%lToDo);
            
            while ( lToDo > 0 ){
                // Debugging prints
                    //printf("while loop hToDo is %d\n", hToDo);
                    //printf("while loop lToDo is %d\n", lToDo);
                // Print statment for accurecy checks
                    //printf("%d,%d\r\n",lToDo,hToDo );
                    //    
                for ( unsigned int i = (unsigned int)((hToDo/(lToDo))+0.5); i > 0; i--){                  // Inside the x itteration we have the more frequent y itteration
                    // Debugging prints
//                        printf("hTodo is: %d\r\n", hToDo);
//                        printf("Fast loop -> ");
                        
                        
                    //
//                    printf("hTodo is: %d\r\n", hToDo);
//                    printf("hStep_ptr is: %d\r\n", *hStep_ptr);
//                    printf("hDir  is: %d\r\n", hDir);
//                    printf("Step# is: %d\r\n", HalfStep[*hStep_ptr&0x7]<<*lSft);
                    
                    output_b(HalfStep[*hStep_ptr&0x7]<<lSft);
                    *hStep_ptr+=hDir;
                    hToDo--;
                    delay_ms(d);
                    //printf("%ld,%ld \r\n",hToDo,lToDo);
                    if ( hToDo%lToDo != 0 && hToDo%(  mod /*(int) 300/(hToDo%lToDo)*/ ) == 0 ){
                    	//printf("extra step\n");
	                    i++;	
                    }
                
                }

                // Debugging prints
                    //printf("hToDo is %d\r\n", hToDo);
                    //printf("lToDo is %d\r\n", lToDo);

                    //printf("Vals;    xStep:%d           yStep%d\r\n", xStep, yStep);
                    //printf("Vals;    xLeft:%d           yLeft%d\r\n", xToDo, yToDo);
                    //printf("Loop ");   
                    //printf("Slow loop -> ");
                //
                output_b(HalfStep[*lStep_ptr&0x7]<<hSft); //| HalfStep[yStep&0x7]<<lSft);
                    
                *lStep_ptr+=lDir;
                lToDo--;
                //*hStep_ptr+=hDir;          
                delay_ms(d);
                //printf("%ld,%ld \r\n",hToDo,lToDo);
                // Debugging prints
//                    printf("H Step: %d\r\n", *hStep_ptr);
//                    printf("L Step: %d\r\n", *lStep_ptr);
                //
//                    printf("lTodo is: %d\r\n", lToDo);
//                    printf("lStep_ptr is: %d\r\n", *lStep_ptr);
//                    printf("lDir  is: %d\r\n", lDir);
//                    printf("Step# is: %d\r\n", HalfStep[*hStep_ptr&0x7]<<*lSft);                
                
            }

            // printf("Done With this loop\n");
            output_b(0x000);
        }

            // after action report
            printf("\r\nInput:        AZ:%f      ALT:%f\r\n", degfrom(lAZ), degfrom(lALT));
//            printf("Output:       AZ:%f      ALT:%f\r\n", degfrom((float)xStep*(double)radStep), degfrom((float)yStep*(double)radStep));
//
//            printf("diffrence:    AZ:%f      ALT:%f\r\n", degfrom((float)lAZ-(float)xStep*(float)radStep), degfrom((float)lALT-(float)yStep*(float)radStep));

            printf("Vals:    xStep:%d      xLeft%d\r\n", *lStep_ptr, lToDo);
            printf("Vals:    yStep:%d      yLeft%d\r\n", *hStep_ptr, hToDo);
    }
    
    float degfrom(float x){
      return x * ((double)180/pi);
    }
    float radfrom(float x){
      return x * (pi/(double)180);
    }    