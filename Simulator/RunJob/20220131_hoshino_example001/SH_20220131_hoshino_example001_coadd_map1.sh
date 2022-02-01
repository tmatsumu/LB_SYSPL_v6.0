#!/bin/sh
python /home/cmb/tmatsumu/LB_simulation/LB_SYSPL_v6.0/Simulator//src/run_coaddmaps.py /group/cmb/litebird/usr/tmatsumu/LB_SYSPL_v6.0//SimedMaps/RunLog/20220131_hoshino_example001 coadd_map1 512 "select * from LBSimPtg where id < 2" /home/cmb/tmatsumu/LB_simulation/LB_SYSPL_v6.0/Simulator//xml/xml_par_20220131_hoshino_example001.xml & 
wait
