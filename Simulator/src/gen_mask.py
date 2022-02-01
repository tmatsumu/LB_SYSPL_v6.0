'''
python maskgen.py filename_in filename_out
'''
import numpy as np
import healpy as h
import numpy as np
import matplotlib as mpl
mpl.use('Agg')
import matplotlib.pylab as py
#import pylab as pl
import sys

filename_in = sys.argv[1]
filename_out = sys.argv[2]

map = h.read_map(filename_in)

npix = len(map)
mask = np.zeros(npix,int)
ind = np.where(map > 0)

mask[ind[0]] = 1

h.write_map(filename_out,mask)


