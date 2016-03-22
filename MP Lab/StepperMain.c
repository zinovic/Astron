#include <24HJ128GP502.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

#if !defined(__PCD__)
#error This example will only compile for the PCD 24 bit compiler
#endif
#fuses HS,NOWDT,noprotect
//#fuses FRC,FRC_PLL,NOWDT,NOPROTECT,
#use delay(clock=7350000)//This odd value is because 8mzh does not give good RS232 readings at 9600 baud. No idea why

#use rs232(baud=9600, RCV=PIN_B10, stream=GPSMODULE, ERRORS)
#use rs232(baud=9600, XMIT=PIN_B11, RCV=PIN_B10, stream=PC, ERRORS,parity=N,bits=8)
#use rs232(baud=9600, XMIT=PIN_B6, stream=LCD, ERRORS,parity=N,bits=8)

//Motors
// Delay for step timing; Ratio for how many radians one step is equal to; The step sequence in hex numbers
int xStep = 0;
int yStep = 0;
int d = 1;
float radStep = 0.0002556634;
unsigned int HalfStep[8] = {0x1, 0x3, 0x2, 0x6, 0x4, 0xC, 0x8, 0x9};
void twoStep(float lALT, float LAZ);

// For converting between radian and degrees 
//     deg     rad
//     360     2pi 
//     180     pi
//     90      2/pi
//     0       0
float radfrom(float x);
float degfrom(float x);

//Star calculation
// Theory 
//  Using global variables for the components needed to calculate the star location. 
//  This is so they can be accessed by any function elsewhere in the code.
//  These global variables will change based on what star is being pointed to. 
//  Some of these have values accessed by default this is for testing purposes, they be set is code as is needed
//  The detailed explanation of how ALT and AZ is calculated for each star will not fit here, visit the link for that:
//      http://www.stargazing.net/kepler/altaz.html
float RA = 0; //4.370740779;    
float DEC = 0; //0.636463406;
float LAT = 51.5; //0.916297857;
float LNG = -0.13 ; //-0.033452145;
float LST = 0;
float HA = 0; //0.949155775;
float64 ALT = 0;
float64 AZ = 0;
int year = 2016;
int month = 03;
int day = 6;
int hour = 23;
int minute = 10;
int second = 0;
double deciDay = 0;
double J2K = 0;
float deciHr = 23.16667;

float64 Altitude(float64 lDEC, float64 lLAT, float64 lHA);
float64 Azimuth(float64 lDEC, float64 lALT, float64 lLAT);
float radfrom(float x);
float degfrom(float x);
float HourAngle(float lLST, float lRA);
float LocalSdrTime(float lday, float lLNG, float ldeciHr);
float64 J2Kfrom(int y, int m, int d, float t);
float decimalDay(int h, int m, int s);
float UTdecimlHours(char h, char m, char s);
void starPoint(int AU );
void StellarCalulation ( float starDec, float starRa );

// Star data types 
//  Using a struct to store the data for one star. Name; ra,dec for the position at J2000; Distance as piece of interesting data tisplay to the usero d
//  An array is then initialised with this struc to form constellations. These arrays are populated once at the beginning of the program. 
typedef struct {
    char name[20];
    float ra;
    float dec;
    float dist;
} astroObject;
astroObject UMi[7];
astroObject UMa[18];
astroObject Leo[9];
void starSetup();

#define control 0x7C
#define command 0xFE

// LCD
// Basic control function for the LCD. 
void LCDrefresh(int16 lDelay);
void LCDnewline();

// Interrupts 
//  These proved difficult to get working, currently only the fixed external interrupt on PIN_B7 works;
//  The pic offers no interrupt interrupt on change as far as was found, there is no INT_RB or equivalent in the header file. 
//  The pic offers 2 more programmable external interrupt pins however no documentation was found on how to associate the pins to IN_EX1 etc.
#INT_EXT0       //Set external interrupt on pin RB0 
void EXT_ISR(void) {
#USE fast_io(B)             // To set port B to low, In case its paused mid motor cycle to avoide over heating the motors
    set_tris_b(0x0FF0);
    output_b(0x0000);
#USE standard_io(B)
    LCDrefresh(50);
    fprintf(LCD, "PAUSED click to continue");
    delay_ms(500);

    disable_interrupts(INT_EXT0);
    while (!input(PIN_B7)) {
        //fprintf(LCD,"In while");
        //just wait for it to be clicked again 
    }
    enable_interrupts( INT_EXT0 );
    LCDrefresh(50);
}

