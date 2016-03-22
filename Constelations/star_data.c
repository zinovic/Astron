#include <stdio.h>
#include <stdlib.h>

double UMiNumbers[20] = {							// 7 * 3 -1 = 20
	2.52974312,89.26413805,132.2751323,				// Polaris, a
	17.53691588,86.58632924,56.02240896,			// Yildun, d
	16.76615597,82.03725071,106.2699256,			// e UMi, e
	15.73429554,77.79449901,115.2073733,			// z UMi, z
	14.84510983,74.15547596,38.77471888,			// Kochab, b
	15.34548589,71.83397308,147.275405,				// Pherkad, g
	16.29180584,75.75470385,29.83293556				// n UMi, n
};

char UMiStars[6][10] = {
	"Polaris","Yildun","e UMi","z UMi","Kochab","Pherkad","n UMi"
};


//int UMi[6]; 

typedef struct{
 int* name;
 int* ra;
 int* dec;
 int* mag;
} astroObject;

astroObject UMi[6];
astroObject Orion[10];

void setup();

void main(){
 setup();
 printf("%d\n", &UMi[0].ra);
}

void setup(){

	// Ursa Minor
	for (char i = 0; i<=7; i++){		
		UMi[i].name = &UMiStars[i];
		UMi[i].ra =	&UMiNumbers[i];
		UMi[i].dec = &UMiNumbers[i+1];
		UMi[i].mag = &UMiNumbers[i+2];
		}
 }