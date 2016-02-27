//	
	#include <math.h>
	#include <stdio.h>
	#include <stdlib.h>
	#include <unistd.h>
	#include <time.h>

	int xStep = 0;
	int yStep = 0;

	int d = 1;

	unsigned int HalfStep[8]={0x1,0x3,0x2,0x6,0x4,0xC,0x8,0x9};

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

	void loop (int xStepsTobe, int yStepsTobe);

	int main(){
		loop(573,158);
		delay_ms(1000);
		loop(500,291);

	}



	void loop (int xStepsTobe, int yStepsTobe){

	        signed int yToDo = yStepsTobe - yStep;
	        signed int xToDo = xStepsTobe - xStep;

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

			// setting values and exception cases
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
/*		            
		        if (xStep == 0 || yStep == 0){                          // If one motor moves and the other does not, to avoid devision by 0    
		            printf("in the one move fore loop\n\r");
		            yToDo = abs(yToDo);
		            xToDo = abs(xToDo);
		            
		            while (hToDo > 0){
		                output_b(HalfStep[*hStep_ptr&0x7]<<lSft);
		                *hStep_ptr+=hDir;
		                hToDo--;
		                delay_ms(d);
		            }
		            
		            output_b(0x000);
		        // }*/
//
        ///////////////////////// Main Stepper Code Loop ///////////////////////////
	

///////// stepper loop
        while (hToDo>lToDo){
            
            int mod = (int) hToDo/(hToDo%lToDo);          
				printf("mod: 0 = %d\n",mod);
            
            // Mod array building 
			int lStep_ary[10];
			lStep_ary[0]= hToDo/lToDo;
				printf("A#: 0 = %d\n",lStep_ary[0]);
			
			// Populating the rest of the array
			int m = lToDo;
			for (int i = 1; hToDo%m != 0; i++){				
				//printf("inthe loop\n");
				m = hToDo%m;
				printf("m = %d\n",m);
				lStep_ary[i]=(int)(hToDo/m)+1;// this +1 is used to tweek how it behaves
					printf("A#: %d = %d\n",i,lStep_ary[i]);
			}
			/// steper while loop
            	printf("Starting the while\n");
            while ( lToDo > 0 ){
                for ( unsigned int i = /*lStep_ary[0]*/ (int)((hToDo/(lToDo))+0.5); i > 0; i--){// the divison vs the initial array point for tweeking
                    output_b(HalfStep[*hStep_ptr&0x7]<<lSft);
                    *hStep_ptr+=hDir;
                    hToDo--;
                    delay_ms(d);
                    	//printf("fast\n");
                    for (int e = 1; e < 10; e++){
                    	//printf("firstif\n");
	                    if ( lStep_ary[e] != 0 && hToDo%lToDo != 0 && hToDo%lStep_ary[e] == 0 ){
		                    //printf("Extra\n");
		                    i++;	
	                    }
	                    break;
	                }
                }
	                //printf("Slow itteration\n");
	                
	                
                output_b(HalfStep[*lStep_ptr&0x7]<<hSft); //| HalfStep[yStep&0x7]<<lSft);
                    
                *lStep_ptr+=lDir;
                lToDo--;
                delay_ms(d);
                printf("%d,%d\r\n", lToDo, hToDo);
            }

            output_b(0x000);
        }

    }