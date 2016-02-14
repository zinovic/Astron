#define PORT_S PIN_B3

#if !defined(__PCD__)
#error This example will only compile for the PCD 24 bit compiler
#endif
#include <24HJ128GP502.h>

#fuses HS,NOWDT,noprotect
//#fuses FRC,FRC_PLL,NOWDT,NOPROTECT,
#use delay(clock=7350000)                   //7.212Mhz is apparently optimal for 0% error with baud 9600. 
//IT works with the following code:
//#pin_select U1TX = PIN_B14
//#pin_select U1RX = PIN_B15
//#use rs232(baud=9600, xmit=PIN_B14, rcv=PIN_B15,parity=N,bits=8)

//#fuses INTRC_IO, NOWDT, PUT, NOMCLR, BROWNOUT
//#use delay(clock=8000000)
#use rs232(baud=9600, RCV=PIN_B13, stream=GPSMODULE, ERRORS)
#use rs232(baud=9600, XMIT=PIN_B14, RCV=PIN_B15, stream=PC, ERRORS,parity=N,bits=8)


#include <string.h>
#include <stdlib.h>

#define GPS fgetc(GPSMODULE)
#define spacer fprintf(PC, "\n\r")

unsigned char s[20], start[6], RMC[]="GPRMC", GGA[]="GPGGA",lat[12], lon[12];
unsigned char UTC[9], latD[2], latDeg[3], latMin[10], lonDeg[3], lonMin[10], lonD[2], sat[3], fix = 0, date[9];

#locate s=0xA0
#locate start=0xB5
#locate RMC=0xBC
#locate GGA=0xC2
#locate latMin=0x120
#locate lonMin=0x130
#locate lat=0x1A0
#locate lon=0x1B0
#locate date=0x140
#locate UTC=0x150

void getBlock();
void getStart();
void update();
void getGGA();
void getRMC();


void main(){

   
    setup_spi(FALSE);                         
    setup_wdt(WDT_OFF);                       
//                 
//    setup_low_volt_detect(FALSE);                 
//    SET_TRIS_B(0xFF);
//    // B7,B6,B5,B4 are outputs
//    // B15,B14,B13,B12,B11,B10,B9,B8, B3,B2,B1,B0 are inputs
//    setup_timer1(TMR_INTERNAL|TMR_DIV_BY_1);          // Internal clock
//    
    
    printf("\n\r\vEnter char to start read and set all pins to low");

    //setup_oscillator(7350000);
    unsigned char k = fgetc(PC);
    
//    output_b(0);
//    output_a(0);
    delay_ms(2000);
    fprintf(PC, "\n\r \n\rReading GPS...\n\r");

        while(1){
            update();
            printf("\tDone update loop\n\r");
            getRMC();
            printf("\tDone RMC loop\n\r");
            update();
            printf("\tGPS loop\n\r");
            getGGA();
            printf("\tRun GGA\n\r");
            getRMC();
            printf("\tRun RMS\n\r");
            update();
            printf("Looping");
        }

    
    printf("\n\r\vclear");
do {
   printf("Message\n\r");
   delay_ms(500);

   } while (TRUE);

} 


void ShiftCharLeft(char * s){				//Move whole string left 1 to get rid of leading character
	unsigned char i=0;
	while(s[i]!=NULL){
		s[i]=s[i+1];
		i++;
	}
	s[i]=NULL;
}

void getBlock(){							//grab GPS string text block upto first comma
	unsigned char c=' ', i=0;
	while(c!=','){							//Blocks are comma delimited
		s[i]=c=GPS;
		i++;
	}
	s[i-1]=NULL;							//Replace last character (comma) with NULL
}

short getStart(unsigned char * header){
	printf("\tgetStart start RMS\n\r");
    while(GPS!='$');
	start[0]=GPS;
	start[1]=GPS;
	start[2]=GPS;
	start[3]=GPS;
	start[4]=GPS;
	start[5]=GPS;
	start[5]=NULL;
	printf("\tgetStart Loop\n\r");
    if(strcmp(start,header)==0) return 1;	
		else return 0;
}

