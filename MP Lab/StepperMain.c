#include <24HJ128GP502.h>
//#define PORT_S PIN_B3

#define control 0x7C
#define command 0xFE

#if !defined(__PCD__)
#error This example will only compile for the PCD 24 bit compiler
#endif

#fuses HS,NOWDT,noprotect
//#fuses FRC,FRC_PLL,NOWDT,NOPROTECT,
#use delay(clock=7350000)                   //7.212Mhz is apparently optimal for 0% error with baud 9600. 

//#use delay(crystal=32mhz)

#use rs232(baud=9600, RCV=PIN_B13, stream=GPSMODULE, ERRORS)
#use rs232(baud=9600, XMIT=PIN_B14, RCV=PIN_B15, stream=PC, ERRORS,parity=N,bits=8)
#use rs232(baud=9600, XMIT=PIN_B12, stream=LCD, ERRORS,parity=N,bits=8)

//     deg     rad
//     360     2pi 
//     180     pi
//     90      2/pi
//     0       0

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
//float pi = 3.141592654;

// Stepper stuff

float radfrom(float x);
float degfrom(float x);

int d = 1;
float radStep = 0.0002556634;
unsigned int HalfStep[8] = {0x1, 0x3, 0x2, 0x6, 0x4, 0xC, 0x8, 0x9};

void twoStep(float lALT, float LAZ);

int xStep = 0;
int yStep = 0;

void SetupLCD();

// Math stuff

float RA = 0; //4.370740779;
float DEC = 0; //0.636463406;

float LAT = 52.5; //0.916297857;
float LNG = -1.9166667; //-0.033452145;

float LST = 0;
float HA = 0; //0.949155775;

float ALT = 0;
float AZ = 0;

int year = 2016;
int month = 03;
int day = 6;

int hour = 23;
int minute = 10;
int second = 0;

double deciDay = 0;
double J2K = 0;

float deciHr = 23.16667;

// Starr functions
float32 Altitude(float32 lDEC, float32 lLAT, float32 lHA);
float Azimuth(float lDEC, float lALT, float lLAT);

float radfrom(float x);
float degfrom(float x);

float HourAngle(float lLST, float lRA);

float LocalSdrTime(float lday, float lLNG, int lUT);

float64 J2Kfrom(int y, int m, int d, float t);

float decimalDay(int h, int m, int s);

float UTdecimlHours(char h, char m, char s);

void LingLang ( float starDec, float starRa );

// Star Stuff 
//char UMiStars[7][10] = {
//	"Polaris","Yildun","e UMi","z UMi","Kochab","Pherkad","n UMi"
//};

typedef struct {
    char name[20];
    float ra;
    float dec;
    float48 dist;
} astroObject;

astroObject UMi[7];
astroObject UMa[18];
astroObject Leo[9];

void starSetup();

void LCDrefresh();

