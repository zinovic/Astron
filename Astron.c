#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>

#include <math.h>

float RA = 0;   //4.370740779;
float DEC = 0;  //0.636463406;

float LAT = 0.916297857;
float LONG = -0.033452145;

float LST = 0;
float HA = 0.949155775;

float ALT = 0;
float AZ = 0;

int year = 1998;
int month = 8;
int day = 10;

int hour = 23;
int minute = 10;
int second = 0;

float UTC = 0;
float J2k = 0;

float pi = 3.141592654;

#define print_int(x) printf("var " #x "= %d\n", x)          //  print_int(a); 
#define print_char(x) printf("var " #x "= %c\n", x)         //  print_char(letter);
#define print_string(x) printf("var " #x "= %s\n", x)       //  print_string(phrase);

//#define pi atan(1)*4;


//:-----------~|Function Defs|~-----------://

float Altitude (char lDEC, char lLAT, char lHA);
float Azimuth (char lDEC, char lALT, char lLAT);

float radfrom(float x);
float degfrom(float x);

float J2kfrom( int y, int m, int d, float t);
float decimalday(int h, int m, int s);

int inputfloat (char x);

void DumpPrint ();

//:-----------~|     MAIN     |~-----------://

int main ()//int argc, char *argv[])
{
  //pi = atan(1)*4;
  
  //printf("RA in degrees:");    scanf("%f", &RA);






  /* ----------------- OLD CODE -------------------
  */
  printf("Please enter the cordinates of the star\n");
  printf("RA:");    scanf("%f", &RA);
  printf("DEC:");   scanf("%f", &DEC);

  printf("Now enter your GPS cordinates\n");
  printf("LAT:");    scanf("%f", &LAT);
  printf("LONG:");   scanf("%f", &LONG);

  printf("The date of the observation in internatinal date format yyyymm\n");
  scanf("%d""%d""%d", &year, &month, &day);

  printf("Finally the time of the observation hh mm ss\n");
  scanf("%d""%d""%d", &hour, &minute, &second);

  ALT = Altitude(DEC,LAT,HA);
  AZ = Azimuth(DEC,ALT,LAT);
  
  UTC = decimalday(hour,minute,second);
  J2k = J2kfrom(year,month,day,UTC);

  printf("J2000: %f\n", J2k);
  printf("Decimal day: %f\n", UTC);
  
  DumpPrint();

  printf("Altitude in degrees: %f\n", degfrom(ALT));
  printf("Azimuth in degrees: %f\n", degfrom(AZ));
  
  /*
  printf("Radians to degrees: %f\n", radfrom(57.3));
  printf("Degrees to radians: %f\n", degfrom(1));

  printf("Radians to degrees: %f\n", 1*180/3.141592654);

  printf("Radians to degrees: %f\n", pi);
  */
  return 1;
}


//:-----------~|  FUCNTIONS   |~-----------://

float Altitude (char lDEC, char lLAT, char lHA){
  float x = sin(DEC)*sin(LAT)+cos(DEC)*cos(LAT)*cos(HA);
  return asin(x);
}

float Azimuth (char lDEC, char lALT, char lLAT){
  float x = (sin(lDEC) - sin(lALT)*sin(lLAT))/(cos(lALT)*cos(lLAT));
  return acos(x);
}

float HourAngle (char lLST, char lRA){
  return lLST - lRA;
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
  return x * ((double)180/pi);
}
float radfrom(float x){
  return x * (pi/(double)180);
}

/*
int inputfloat(char c){
  scanf("%f", &c);
}
*/

void DumpPrint (){
  printf("RA: %f\n", RA);
  printf("DEC: %f\n", DEC );
  printf("LAT: %f\n", LAT);
  printf("LONG: %f\n", LONG);
  printf("HA: %f\n", HA);
  printf("ALT: %f\n", ALT);
  printf("AZ: %f\n", AZ);

}

//:-----------~|     NOTES    |~-----------://
/*



  char uinp;
  printf("please enter r radian and d for degrees\n");
  scanf("%c", &uinp);



  printf("Enter in radians the Right Accention and Declination of the star\n");
  printf("RA:");    scanf("%f", &RA);
  printf("DEC:");   scanf("%f", &DEC);



void uinput (char u){
    printf("please enter r radian and d for degrees\n");
  scanf("%c", &uinp);
  Altitude(uinp);
}



#include <stdio.h>
 
#define print_int(x) printf("Variable " #x "= &#37;d\n", x)  
#define print_char(x) printf("Variable " #x "= %c\n", x)  
#define print_string(x) printf("Variable " #x "= %s\n", x)  
 
int main (int argc, const char * argv[]) {
    int a = 2 ; 
    char letter = 'Z' ; 
    char phrase[] = "Look, I can do this!!" ; 
     
    print_int(a) ;
    print_char(letter) ; 
    print_string(phrase) ; 
    return 0;
}
*/