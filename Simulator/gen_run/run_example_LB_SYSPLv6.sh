#!/bin/sh

waittime=1s

# insert $1: optioncs are 
# init
# mm
# coadd
# ...
# 
run_name=example_LB_SYSPLv6
#sleep $waittime | ./run_LB_SYSPL_v4.2.sh $1 $run_name 'select * from LBSimPtg where id > 180 and id < 366' 'select * from detector where detid < 370' $2 $3
sleep $waittime | ./run_LB_SYSPL_v6.0.sh $1 $run_name 'select * from LBSimPtg where id < 4' 'select * from detector where detid < 10' $2 $3

exit