void getGGA(){
        printf("\tinside getGGA\n\r");
	
    unsigned char t[7], i, n, fixs[2];
		while(!getStart(GGA));					//Repeatedly try for GGA header
		printf("\tPast getStart GGA\n\r");
        getBlock();								//first is UTC Time in hhmmss.00
		strncpy(t,s,6);							//
		getBlock();
		strcpy(lat,s);							//Lattitude is 2nd block
		getBlock();
		strcpy(latD,s);							//Lattitude Direction is 3rd block
		getBlock();
		strcpy(lon,s);							//Longitude is 4th block
		getBlock();
		strcpy(lonD,s);							//Longitude Direction is 5th block
		getBlock();
		strcpy(fixs,s);							//Fix status is 6th
		getBlock();
		strcpy(sat,s);							//Number of satellites tracked is 7th
		UTC[0]=t[0];							//Format time
		UTC[1]=t[1];
		UTC[2]=':';
		UTC[3]=t[2];
		UTC[4]=t[3];
		UTC[5]=':';
		UTC[6]=t[4];
		UTC[7]=t[5];
		UTC[8]=NULL;

		latDeg[0]=lat[0];								////Copy first two digits to degrees
		latDeg[1]=lat[1];
		latDeg[2]=NULL;
		n=strchr(lat,'.')-lat;							//Find dot location in string
		for(i=(n-2);i<(n+6);i++)latMin[i-2]=lat[i];		//Take 2 digits before and 5 digits after dot
		if(!isdigit(latMin[0]))latMin[0]=' ';						
		latMin[8]=NULL;									//Terminate string
						
		lonDeg[0]=lon[0];								////Copy first two digits to degrees
		lonDeg[1]=lon[1];
		lonDeg[2]=NULL;
		n=strchr(lon,'.')-lon;							//Find dot location in string
		for(i=(n-2);i<(n+6);i++)lonMin[i-2]=lon[i];		//Take 2 digits before and 5 digits after dot
		lonMin[9]=NULL;									//Terminate string
		ShiftCharLeft(lonMin);							//Has a leading character for some reason

		fix=atoi(fixs);									//Get fix as a number
}


void getRMC(){											//RMC used for date only
		printf("\tRMC void\n\r");
        unsigned char i; 
		while(!getStart(RMC));
		for(i=0;i<9;i++)getBlock();						//Date stamp is 9th block
		date[0]=s[0];
		date[1]=s[1];
		date[2]='/';
		date[3]=s[2];
		date[4]=s[3];
		date[5]='/';
		date[6]=s[4];
		date[7]=s[5];
		date[8]=NULL;
}

void update(){
		if(fix>0){
			fprintf(PC,"\n\rGood fix with %s satellites", sat);
			fprintf(PC,"\n\rUTC:%s  ", UTC);
			fprintf(PC,"Date:%s", date);
			fprintf(PC,"\n\r%s%s°%s' %s%s°%s' ", latD, latDeg, latMin, lonD, lonDeg, lonMin);
		}
		else fprintf(PC,"\n\r*** POOR SATELLITE RECEPTION ***"); 
		spacer;
}

//GPGLL
//GPRMC
//GPVTG
//GPGGA
//GPGSA
//GPGSV
//GPGSV
//GPGSV

//$GPRMC,194902.00,V,,,,,,,170414,,,N*7D

//$GPGGA,181214.00,5129.87836,N,00015.87946,W,1,03,6.19,68.6,M,45.8,M,,*70
//eg3. $GPGGA,hhmmss.ss,llll.ll,a,yyyyy.yy,a,x,xx,x.x,x.x,M,x.x,M,x.x,xxxx*hh
//1    = UTC of Position
//2    = Latitude
//3    = N or S
//4    = Longitude
//5    = E or W
//6    = GPS quality indicator (0=invalid; 1=GPS fix; 2=Diff. GPS fix)
//7    = Number of satellites in use [not those in view]
//8    = Horizontal dilution of position
//9    = Antenna altitude above/below mean sea level (geoid)
//10   = Meters  (Antenna height unit)
//11   = Geoidal separation (Diff. between WGS-84 earth ellipsoid and
//       mean sea level.  -=geoid is below WGS-84 ellipsoid)
//12   = Meters  (Units of geoidal separation)
//13   = Age in seconds since last update from diff. reference station
//14   = Diff. reference station ID#
//15   = Checksum
/*
 * 
 */