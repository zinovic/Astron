#include <stdio.h>
#include <stdlib.h>
#include <string.h>

double UMiNumbers[20] = {						// 7 * 3 -1 = 20
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
 char* name;
 int * ra;
 int * dec;
 int * dist;
} astroObject;

astroObject UMi[6];
astroObject Orion[10];

void setup();

void main(){
 printf("%f\n", UMi[0].ra);
 setup();
 printf("%e\n", UMi[0].ra);
}



void setup(){

	// Ursa Minor
		// Polaris
			//strcpy( UMi[0].name,  "Polaris");
			UMi[0].ra =	2.52974312;
			UMi[0].dec = 89.26413805;
			UMi[0].dist = 132.2751323;
		//
		// Yildun
			//strcpy( UMi[0].name,  "Yildun");
			UMi[1].ra =	17.53691588;
			UMi[1].dec = 86.58632924;
			UMi[1].dist = 56.02240896;
		//
 		// e UMi
			//strcpy( UMi[0].name,  "e UMi");
			UMi[2].ra =	16.76615597;
			UMi[2].dec = 82.03725071;
			UMi[2].dist = 106.2699256;
		//
  		// z UMi
			//strcpy( UMi[0].name,  "z UMi");
			UMi[3].ra =	15.73429554;
			UMi[3].dec = 77.79449901;
			UMi[3].dist = 115.2073733;
		//
		// Kochab
			//strcpy( UMi[0].name,  "Kochab");
			UMi[4].ra =	14.84510983;
			UMi[4].dec = 74.15547596;
			UMi[4].dist = 38.77471888;
		//
		// Pherkad
			//strcpy( UMi[0].name,  "Pherkad");
			UMi[5].ra =	15.34548589;
			UMi[5].dec = 71.83397308;
			UMi[5].dist = 147.275405;
		//
		// n UMi
			//strcpy( UMi[0].name,  "n UMi");
			UMi[6].ra =	16.29180584;
			UMi[6].dec = 75.75470385;
			UMi[6].dist = 29.83293556;
		//
	// Ursa Major
		// Alkaid
			//strcpy( UMa[0].name,  "Alkaid");			
			UMa[0].ra =	13.79237392;
			UMa[0].dec = 49.31330288;
			UMa[0].dist = 30.87372646;
		//
		// Mizar	
			//strcpy( UMa[0].name,  "Mizar");			
			UMa[1].ra =	13.39872773;
			UMa[1].dec = 54.92541525;
			UMa[1].dist = 23.96357537;
		//
		// Alioth
			//strcpy( UMa[0].name,  "Alioth");			
			UMa[2.ra =	12.9004536;
			UMa[2].dec = 55.95984301;
			UMa[2].dist = 24.81389578;
		//		
		// Megrez			
			//strcpy( UMa[0].name,  "Megrez");			
			UMa[3].ra =	12.25706919;
			UMa[3].dec = 57.03259792;
			UMa[3].dist = 24.96878901;
		//		
		// Phad		
			//strcpy( UMa[0].name,  "Phad");			
			UMa[4].ra =	11.89715035;
			UMa[4].dec = 53.69473296;
			UMa[4].dist = 25.64760195;
		//
		// Merak		
			//strcpy( UMa[0].name,  "Merak");			
			UMa[5].ra =	11.0306641;
			UMa[5].dec = 56.38234478;
			UMa[5].dist = 24.348673;
		//	
		// Dubhe		
			//strcpy( UMa[0].name,  "Dubhe");			
			UMa[5].ra =	11.06217691;
			UMa[5].dec = 61.75111888;
			UMa[5].dist = 37.90750569;
		//	
		// h UMa
			//strcpy( UMa[0].name,  "h UMa");			
			UMa[5].ra =	9.52543601;
			UMa[5].dec = 63.06179545;
			UMa[5].dist = 23.14814815;
		//	
		// Muscida
			//strcpy( UMa[0].name,  "Muscida");			
			UMa[5].ra =	8.50445282;
			UMa[5].dec = 60.7184311;
			UMa[5].dist = 56.30630631;
		//	
		// u UMa
			//strcpy( UMa[0].name,  "u UMa");			
			UMa[5].ra =	9.84991436;
			UMa[5].dec = 59.03910437;
			UMa[5].dist = 35.27336861;
		//	
		// ph UMa
			//strcpy( UMa[0].name,  "ph UMa");			
			UMa[5].ra  = 9.86843337;
			UMa[5].dec = 54.06428574;
			UMa[5].dist = 133.8688086;
		//	
		// th UMa
			//strcpy( UMa[0].name,  "th UMa");			
			UMa[5].ra =	9.54786812;
			UMa[5].dec = 51.67860208;
			UMa[5].dist = 13.48617667;
		//	
		// Talitha
			//strcpy( UMa[0].name,  "Talitha");			
			UMa[5].ra =	8.98689965;
			UMa[5].dec = 48.04234956;
			UMa[5].dist = 14.63700234;
		//	
		// k UMa
			//strcpy( UMa[0].name,  "k UMa");			
			UMa[5].ra =	9.06043299;
			UMa[5].dec = 47.15665934;
			UMa[5].dist = 129.7016861;
		//	
		// ch UMa
			//strcpy( UMa[0].name,  "ch UMa");			
			UMa[5].ra =	11.76753725;
			UMa[5].dec = 47.77933701;
			UMa[5].dist = 60.09615385;
		//	
		// ps UMa
			//strcpy( UMa[0].name,  "ps UMa");			
			UMa[5].ra =	11.16107206;
			UMa[5].dec = 44.49855337;
			UMa[5].dist = 45.02476362;
		//	
		// l UMa
			//strcpy( UMa[0].name,  "l UMa");			
			UMa[5].ra =	10.2849797;
			UMa[5].dec = 42.91446855;
			UMa[5].dist = 41.20313144;
		//	
		// Tania Australis
			//strcpy( UMa[0].name,  "l UMa");			
			UMa[5].ra =	10.37216756;
			UMa[5].dec = 41.4994335;
			UMa[5].dist = 76.27765065;
		//	

 }