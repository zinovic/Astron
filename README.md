# Astron

A green laser aimed at the sky is like a pointing stick for the heavens.
										*Paraphrasing- Neil deGrasse Tyson*

So it’s basically a green laser strapped to some stepper motors that is programmed to point at the stars, constellations etc. 

The tools used for this project are:

PIC16F819 				-originally but that has too little ROM 
PIC 24HJ128GP502 		-now using this with its amazing 128k ROM!

MPLab X 				-for the pic development  
CCSC 					-compiler for the pic
Pic Kit 2				-write to the pic
Serial to USB			-for reading serial data form pic and other moduels

Sublime text 3			-PC side code writing
MinG					-PC side compiling, the make file used is included


**Desired features:**
- Stepper motor control form the pic
- LCD screen readout with the current object and ALT AZ coordinates 
- GPS connection to automatically aquire location and time data
- Ablity to search a list of stars and constelations and pick which to be shown
- Choose a sequence of constelation to be shown
- Analog cuircut to detect if the beem has been interruped at close range, using an op-amp cuircuit. It should trigger based of a % change in the LDR resistance, this is fairly challenging as a compromise a threshhold can be set with a voltage devider which is much simpler. 
- Some sort of sizable database of stars, using an SD card to store at best (100k stars) as compromise using EEPROM (1-4k stars)and even more compromise hardcoded on to the pic (10-30 stars). 


**Currenty on the repo:**
Astron.c 
Will compile on PC and contains the function reqiered to calculate ALT & AZ for a star with given RA & DEC, location and time data. 

Stepper code.c
Will run on PC, this will take radien values for the two motors and output the step sequences that are needed to get there. This has a lot of debuggin printfs all over so they can be used to see how it works. Also on the drive is some of this step data putinto an excel sheet to visually see the deviation form a straight line. There is room to improve the code a lot however time limited. 

GPS_Blink.c
This is a bit of code taken from what lecturer write to decode GPS tracker, adapted for the pic24. This will run on the pic however a clear signal with th GPS has not yet been achived, maybe its a faulty unit? Interestingly the time does show up correctly. 