void main() {
    output_b(0);
    fprintf(LCD,"Populating Stars...");
    starSetup();
    delay_ms(200);
    fputc(command, LCD);
    fputc(0x01, LCD);
    fprintf(PC,"n\rPopulating Stars...\n\r");
    
    //    while(1){
    //        fputc(command, LCD);
    //        fputc('r', LCD); 
    //    }
    
    while (1) {
        for (int i = 0; i < 7; i++) {
            fprintf(PC,"i incrament: %d\n\r",i);
            LingLang(UMi[i].dec,UMi[i].ra);
        }
        //        LCDrefresh();
        //        
        //        fprintf(LCD,"Ursa Minor");
        //        LCDrefresh();
        //        for ( int i = 0; i < 7; i++){
        //            fprintf(LCD,UMi[i].name);
        ////            LCDrefresh();
        //            fputc(command, LCD);
        //            fputc(0xC0, LCD);
        //            fprintf(LCD,"Dec%f RA%f",UMi[i].dec,UMi[i].ra);
        //            LCDrefresh();
        //            
        //            J2K = J2Kfrom( year, month, day, deciDay);
        //            LST = LocalSdrTime(J2K,LNG,deciDay);
        //            HA = HourAngle(LST,UMi[i].ra);
        //            ALT = Altitude (UMi[i].dec, LAT, HA);
        //            AZ = Azimuth (UMi[i].dec, ALT, LAT);
        //            
        //            fprintf(LCD,"J2K:%f",J2K);
        //            fputc(command, LCD);
        //            fputc(0xC0, LCD);            
        //            fprintf(LCD,"LST:%f",LST);
        //            delay_ms(5000);
        //            LCDrefresh();
        //
        //            fprintf(LCD,"HA:%f",HA);
        //            delay_ms(5000);
        //            LCDrefresh();
        //            
        //            fprintf(LCD,"ALT:%f",degfrom(ALT));
        //            fputc(command, LCD);
        //            fputc(0xC0, LCD);            
        //            fprintf(LCD,"AZ:%f",degfrom(AZ));
        //            delay_ms(5000);
        //            LCDrefresh();
        //            
        //        }
        //        
        //        
        //        fprintf(LCD,"Ursa Majoris");
        //        LCDrefresh();      
        //        for ( int y = 0; y < 18; y++){
        //            fprintf(LCD,UMa[y].name);
        //            fputc(command, LCD);
        //            fputc(0xC0, LCD);
        //            fprintf(LCD,"Dec%f RA%f",UMa[y].dec,UMa[y].ra);
        //            LCDrefresh();
        //
        //        }
        //        fprintf(LCD,"Leo");
        //        for ( int e = 0; e < 9; e++){
        //            fprintf(LCD,Leo[e].name);
        //            fputc(command, LCD);
        //            fputc(0xC0, LCD);
        //            fprintf(LCD,"Dec%f RA%f",Leo[e].dec,Leo[e].ra);
        //            LCDrefresh();
        //
        //        }

        //        LCDrefresh();



        //        twoStep(0,0);
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

//  Stepper motor function
//      Takes Input as radian, These are the ALT and AZ values
//      It will convert this input to the steps it needs to do based on its current position

void twoStep(float lALT, float lAZ) { // ALT == y    AZ == x  in Rad and what the new cordinates need to be
    // We need to convert the Rad value to step equivilent
    float yStepsTobe = (float) lALT / (float) radStep; // ALT we have 0 - pi/2 rad at 6144step  360/(5.625/64) = 4096 steps per 2pi; Gear ratio of 10:60 -> (60/4)/10 = 1.5 turns for 2/pi rad 4096*1.5 = 6144 steps for 90 deg or 2/pirad 
    float xStepsTobe = (float) lAZ / (float) radStep; // AZ  we have 0 - 2pi rad at (ALTsteps*4)       // at this step speed one revolution should take 24576ms or 24.6 seconds this means the longest if shoul take to a point the in the sky is 12.3 seconds

    // Explenation
    // lAZ/((2*pi)/(6144*4)) = 0.00025566346464760687161968126491532
    // Lets convert the input radien and the current psition form step values to find the diffrence. 
    // max stp num for Alt is: 6144             6144/(pi/2) = 15645.56 So Alt Has 1
    // max stp num for AZ  is: 6144*4 = 24576
    // Now we need to find the step requiered to get there
    int yToDo = yStepsTobe - yStep;
    int xToDo = xStepsTobe - xStep;

    // Report printfs 
    printf("AZ : %f     xStepsTobe: %f\r\n", lAZ, xStepsTobe);
    printf("ALT: %f     yStepsTobe: %f\r\n", lALT, yStepsTobe);

    printf("xStep: %d xToDO: %d\r\n", xStep, xToDo);
    printf("yStep: %d yToDo: %d\r\n", yStep, yToDo);

    //printf("Mis Vals;                         dif:%f yToDo:%d xToDo:%d \n",difs,yToDo,xToDo);    

    int yDir = 1;
    if (yToDo < 0) yDir = -1; //Assigning a value to know what direction to move the motors
    int xDir = 1;
    if (xToDo < 0) xDir = -1; //Using the nested if statment. 



    int hToDo;
    int lToDo;

    signed int *hStep_ptr;
    signed int *lStep_ptr;

    int hDir;
    int lDir;

    char hSft;
    char lSft;

    if (yToDo == 0 && xToDo == 0) { // This is to prevent goinginto the loop if no movement happens. To stop in crashing inside the loop. 
        printf("ERORO: You told me to go noWHERE\r\n");
        return;
    }

    if (abs(yToDo) > abs(xToDo)) { // This is where ALT is larger ALT is Y 

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
    else if (abs(xToDo) > abs(yToDo)) { // This is where AZ is larger AZ is X 

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
    else // intended for the cases where the x and y change values are the same so we run both at once
        while (abs(yToDo) > 0) {
            yToDo = abs(yToDo);
            xToDo = abs(xToDo);
            output_b(HalfStep[yStep & 0x7] << 4 | HalfStep[xStep & 0x7]);
            delay_ms(d);
            yToDo--;
            xToDo--;
            xStep += xDir;
            yStep += yDir;
            output_b(0x000);
        }

    if (xToDo == 0 || yToDo == 0) { // If one motor moves and the other does not, to avoid devision by 0    
        printf("in the one move fore loop\n\r");
        hDir = xDir;
        lDir = yDir;
        yToDo = abs(yToDo);
        xToDo = abs(xToDo);
        while (hToDo > 0) {
            output_b(HalfStep[*hStep_ptr & 0x7] << lSft);
            *hStep_ptr += hDir;
            hToDo--;
            delay_ms(d);
        }
        output_b(0x000);
    }

    ///////////////////////// Main Stepper Code Loop ///////////////////////////
    while (hToDo > lToDo) {
        // Debugging prints
        // printf("hToDo is %d\n", hToDo);
        // printf("lToDo is %d\n", lToDo);
        // printf("yDir  %d  xDir: %d\n",yDir,xDir);
        //

        int mod = (int) hToDo / (hToDo % lToDo);

        while (lToDo > 0) {
            // Debugging prints
            //printf("while loop hToDo is %d\n", hToDo);
            //printf("while loop lToDo is %d\n", lToDo);
            // Print statment for accurecy checks
            //printf("%d,%d\r\n",lToDo,hToDo );
            //    
            for (unsigned int i = (unsigned int) ((hToDo / (lToDo)) + 0.5); i > 0; i--) { // Inside the x itteration we have the more frequent y itteration
                // Debugging prints
                //                        printf("hTodo is: %d\r\n", hToDo);
                //                        printf("Fast loop -> ");


                //
                //                    printf("hTodo is: %d\r\n", hToDo);
                //                    printf("hStep_ptr is: %d\r\n", *hStep_ptr);
                //                    printf("hDir  is: %d\r\n", hDir);
                //                    printf("Step# is: %d\r\n", HalfStep[*hStep_ptr&0x7]<<*lSft);

                output_b(HalfStep[*hStep_ptr & 0x7] << lSft);
                *hStep_ptr += hDir;
                hToDo--;
                delay_ms(d);
                //printf("%ld,%ld \r\n",hToDo,lToDo);
                if (hToDo % lToDo != 0 && hToDo % (mod /*(int) 300/(hToDo%lToDo)*/) == 0) {
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
            output_b(HalfStep[*lStep_ptr & 0x7] << hSft); //| HalfStep[yStep&0x7]<<lSft);

            *lStep_ptr += lDir;
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

float degfrom(float x) {
    return x * ((double) 180 / PI);
}

float radfrom(float x) {
    return x * (PI / (double) 180);
}


//:-----------~|  LCD functions   |~-----------://

void SetupLCD() {
#use  rs232(baud=9600, xmit=PIN_B12, STREAM=LCD) //Define BAUD, transmit pin and Stream ID
    fputc(control, LCD); //Set to 9600 BAUD
    fputc('r', LCD); // 
    fputc(control, LCD); //Set 16 character width 
    fputc(4, LCD); // 
    fputc(control, LCD); //Set 2 line LCD 
    fputc(6, LCD); //
    fputc(control, LCD); //Set to brightest display 
    fputc(157, LCD); // 
    puts("\rDisplay Ready\r\n", LCD); //Print Message 
    delay_ms(1000); //Wait 5 seconds 
    fputc(command, LCD); //Clear display 
    fputc(1, LCD); // 
    delay_ms(100); // need delay after backlight change
}

void LCDrefresh() {
    delay_ms(800);
    fputc(command, LCD);
    fputc(0x01, LCD); //
    delay_ms(50);
}




//:-----------~|  STAR FUCNTIONS   |~-----------://

void LingLang ( float starDec, float starRa ){
    //for (int i = 0; i < 7; i++) {
        //fprintf(PC,"%1.9f\n\r",deciDay);
        
        // Time conversions
        deciDay = decimalDay(hour, minute, second);
        deciHr = UTdecimlHours(hour, minute, second);
        J2K = J2Kfrom(year, month, day, deciDay);
        
        // Time relative to location
        LST = LocalSdrTime(J2K, LNG, deciHr);
        HA = HourAngle(LST, starRa);
        
        fprintf(PC, "LST:%5.5f\n\rHA :%9.5f\n\r",LST, HA);
        fprintf(PC, "deciHr:%5.5f\n\rdeciDay:%9.5f\n\r",deciHr, deciDay);
        fprintf(PC, "LST:%5.5f\n\rHA:%9.5f\n\r",LST, HA);
        fprintf(PC, "ALT:%5.5f\n\rAZ:%9.5f\n\r", ALT, AZ);

        // Coordinates calculations 
        ALT = Altitude(starDec, LAT, HA);
        AZ = Azimuth(starDec, ALT, LAT);

        fprintf(PC, "J2K:%1.9f\n\r", J2K);
        fprintf(PC, "DEC:%9.5f\r\nRA:%9.5f\n\r", UMi[0].dec, starRa);
        fprintf(PC, "ALT:%5.5f\n\rAZ:%9.5f\n\r", ALT, AZ);
        ALT = 0;
        AZ = 0;
        delay_ms(3000);
    //}

}

float32 Altitude(float32 lDEC, float32 lLAT, float32 lHA) {
    float32 x = (float32)sin(lDEC) * sin(lLAT) + cos(DEC) * (float32)cos(lLAT) * cos(lHA);
    return (float32) asin(x);
}

float Azimuth(float lDEC, float lALT, float lLAT) {
    float x = (float)(sin(lDEC) - sin(lALT) * sin(lLAT)) / ((float)cos(lALT) * cos(lLAT));
    return (float) acos(x);
}

float HourAngle(float lLST, float lRA) {
    return ((float) lLST - lRA);
}

float LocalSdrTime(float lJ2K, float lLNG, int ldeciHr) {
    float  lLST = 100.46 + 0.985647 * (float)lJ2K + lLNG + 15 * (float)ldeciHr;
    char dirLST = 1;
    
    while (lLST < 0){
        lLST += 360;
    }
    while (lLST >= 360){
        lLST -= 360;
    }
      
    return lLST; 
    
    //lLST -= 360;return 100.46 + 0.985647 * (float)lJ2K + lLNG + 15 * (float)ldeciHr;
}

float UTdecimlHours(char h, char m, char s){
    return (h + (float)(m+(s/60))/60);
}

// J2K is the Julian time since 12:00 1 jan 2000 of the (cregorian calender) referred to Epoh 2000.
//      The stellar data is adjusted for Epoh 2000; J2000 == J 2451545.0
//      The J date conversion formula is from UNSO: http://aa.usno.navy.mil/faq/docs/JD_Formula.php
//      The values form this equation need to be adjusted for J2000.
//      The decimal day is also added to the result to give a J200 time.
// Checking J2K
//      J2K*60*60*24 == Julian seconds
//      ((Julian seconds/60)/60)/24  == J2K
//      Use this for checking : https://nsidc.org/data/icesat/glas-date-conversion-tool/date_convert/ 
// Example 
//      Input:  2016 03 06    18:47:00                   >>   5909.282638847    
//      Check: 5909.282638847*60*60*24 = 510562019.996   >>   2016-03-06    18:46:59.995    

float64 J2Kfrom(int y, int m, int d, float t) {
    unsigned int64 JD = d - 32075 + 1461 * (double) (y + 4800 + (m - 14) / 12) / 4 + 367 * (m - 2 - (m - 14) / 12 * 12) / 12 - 3 * ((y + 4900 + (m - 14) / 12) / 100) / 4;
    fprintf(PC, "JD:%1.9Lu\n\r", JD);
    return (JD - (float64) 2451545.0 + t - 0.5);
}

float decimalDay(int h, int m, int s) {
    return (h + (float)(m+(s/60))/60)/24;
}

void starSetup() {

    // Ursa Minor
    // Polaris
    strcpy(UMi[0].name, "Polaris");
    UMi[0].ra = 2.52974312;
    UMi[0].dec = 89.26413805;
    UMi[0].dist = 132.2751323;
    //

    // Yildun
    strcpy(UMi[1].name, "Yildun");
    UMi[1].ra = 17.53691588;
    UMi[1].dec = 86.58632924;
    UMi[1].dist = 56.02240896;
    //
    // e UMi
    strcpy(UMi[2].name, "Eps UMi");
    UMi[2].ra = 16.76615597;
    UMi[2].dec = 82.03725071;
    UMi[2].dist = 106.2699256;
    //
    // z UMi
    strcpy(UMi[3].name, "Zet UMi");
    UMi[3].ra = 15.73429554;
    UMi[3].dec = 77.79449901;
    UMi[3].dist = 115.2073733;
    //
    // Kochab
    strcpy(UMi[4].name, "Kochab");
    UMi[4].ra = 14.84510983;
    UMi[4].dec = 74.15547596;
    UMi[4].dist = 38.77471888;
    //
    // Pherkad
    strcpy(UMi[5].name, "Pherkad");
    UMi[5].ra = 15.34548589;
    UMi[5].dec = 71.83397308;
    UMi[5].dist = 147.275405;
    //
    // n UMi
    strcpy(UMi[6].name, "Eta UMi");
    UMi[6].ra = 16.29180584;
    UMi[6].dec = 75.75470385;
    UMi[6].dist = 29.83293556;
    //
    //  

    // Ursa Major
    // Alkaid
    strcpy(UMa[0].name, "Alkaid");
    UMa[0].ra = 13.79237392;
    UMa[0].dec = 49.31330288;
    UMa[0].dist = 30.87372646;
    //
    // Mizar	
    strcpy(UMa[1].name, "Mizar");
    UMa[1].ra = 13.39872773;
    UMa[1].dec = 54.92541525;
    UMa[1].dist = 23.96357537;
    //
    // Alioth
    strcpy(UMa[2].name, "Alioth");
    UMa[2].ra = 12.9004536;
    UMa[2].dec = 55.95984301;
    UMa[2].dist = 24.81389578;
    //		
    // Megrez			
    strcpy(UMa[3].name, "Megrez");
    UMa[3].ra = 12.25706919;
    UMa[3].dec = 57.03259792;
    UMa[3].dist = 24.96878901;
    //		
    // Phad		
    strcpy(UMa[4].name, "Phad");
    UMa[4].ra = 11.89715035;
    UMa[4].dec = 53.69473296;
    UMa[4].dist = 25.64760195;
    //
    // Merak		
    strcpy(UMa[5].name, "Merak");
    UMa[5].ra = 11.0306641;
    UMa[5].dec = 56.38234478;
    UMa[5].dist = 24.348673;
    //	
    // Dubhe		
    strcpy(UMa[6].name, "Dubhe");
    UMa[6].ra = 11.06217691;
    UMa[6].dec = 61.75111888;
    UMa[6].dist = 37.90750569;
    //	
    // h UMa
    strcpy(UMa[7].name, "23 UMa");
    UMa[7].ra = 9.52543601;
    UMa[7].dec = 63.06179545;
    UMa[7].dist = 23.14814815;
    //	
    // Muscida
    strcpy(UMa[8].name, "Muscida");
    UMa[8].ra = 8.50445282;
    UMa[8].dec = 60.7184311;
    UMa[8].dist = 56.30630631;
    //	
    // u UMa
    strcpy(UMa[9].name, "Ups UMa");
    UMa[9].ra = 9.84991436;
    UMa[9].dec = 59.03910437;
    UMa[9].dist = 35.27336861;
    //	
    // ph UMa
    strcpy(UMa[10].name, "Phi UMa");
    UMa[10].ra = 9.86843337;
    UMa[10].dec = 54.06428574;
    UMa[10].dist = 133.8688086;
    //	
    // th UMa
    strcpy(UMa[11].name, "The UMa");
    UMa[11].ra = 9.54786812;
    UMa[11].dec = 51.67860208;
    UMa[11].dist = 13.48617667;
    //	
    // Talitha
    strcpy(UMa[12].name, "Talitha");
    UMa[12].ra = 8.98689965;
    UMa[12].dec = 48.04234956;
    UMa[12].dist = 14.63700234;
    //	
    // k UMa
    strcpy(UMa[13].name, "Kap UMa");
    UMa[13].ra = 9.06043299;
    UMa[13].dec = 47.15665934;
    UMa[13].dist = 129.7016861;
    //	
    // ch UMa
    strcpy(UMa[14].name, "Chi UMa");
    UMa[14].ra = 11.76753725;
    UMa[14].dec = 47.77933701;
    UMa[14].dist = 60.09615385;
    //	
    // ps UMa
    strcpy(UMa[15].name, "Psi UMa");
    UMa[15].ra = 11.16107206;
    UMa[15].dec = 44.49855337;
    UMa[15].dist = 45.02476362;
    //	
    // l UMa
    strcpy(UMa[16].name, "Lam UMa");
    UMa[16].ra = 10.2849797;
    UMa[16].dec = 42.91446855;
    UMa[16].dist = 41.20313144;
    //	
    // Tania Australis
    strcpy(UMa[17].name, "Tania Australis");
    UMa[17].ra = 10.37216756;
    UMa[17].dec = 41.4994335;
    UMa[17].dist = 76.27765065;
    //
    //
    // Leo
    // Algenubi
    strcpy(Leo[0].name, "Algenubi");
    Leo[0].ra = 9.764195;
    Leo[0].dec = 23.77428;
    Leo[0].dist = 76.86395;
    //
    // Rasalas	
    strcpy(Leo[1].name, "Rasalas");
    Leo[1].ra = 9.879433;
    Leo[1].dec = 26.00708;
    Leo[1].dist = 40.78303;
    //
    // Adhafera
    strcpy(Leo[2].name, "Adhafera");
    Leo[2].ra = 10.27817;
    Leo[2].dec = 23.41733;
    Leo[2].dist = 79.61783;
    //		
    // Algieba			
    strcpy(Leo[3].name, "Algieba");
    Leo[3].ra = 10.33282;
    Leo[3].dec = 19.84186;
    Leo[3].dist = 38.5208;
    //		
    // Zosma		
    strcpy(Leo[4].name, "Zosma");
    Leo[4].ra = 11.23511;
    Leo[4].dec = 20.52403;
    Leo[4].dist = 17.69285;
    //
    // Denebola		
    strcpy(Leo[5].name, "Denebola");
    Leo[5].ra = 11.81774;
    Leo[5].dec = 14.57234;
    Leo[5].dist = 11.09139;
    //	
    // Theta Leo		
    strcpy(Leo[6].name, "Theta Leo");
    Leo[6].ra = 11.23734;
    Leo[6].dec = 15.42976;
    Leo[6].dist = 54.46623;
    //	
    // Regulus
    strcpy(Leo[7].name, "Regulus");
    Leo[7].ra = 10.13957;
    Leo[7].dec = 11.9672;
    Leo[7].dist = 23.75861;
    //	
    // Eta Leo
    strcpy(Leo[8].name, "Eta Leo");
    Leo[8].ra = 10.12221;
    Leo[8].dec = 16.76267;
    Leo[8].dist = 653.5948;
    //	
    // 

    // Special Credit to Nikita ... for helping with managing the Stellar database 
}
