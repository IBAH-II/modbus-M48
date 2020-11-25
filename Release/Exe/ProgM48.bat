avreal +MEGA48 -p1 -am=map.txt -o1000kHz -5 -e -b -w -v -l2 -fCKOUT=1,CKSEL=2,CKDIV=1,SUT=1,BLEV=6 -cmodbus.a90
if ERRORLEVEL 1 pause
real_set 00001000
