#include "RMatlabConvert.h"

/*
 This handles calling R functions with named arguments.
 The first nargs - 1  elements of the args array of inputs
 consist of the unnamed arguments.  The last element
 is expected to be a Matlab cell of name-value pairs
 identifying the named arguments.
 The remainder of the code is the same as callR.
 In fact, we should consolidate the two into a single block
 of code and pass the named arguments as an additional argument from the
 mexFunction. 
*/
mxArray *
callR(char *funcName, int nargs, const mxArray *args[], const mxArray *namedArgs,
        int nout, mxArray *output[])
{
  SEXP r_expr, rans; 
  int errorOccurred = 0;
  mxArray *mxAns;
  int totalNumArgs = 0, numNamedArgs = 0;

  
  if(namedArgs) { 
      numNamedArgs = mxGetNumberOfElements(namedArgs);
      if(numNamedArgs % 2) {
        mexErrMsgTxt("The collection of named arguments does not have an even number of elements");
      }
      numNamedArgs /= 2;
  }

  totalNumArgs = nargs + numNamedArgs;

  PROTECT(r_expr = allocVector(LANGSXP, 1 + totalNumArgs));
  SETCAR(r_expr, Rf_install(funcName));

  if(totalNumArgs > 0) { 
    int i, ctr;

    SEXP el = CDR(r_expr);
      /* Put the unnamed arguments into the call. */ 
    for(i = 0 ; i < nargs ; i++) {
        SETCAR(el, convertToR(args[i]));
        el = CDR(el);
    }

       /* Now add the named arguments. The idea is to walk over the  
          name, value pairs of the Matlab cell
        */
    for(i = 0, ctr = 0; i < numNamedArgs; i++, ctr += 2) {
	char *buf;
	int len;
	const mxArray *name;

        SETCAR(el, convertToR(mxGetCell(namedArgs, ctr + 1)));

	name = mxGetCell(namedArgs, ctr);
	len = mxGetN(name) * mxGetM(name) + 1;
	buf = (char *) R_alloc(len, sizeof(char)); 	
	mxGetString(name, buf, len); 
        SET_TAG(el, Rf_install(buf));
	el = CDR(el);
    }
  }

  rans = R_tryEval(r_expr, R_GlobalEnv, &errorOccurred);

  if(errorOccurred) {
    /* error from R. */
    UNPROTECT(1);
    MATLAB_ERROR_MESSAGE("Error in R when calling function");
  }

  /* Convert the result to Matlab objects. */
  PROTECT(rans);
  mxAns = convertFromR(rans, nout, output); 
  UNPROTECT(2);

  return(mxAns);
}


/*
 This is the entry point for this file.
*/
void
mexFunction(int nlhs, mxArray *plhs[],
            int nrhs, const mxArray *prhs[])
{
  char *buf;
  int status, buflen;

#if 0
  /* Check R has been initialized. */
  checkRIsInitialized();
#endif


  /* Check we have a single string argument. */
  if(nrhs == 0 || !mxIsChar(prhs[0]) || mxGetM(prhs[0]) != 1)
    MATLAB_ERROR_MESSAGE("Must call routine with a single string");


  buflen = mxGetNumberOfElements(prhs[0]) + 1;
  buf = (char *) mxCalloc(buflen, sizeof(char));
  status = mxGetString(prhs[0], buf, buflen);

  if(status != 0) 
    MATLAB_ERROR_MESSAGE("Problem getting R function name");

#ifdef R_NAMED_CALL
 {
       /* The -2 is because nrhs contains the cell with the named arguments
          and the cell is not an actual argument, 
          just a container for actual arguments. 
          And of course we subtract one for the name of the function to be called.
        */
   int haveNames = nrhs > 1 && mxIsCell(prhs[nrhs-1]);
   callR(buf, nrhs - (haveNames ? 2 : 1), prhs+1, 
             haveNames ? prhs[nrhs - 1] : NULL, nlhs, plhs);
 }
#else
  callR(buf, nrhs - 1, prhs+1, NULL, nlhs, plhs);
#endif

  return;  
}