// RTCC
//  Sets the internal 
void timeSet();
int8 get_number();
void set_clock(rtc_time_t &date_time);
void InitTime();
rtc_time_t tmp;
rtc_time_t write_clock, read_clock;
void main() {
    //#USE fast_io(B)
    //set_tris_b(0x0FF0);
    
    //setup_timer1(TMR_INTERNAL,TMR_DIV_BY_256);
    //setup_adc_ports(NO_ANALOGS); 
    //enable_interrupts(INT_RB); 

    //Interrupt enabels  
    
// LCD Splash screen settings
//LCDrefresh(800);
//fprintf(LCD,"Astron                   rev 0.1");
//delay_ms(500);
//fputc(0x7C, LCD);
//fputc(10, LCD);
    output_b(0x0000);
    delay_ms(500);    
    LCDrefresh(800);
    // LCD and STAR function setup
    fprintf(LCD,"populating stars");
    starSetup();
    LCDrefresh(600);
    enable_interrupts( INT_EXT0 );
    ext_int_edge(L_TO_H);
    //set_pullup(TRUE, PIN_B7); 
    enable_interrupts(INTR_GLOBAL);
    fprintf(LCD,"interrupts");
    LCDrefresh(600);
    //RTC setup
    setup_rtc(RTC_ENABLE,0);         //enables internal RTCC
    //set_clock(write_clock);
    rtc_write(&write_clock);           //writes new clock setting to RTCC
    setup_adc_ports(sAN0);
    setup_adc(ADC_CLOCK_INTERNAL);
    set_adc_channel(0);
    setup_rtc(RTC_ENABLE, 0);
    InitTime();
    fprintf(LCD, "clock set");
    LCDrefresh(600);
    set_pullup(TRUE,PIN_A0);
    set_pullup(TRUE,PIN_A1);
    set_pullup(TRUE,PIN_A2);

    // Alt axis zeroing
    unsigned int16 ALTzero = 62500;    
    while (input(PIN_A1)){
        
        output_b(HalfStep[ALTzero & 0x7] << 12);
        delay_ms(1);
        ALTzero --;
    }
    // manual calibration
    //  Points at Maizar adn wait for button push form user before resetting
    disable_interrupts(INT_EXT0);
    LCDrefresh(500);
    fprintf(LCD," Point Maizar   ");
    LCDnewline();
    fprintf(LCD," click confirm ");
#USE fast_io(B)
            set_tris_b (0x0FF0);
            StellarCalulation(UMa[1].dec,UMa[1].ra);
            twoStep(ALT,AZ);
#USE standard_io(B)
    LCDrefresh(500);
    fprintf(LCD," Point Maizar    ");
    LCDnewline();
    fprintf(LCD," click confirm ");
    while (!input(PIN_B7)) {
    }
    enable_interrupts( INT_EXT0 );
    twoStep(0,0);
    LCDrefresh(50);
    
    // Main while loop    
    while (1) {                       
        rtc_read(&tmp); 
        rtc_read(&read_clock);        
        LCDrefresh(800);
        fprintf(LCD,"%02u/%02u/20%02u",read_clock.tm_mon,read_clock.tm_mday,read_clock.tm_year);
        LCDnewline();
        fprintf(LCD,"%02u:%02u:%02u",read_clock.tm_hour,read_clock.tm_min,read_clock.tm_sec);
        
        year = read_clock.tm_year;
        month = read_clock.tm_mon;       
        day = read_clock.tm_mday;
        hour = read_clock.tm_hour;
        minute = read_clock.tm_min;
        second = read_clock.tm_sec;
        LCDrefresh(4000);
        // Ursa Majoris
        for (int e = 0; e < 18; e++) {
            LCDrefresh(800);
            fprintf(LCD,"  ");
            StellarCalulation(UMa[e].dec,UMa[e].ra);
            fprintf(LCD,UMa[e].name);
            LCDnewline();
            fprintf(LCD,"ALT:%2.0f AZ:%2.0f    ",degfrom(ALT),degfrom(AZ));           
#USE fast_io(B)
            set_tris_b (0x0FF0);
            twoStep(ALT,AZ);
#USE standard_io(B)
            LCDrefresh(800);
        }
        // Ursa Minor 
        for (int o = 0; o < 7; o++) {
            LCDrefresh(800);
            fprintf(LCD,"  ");
            StellarCalulation(UMi[o].dec,UMi[o].ra);
            fprintf(LCD,UMi[o].name);
            LCDnewline();
            fprintf(LCD,"ALT:%2.0f AZ:%2.0f    ",degfrom(ALT),degfrom(AZ));
#USE fast_io(B)
            set_tris_b (0x0FF0);
            twoStep(ALT,AZ);
#USE standard_io(B)
            LCDrefresh(800);
        }
        // Leo
        for (int i = 0; i < 9; i++) {
            timeSet();
            LCDrefresh(800);
            fprintf(LCD,"  ");
            StellarCalulation(Leo[i].dec,Leo[i].ra);
            fprintf(LCD,Leo[i].name);
            LCDnewline();
            fprintf(LCD,"ALT:%2.0f AZ:%2.0f    ",degfrom(ALT),degfrom(AZ));
#USE fast_io(B)
            set_tris_b (0x0FF0);
            twoStep(ALT,AZ);
#USE standard_io(B)
            LCDrefresh(5000);
        }       
    }
}

