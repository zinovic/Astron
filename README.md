# Astron

> A green laser aimed at the sky is like a pointing stick for the heavens.
										*Paraphrasing- Neil deGrasse Tyson*

So it’s basically a green laser strapped to some stepper motors that is programmed to point at the stars, constellations etc. 

**Tools used:**

- PIC16F819 			-originally but that has too little ROM 
- PIC 24HJ128GP502 		-now using this with its amazing 128k ROM!
- MPLab X 				-for the pic development  
- CCSC 					-compiler for the pic
- Pic Kit 2				-write to the pic
- Serial to USB			-for reading serial data form pic and other moduels
- Sublime text 3		-PC side code writing
- MinG					-PC side compiling, the make file for sublime text 3 is included


**Desired features:**
- Stepper motor control form the pic
- LCD screen readout with the current object and ALT AZ coordinates 
- GPS connection to automatically aquire location and time data
- Ablity to search a list of stars and constelations and pick which to be shown
- Choose a sequence of constelation to be shown
- Analog cuircut to detect if the beem has been interruped at close range, using an op-amp cuircuit. It should trigger based of a % change in the LDR resistance, this is fairly challenging as a compromise a threshhold can be set with a voltage devider which is much simpler. 
- Some sort of sizable database of stars, using an SD card to store at best (100k stars) as compromise using EEPROM (1-4k stars)and even more compromise hardcoded on to the pic (10-30 stars). 


**Currenty on the repo:**

- Astron.c 
Will compile on PC and contains the function reqiered to calculate ALT & AZ for a star with given RA & DEC, location and time data. 

- Stepper code.c
Will run on PC, this will take radien values for the two motors and output the step sequences that are needed to get there. This has a lot of debuggin printfs all over so they can be used to see how it works. Also on the drive is some of this step data putinto an excel sheet to visually see the deviation form a straight line. There is room to improve the code a lot however time limited.

The main concern with this is getting the PIC to run both stepper motors at once the timming between the two have to be figured out. 
Solution for motor x & y steps
x = 573
y = 159

573/159 = 3.6037

every time y runs 1 x needs to run 3.6 steps, this is not possible so we simplify to 3 steps still have to make up this 0.6 diffrence

573%159=96	there are 69 x steps missing from 3:1 ratio
we need to spread these 69 x steps evenly

573/96 = 5.86

every 5th x step needs to add an extra step to the 3:1 ratio -> 4:1

every 5th step is when 573%5 == 0; 573-- every loop

with some protection this seems to work, atleast compile and run without a fatal error like /0

- StepperMain.c 
This is the PIC file for the motors, it runs them fairly quickly The AZ motor correctly turns for the given radien values i.e. 2pi == 360deg turn. 




