# Astron

Takes RA DEC LAT LONG TIME in radien

to give processed data FOR ALT AZ J2000 

FOR RA and DEC it will calculate the ALT and AZ using a predefine hour angle, the equation for hour angle is written but not implemented. 

decimal day the time converted to decimal form. Called UTC in code, this should be changed. The value is used in determing the current J2000 day. 

J2000 is the current Julian time to epoch 2000 in days. With the decimal points being hours. Perhaps this should be split to avoide doing the whole day calculation everyitme an updated time value is needed. The accurecy is 2 decimal points. Checked against the NASA ICE/GLAS data 
	nsidc.org/data/icesat/glas-date-conversion-tool/date_convert/

The equations for converting decimal to radien and vieversa are written but not used on the inputs. 