//:-----------~|  Motor functions   |~-----------://
void twoStep(float lALT, float lAZ) { // ALT == y    AZ == x  in Rad and what the new cordinates need to be
    // We need to convert the Rad value to step equivilent
    // ALT we have 0 - pi/2 rad at 6144step  360/(5.625/64) = 4096 steps per 2pi; Gear ratio of 10:60 -> (60/4)/10 = 1.5 turns for 2/pi rad 4096*1.5 = 6144 steps for 90 deg or 2/pi rad 
    // AZ  we have 0 - 2pi rad at (ALTsteps*4)       
    // at this step speed one revolution should take 24576ms or 24.6 seconds this means the longest it should take to a point the in the sky is 12.3 seconds
    float yStepsTobe = (float) lALT / (float) radStep; 
    float xStepsTobe = (float) lAZ / (float) radStep; 
    // Explenation
    // lAZ/((2*pi)/(6144*4)) = 0.00025566346464760687161968126491532
    // Lets convert the input radien and the current position form step values to find the diffrence. 
    // max stp num for Alt is: 6144             6144/(pi/2) = 15645.56 So Alt Has 1
    // max stp num for AZ  is: 6144*4 = 24576
    // Now we need to find the step requiered to get there
    int yToDo = yStepsTobe - yStep;
    int xToDo = xStepsTobe - xStep;
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

    if (yToDo == 0 && xToDo == 0) { // This is to prevent goinginto the loop if no movement happens. To stop a devision by 0 and crashing inside the loop. 
        fprintf(PC, "ERORO: You told me to go noWHERE\r\n");
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
        lSft = 12;  
    }
    else if (abs(xToDo) > abs(yToDo)) { // This is where AZ is larger AZ is X 
        hToDo = abs(xToDo);
        lToDo = abs(yToDo);
        hStep_ptr = &xStep;
        lStep_ptr = &yStep;
        hDir = xDir;
        lDir = yDir;
        hSft = 12;
        lSft = 0;
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
        int mod = (int) hToDo / (hToDo % lToDo);
        while (lToDo > 0) {  
            // Limit switches code This will happen every 
//            if (!input(PIN_A0)) {
//                LCDrefresh(10);
//                fprintf(LCD, "Negative Limit triggered");
//                LCDrefresh(900);
//
//                if (abs(yToDo) > abs(xToDo)) {
//                    lToDo = 1;
//                    output_b(HalfStep[*hStep_ptr & 0x7] << lSft);
//                    *hStep_ptr += hDir;
//                    hToDo--;
//                    delay_ms(d);
//                }
//
//                if (abs(xToDo) > abs(yToDo)) {
//                    hToDo = 1;
//                    output_b(HalfStep[*lStep_ptr & 0x7] << hSft);
//                    *lStep_ptr += lDir;
//                    lToDo--;
//                    delay_ms(d);
//                }
//                break;
//            }
//            if (!input(PIN_A1)) {
//                LCDrefresh(10);
//                fprintf(LCD, "Positive Limit triggered");
//                LCDrefresh(900);
//
//                if (abs(yToDo) > abs(xToDo)) {
//                    lToDo = 1;
//                    output_b(HalfStep[*hStep_ptr & 0x7] << lSft);
//                    *hStep_ptr += hDir;
//                    hToDo--;
//                    delay_ms(d);
//                }
//
//                if (abs(xToDo) > abs(yToDo)) {
//                    hToDo = 1;
//                    output_b(HalfStep[*lStep_ptr & 0x7] << hSft);
//                    *lStep_ptr += lDir;
//                    lToDo--;
//                    delay_ms(d);
//                }
//                break;
//            }
            
            for (unsigned int i = (unsigned int) ((hToDo / (lToDo)) + 0.5); i > 0; i--) { // Inside the x itteration we have the more frequent y itteration
                output_b(HalfStep[*hStep_ptr & 0x7] << lSft);
                *hStep_ptr += hDir;
                hToDo--;
                delay_ms(d);
                // This is where the extra steps are added. 
                if (hToDo % lToDo != 0 && hToDo % (mod) == 0) {
                    i++;
                }
            }
            output_b(HalfStep[*lStep_ptr & 0x7] << hSft); //| HalfStep[yStep&0x7]<<lSft);
            *lStep_ptr += lDir;
            lToDo--;
            delay_ms(d);
        }
        output_b(0x000);    // setting pins to low to avoid over heating the motors and wasting power 
    }
}

