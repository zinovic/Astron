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
	loop(300,73);
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
	        }
	    //

        ///////////////////////// Main Stepper Code Loop ///////////////////////////
		    
		        // Lets make an array for the delays

			    //int *arr = (int*)malloc(size*sizeof(int));
				// int lArray = 0;
			    int lStep_ary[10];
			    lStep_ary[0]= hToDo/lToDo;
			    	printf("A#: 0 = %d\n",lStep_ary[0]);
			    int m = lToDo;
			    
			    // printf("we are here\n");
			    // printf("initial while loop condition = %d\n",hToDo%lStep_ary[i]);
			    // printf("hToDo = %d\n",hToDo);
			    // printf("m = %d\n",m);

			    for (int i = 1; hToDo%m != 0; i++){				// the array stuff
			    	//printf("inthe loop\n");
			    	
			    	m = hToDo%m;
			    	//printf("m = %d\n",m);
			    	lStep_ary[i]=hToDo/m;
			    		printf("A#: %d = %d\n",i,lStep_ary[i]);
			    }

			    while (lToDo > 0){								// Running the array
			    	printf("firstlStep,%d,hStep,%d\r\n", *lStep_ptr, *hStep_ptr);
								lToDo--;
		    		*lStep_ptr+= lDir;

			    	for (int e = lStep_ary[0]; e>0; e--){    	 			 			
			 			hToDo--;
					    *hStep_ptr+= hDir;

					    if (lToDo%lStep_ary[1]==0){
					    	hToDo--;
					    	*hStep_ptr+= hDir;
					    	printf("debug#: %d\n",lStep_ary[1]);
					    }
			 		
			 		}
			    	
			    	printf("lStep,%d,hStep,%d\r\n", *lStep_ptr, *hStep_ptr);
			    }

	   	while (hToDo > 0){								// Running the array
	    	hToDo --;
	    	*hStep_ptr+= hDir;
 			
 			for (int i = 0; i<11; i++){
 				if(lStep_ary[i]==0){break;}
 				if(hToDo%lStep_ary[i]==0){
	 				lToDo--;
		    		*lStep_ptr+= lDir;
		    		printf("debug#: %d = %d\n",i,lStep_ary[i]);
		    		break;
		    	}
 			}
	    	// 
	    	printf("lStep,%d,hStep,%d\r\n", *lStep_ptr, *hStep_ptr);
		}

  //           printf("Vals:    xStep:%d      xLeft%d\r\n", *lStep_ptr, lToDo);
  //           printf("Vals:    yStep:%d      yLeft%d\r\n", *hStep_ptr, hToDo);



	    // Running the motors 
        while (hToDo>lToDo){

        	int mod = (int) hToDo/(hToDo%lToDo);

            while ( lToDo > 0 ){
   
                for ( unsigned int i = (unsigned int)((hToDo/(lToDo))+0.5); i > 0; i--){                  // Inside the x itteration we have the more frequent y itteration
 
                    output_b(HalfStep[*hStep_ptr&0x7]<<lSft);
                    *hStep_ptr+=hDir;
                    hToDo--;
                    delay_ms(d);

                    printf("Vals:    xStep:%d      xLeft%d\r\n", *lStep_ptr, lToDo);
            		printf("Vals:    yStep:%d      yLeft%d\r\n", *hStep_ptr, hToDo);

                    if ( hToDo%lToDo != 0 && hToDo%(  mod /*(int) 300/(hToDo%lToDo)*/ ) == 0 ){
                    	printf("extra step\n");
	                    i++;	
                    }

                }

                output_b(HalfStep[*lStep_ptr&0x7]<<hSft); //| HalfStep[yStep&0x7]<<lSft);
                    
                *lStep_ptr+=lDir;
                lToDo--;
                //*hStep_ptr+=hDir;

                delay_ms(d);

            }

            output_b(0x000);
        }

            // after action report
            printf("Vals:    xStep:%d      xLeft%d\r\n", *lStep_ptr, lToDo);
            printf("Vals:    yStep:%d      yLeft%d\r\n", *hStep_ptr, hToDo);

}

/*

int mod = 9999;
            if ( hToDo%lToDo != 0){
                mod = (int) hToDo/(hToDo%lToDo);
            }
            */