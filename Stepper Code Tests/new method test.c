#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>

int main(){
	int h = 567;
	int l = 43;
	float ratio = 567/43;
	while (1){
			
		//printf("s: %d &: %d ~s: %d\n",s, s&9 ,~s);

		for (int i = (h-(int)(ratio*l)); i >= 0 ; i--){
			h --;
		}	

		l--;
		printf("%d,%d\n",l,h );
		sleep(0.2);
		if (l== 0){
			printf("Done!\n");
			sleep(120);
		}
	}
	



	/* code */
	return 0;
}
