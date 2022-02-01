#!/bin/sh

dir_bin=/home/cmb/tmatsumu/LB_simulation/LB_SYSPL_v6.0/pyScans/

# LiteBIRD sidelobe source 

# (option of shell input)
# (title)
# (sampleing [Hz])
# (sec in a day)
# (precession angle [degs])
# (precession period [min])
# (spin axis angle [degs])
# (spin axis period [rpm])
# (sim run period [days])
# (nside for projection)

# this script was used to compare with the pointing generated by the Guillaume and also the Tomo's pyScan pointing running on the local laptop in python.

date=20220128_example01

python ${dir_bin}/src/run_scan_todgen_c.mod2.py y LB_L2_${date}_samplerate 1 86400. 45. 90.6 50. 0.1 1 256