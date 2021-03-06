README for LB_SYSPL_v6.0
       written by T. Matsumura, 2022-2-1

0) What's new?
   Refurbish the code in order to be compatible with the current KEKCC environment and newer python modules.
   
1) About
This set of codes is to study the systematic effects for LiteBIRD. 
   a) generate the boresight pointing
   b) generate the signal TOD for each pixel given the focal plane information
   c) do the simple mapmaking based on the differenced time stream
   d) compute the cl using anafast from the created map
   
The systematic effects which the code can study is 
   a) bias in the focal plane database, pixel pointing, differential pointing, polarization angle
   b) bias to the boresight pointing
   c) gain variation w/ and w/o the dipole gain calibration
   d) bandpass mismatch
   e) bias to the HWP mueller matrix
   f) naive sidelobe and ghost

2) How to download this file
> git clone https://github.com/tmatsumu/LB_SYSPL_v6.0.git

3) Codes
This directory contains the two softwares,
     1. pyScan: python program to generate the boresight pointing
     2. Simulator: TOD based simulation pipline to study the systematic effects

In order to start using Simulator code, 
   - execute init.sh as in this directory
   > init.sh

then you'll know what to do ...

4) Setting up the environment
For now the code is there to run at kekcc. To run in the other system is yet to be implement. 
For kekcc, one should set the following path in your .bash_profile as

#PATH=$PATH:$HOME/.local/bin:$HOME/bin
#export $PATH
#export PYTHONPATH=/group/cmb/litebird/common_tools/opt/bin/:/group/cmb/litebird/common_tools/opt/:$PYTHONPATH
#export PATH=/group/cmb/litebird/common_tools/opt/bin/:$PATH

In order to run in some other system, here is the library (incl. python module) which the code needs:
python 2.7
> pip install --user healpy
> pip install astropy --user
healpy 1.14.0 gives an error and so go to the older version:
> pip install healpy==1.13.0 --user
now healpy import errors goes away
> pip install pyephem --user
> pip install pySLALIB --user
> pip install pyfits --user

#python 2.7
#setuptools
#Cython
#numpy
#lapack
#atlas
#scipy
#pyfits-3.3
#pyslalib
#matplotlib-1.5.3
#healpy
#pyephem
#cfitsio
#Healpix3.31
