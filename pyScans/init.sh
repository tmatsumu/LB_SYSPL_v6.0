#!/bin/sh

echo ''
echo '##############################'
echo ''
echo 'Initialization in pyScan, T.Matsumura 2014-10-2'
echo '  revised by TM/2017-11-04, fixing bug pointed by Guillaume'
echo ''
echo ' Execute below then you know what to do '
echo '> init.sh help'
echo ''
echo 'This script creats the symbolic link for pyScans'
echo ' and symbolic link to the pointing data'
echo '##############################'
echo ''

dir_LBSYSver=/home/cmb/tmatsumu/LB_SYSPL/LB_SYSPL_v6.0/
dir_data=/group/cmb/litebird/simdata/Scans/dataout/

if [[ $1 = "help" ]];then
    echo '==========================='
    echo ' ###### INSTRUCTION #######'
    echo 'change the following directory names in init.sh'
    echo '   dir_LBSYSver: the path to the directory that contains pyScan '
    echo '   dir_data (leave this as it is if you want to write the output pointing to the shared pointing directory)'
    echo ''
    echo '   Once you change the directory names, do below'
    echo '> ./init.sh exec'
    echo ''
fi


if [[ $1 = "exec" ]];then
    echo ''
    echo 'add the sympolic link of codes/matsumulib.py to pyScans/src'
    echo ${dir_LBSYSver}/codes/matsumulib.py
    echo ${dir_LBSYSver}/pyScans/src/matsumulib.py
    ln -s ${dir_LBSYSver}/codes/matsumulib.py  ${dir_LBSYSver}/pyScans/src/matsumulib.py
    echo 'add the sympolic link of codes/matsumulib.py to pyScans/src'

    ln -s ${dir_data} ${dir_LBSYSver}/pyScans/dataout
    echo ''
    echo 'DONE'
fi