float degfrom(float x) {
    return x * ((float) 180 / PI);
}
float radfrom(float x) {
    return x * (PI / (float) 180);
}

//:-----------~|  LCD functions   |~-----------://
// LCD Splash screen setting
//LCDrefresh(800);
//fprintf(LCD,"Astron                   rev 0.1");
//delay_ms(500);
//fputc(0x7C, LCD);
//fputc(10, LCD);
void LCDnewline(){
    fputc(command, LCD);
    fputc(0xC0, LCD);
}
void LCDrefresh(int16 lDelay) {
    delay_ms(lDelay);
    //fprintf(LCD," ");
    fputc(command, LCD);
    fputc(0x01, LCD); //
    delay_ms(50);
}

//:-----------~|  TIME functions   |~-----------://
// RTCC real time clock and calender
//  This pic offers a dedicated module for this function. 
//  It requires  an external clock 32.768 crystal  
int8 get_number(){
  char first,second;

  do {
    first=getc();
  } while ((first<'0') || (first>'9'));
  putc(first);
  first-='0';

  do {
    second=getc();
  } while (((second<'0') || (second>'9')) && (second!='\r'));
  putc(second);

  if(second=='\r')
    return(first);
  else
    return((first*10)+(second-'0'));
}
void set_clock(rtc_time_t &date_time) {
    printf("\r\nPress ENTER after 1 digit answers.");
    printf("\r\nYear 20: ");
    date_time.tm_year = get_number();
    printf("\r\nMonth: ");
    date_time.tm_mon = get_number();
    printf("\r\nDay: ");
    date_time.tm_mday = get_number();
    printf("\r\nWeekday 1-7: ");
    date_time.tm_wday = get_number();
    printf("\r\nHour: ");
    date_time.tm_hour = get_number();
    printf("\r\nMin: ");
    date_time.tm_min = get_number();
    date_time.tm_sec = 0;
    printf("\r\n\n");
}
void InitTime() {
    rtc_time_t tmp;
    tmp.tm_year = 16;
    tmp.tm_mday = 18;
    tmp.tm_mon = 3;
    tmp.tm_hour = 22;
    tmp.tm_wday = 5;
    tmp.tm_sec = 0;
    tmp.tm_min = 10;
    rtc_write(&tmp);
}
// assign time   
void timeSet(){
        rtc_read(&tmp); 
        rtc_read(&read_clock);        //reads clock value from RTCC
        year = 2000+read_clock.tm_year;
        month = read_clock.tm_mon;       
        day = read_clock.tm_mday;
        hour = read_clock.tm_hour;
        minute = read_clock.tm_min;
        second = read_clock.tm_sec;
        LCDnewline();        
        LCDrefresh(800);
}

