
if !(${?MATLABPATH}) then
   setenv MATLABPATH /home/duncan/Rpackages/RMatlab/mex
else
   setenv MATLABPATH /home/duncan/Rpackages/RMatlab/mex:$MATLABPATH
endif


setenv R_HOME /usr/local/lib/R

if ("/usr/local/lib/R/lib" != "") then
 if !(${?LD_LIBRARY_PATH}) then
     setenv LD_LIBRARY_PATH /usr/local/lib/R/lib
 else
      setenv LD_LIBRARY_PATH ${LD_LIBRARY_PATH}:/usr/local/lib/R/lib
 endif
endif

if !(${?LD_LIBRARY_PATH}) then
     setenv LD_LIBRARY_PATH @MATLAB_LIB_DIR@
else
      setenv LD_LIBRARY_PATH ${LD_LIBRARY_PATH}:@MATLAB_LIB_DIR@
endif

