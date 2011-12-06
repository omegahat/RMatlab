if test -z "$MATLABPATH" ; then
   MATLABPATH=/home/duncan/Rpackages/RMatlab/mex
else
   MATLABPATH=/home/duncan/Rpackages/RMatlab/mex:$MATLAB_PATH
fi

export MATLABPATH


R_HOME=/usr/local/lib/R
export R_HOME

if test -n "/usr/local/lib/R/lib" ; then
 if test -z "$LD_LIBRARY_PATH" ; then
     LD_LIBRARY_PATH=/usr/local/lib/R/lib
 else
     LD_LIBRARY_PATH=${LD_LIBRARY_PATH}:/usr/local/lib/R/lib
 fi
fi


if test -z "${LD_LIBRARY_PATH}" ;  then
     export LD_LIBRARY_PATH=@MATLAB_LIB_DIR@
else
     export LD_LIBRARY_PATH=${LD_LIBRARY_PATH}:@MATLAB_LIB_DIR@
fi
