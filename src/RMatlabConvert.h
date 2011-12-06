#ifndef R_MATLAB_CONVERT_H
#define R_MATLAB_CONVERT_H

#include "mex.h"
#include <Rinternals.h>

#define MATLAB_ERROR_MESSAGE(x) mexErrMsgTxt((x))

mxArray *convertFromR(SEXP val, int nout, mxArray *output[]);
SEXP convertToR(const mxArray *val);

#endif
