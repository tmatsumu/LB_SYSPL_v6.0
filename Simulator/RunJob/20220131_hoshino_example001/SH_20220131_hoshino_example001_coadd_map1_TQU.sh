#!/bin/sh
python /home/cmb/tmatsumu/LB_simulation/LB_SYSPL_v6.0/Simulator//src/fits_sep2oneTQU.py /group/cmb/litebird/usr/tmatsumu/LB_SYSPL_v6.0//SimedMaps/RunLog/20220131_hoshino_example001/coadd_map/coadd_map1/mapT.fits /group/cmb/litebird/usr/tmatsumu/LB_SYSPL_v6.0//SimedMaps/RunLog/20220131_hoshino_example001/coadd_map/coadd_map1/mapQ.fits /group/cmb/litebird/usr/tmatsumu/LB_SYSPL_v6.0//SimedMaps/RunLog/20220131_hoshino_example001/coadd_map/coadd_map1/mapU.fits /group/cmb/litebird/usr/tmatsumu/LB_SYSPL_v6.0//SimedMaps/RunLog/20220131_hoshino_example001/coadd_map/coadd_map1/mapTQU.fits
rm -f /group/cmb/litebird/usr/tmatsumu/LB_SYSPL_v6.0//SimedMaps/RunLog/20220131_hoshino_example001/coadd_map/coadd_map1/*.npy
wait