//:-----------~|  STAR FUCNTIONS   |~-----------://
void starPoint(int * AU ){
// This is intended to be a function that takes a pointer to a constellation array as its input at cycles through the stars in it
}
void StellarCalulation ( float starDec, float starRa ){
    starDec = radfrom(starDec);
    starRa = radfrom(starRa);
    // Time conversions   
    deciDay = decimalDay(hour, minute, second);
    deciHr = UTdecimlHours(hour, minute, second);
    J2K = J2Kfrom(year, month, day, deciDay);
    // Time relative to location
    LST = LocalSdrTime(J2K, LNG, deciHr);
    HA = HourAngle(LST, degfrom(starRa));
    // Coordinates calculations 
    float64 radLAT = radfrom(LAT);
    float64 radHA = radfrom(HA);
    ALT = Altitude(starDec, radLAT, radHA);
    AZ = Azimuth(starDec, ALT, radLAT);
}
float64 Altitude(float64 lDEC, float64 lLAT, float64 lHA) {
    float64 x = (float64)(sin(lDEC)*sin(lLAT))+(float64)(cos(lDEC)*cos(lLAT)* cos(lHA));
    return (float64) asin(x);
}
float64 Azimuth(float64 lDEC, float64 lALT, float64 lLAT) {
    float64 x = (sin(lDEC) - sin(lALT) * sin(lLAT)) / (cos(lALT) * cos(lLAT));
    return (float64) acos(x);
}
float HourAngle(float lLST, float lRA) {    
    float lHA = (lLST - lRA);
    //This is to keep the values between 0 and 360 
    while (lHA < 0) {       
        lLST += 360;
    }
    while (lHA > 360) {
        lLST -= 360;
    }
    return lHA;
}
float LocalSdrTime(float lJ2K, float lLNG, float ldeciHr) {
    lLNG = (float) radfrom(lLNG);
    float lLST = 100.46 + (float) 0.985647 * (float) lJ2K + lLNG + 15 * (float) ldeciHr;
    while (lLST < 0 || lLST > 360 ){
        //This is to keep the values between 0 and 360
        while (lLST < 0) {
            lLST += 360;
        }
        while (lLST > 360){
            lLST -= 360;
        }
    }  
    return lLST; 
}
float UTdecimlHours(char h, char m, char s){
    return (h + (float)(m+(s/60))/60);
}
// J2K is the Julian time since 12:00 1 jan 2000 of the (gregorian calender) referred to Epoh 2000.
//      The stellar data is adjusted for Epoh 2000; J2000 == J 2451545.0
//      The J date conversion formula is from UNSO: http://aa.usno.navy.mil/faq/docs/JD_Formula.php
//      The values form this equation need to be adjusted for J2000.
//      The decimal day is also added to the result to give a complete J2000 time.
// Checking J2K
//      J2K*60*60*24 == Julian seconds
//      ((Julian seconds/60)/60)/24  == J2K
//      Use this for checking : https://nsidc.org/data/icesat/glas-date-conversion-tool/date_convert/ 
// Example 
//      Input:  2016 03 06    18:47:00 >>  this function >>   5909.282638847    
//      Check: 5909.282638847*60*60*24 = 510562019.996   >>   2016-03-06    18:46:59.995
float64 J2Kfrom(int y, int m, int d, float t) {
    unsigned int64 JD = d - 32075 + 1461 * (double) (y + 4800 + (m - 14) / 12) / 4 + 367 * (m - 2 - (m - 14) / 12 * 12) / 12 - 3 * ((y + 4900 + (m - 14) / 12) / 100) / 4;
    return (JD - (float64) 2451545.0 + t - 0.5);
}
float decimalDay(int h, int m, int s) {
    return (h + (float) (m + (s / 60)) / 60) / 24;
}
void starSetup() {
    // Ursa Minor
    // Polaris
    strcpy(UMi[0].name, "Polaris");
    UMi[0].ra = 2.52974312;     // adjusted for J2000 in degrees 
    UMi[0].dec = 89.26413805;
    UMi[0].dist = 132.2751323;  // in light years
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