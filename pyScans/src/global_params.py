import matsumulib as mylib 

dir_wrk='/home/cmb/tmatsumu/LB_simulation/LB_SYSPL_v6.0/pyScans/'
dir_dataout='/group/cmb/litebird/simdata/Scans/'

path_inputparams=dir_wrk+'input_params/'
path_src=dir_wrk+'src/'
path_out=dir_dataout+'dataout/'

#today_julian = mylib.convert_Gregorian2Julian( 2020, 3, 20, 3, 50, 0)
#today_julian = mylib.convert_Gregorian2Julian( 2018, 3, 21, 1, 15, 0)

today_julian = mylib.convert_Gregorian2Julian( 2027, 3, 21, 3, 50, 0)
#today_julian = mylib.convert_Gregorian2Julian( 2027, 6, 21, 3, 50, 0)

option_gen_ptg = True
