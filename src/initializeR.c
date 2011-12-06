#include "mex.h"
#include "engine.h"

#include <Rinternals.h>
#include <Rdefines.h>


#include <Rversion.h>
#if R_VERSION >= R_Version(2, 4, 0)
#include <Rembedded.h> /* for Rf_initEmbeddedR() and Rf_endEmbeddedR() */
#endif
#if R_VERSION >= R_Version(2, 3, 1)
#define CSTACK_DEFNS /* for R_CStackLimit */
#include <Rinterface.h> /* for R_SignalHandlers */
#endif


static int loadRMatlab();

static void RMatlab_closeRSession() {
#if R_VERSION >= R_Version(2, 4, 0)
  Rf_endEmbeddedR(0);
#endif
}



/*
  Takes a Matlab cell array of strings and places them in 
  an array.
*/
static char **
copyCommandLineArguments(const mxArray *mxEls, int *nargs)
{
    char **args;
    int len, i;

    *nargs = mxGetNumberOfElements(mxEls);
    args = (char **) malloc(sizeof(char *) * *nargs);

    for(i = 0; i < *nargs; i++) {
      int len;
      mxArray *el = mxGetCell(mxEls, i);
      len = mxGetN(el) + 1;
      args[i] = (char *) malloc(len * sizeof(char));
      mxGetString(el, args[i], len);
    }

    return(args);
}

/*
 This is the routine that actually starts the R engine.
*/
void
mexFunction(int nlhs, mxArray *plhs[],
            int nrhs, const mxArray *prhs[])
{
     /* Need to make certain these don't go away after we call them. 
        I believe R thinks they will be around for the duration of the R engine 
        so make them static.
      */
    static  char **R_cmdArgs = NULL;
    static int embeddedR_Is_Running = 0;

    int nargs;

    mxArray *init;


    if (embeddedR_Is_Running){
      mexWarnMsgTxt("The R embedded environment was already initialized.\nTo change R parameters you need to restart MATLAB.");
      return;
    }

      /* Command line arguments in prhs[0] */
    nargs = nrhs; 
    R_cmdArgs = copyCommandLineArguments(prhs[0], &nargs);

#if R_VERSION >= R_Version(2, 3, 1)
    /* Before starting the R main loop, we need to disable signal handlers, 
       otherwise they will conflict with MATLAB display management. Without 
       this only 
            matlab -nodisplay
       works. */
    R_SignalHandlers = 0;
#endif

#if 0
    if (!(embeddedR_Is_Running = Rf_initEmbeddedR(nargs, R_cmdArgs)))
    	mexErrMsgTxt("Error initializing the R embedded environment");
#else
     /* We need to unroll Rf_initEmbeddedR(nargs, R_cmdArgs) to disable the
        stack-checking between Rf_initialize_R() and setup_Rmainloop() */
     Rf_initialize_R(nargs, R_cmdArgs);
     R_Interactive = TRUE;  /* Rf_initialize_R set this based on isatty */
     R_CStackLimit = (uintptr_t)-1;
     setup_Rmainloop();
 
     embeddedR_Is_Running = 1;
#endif

    if (R_cmdArgs)
      free(R_cmdArgs);


    loadRMatlab();

    init = mxCreateLogicalScalar(1);
    mexPutVariable("base", "REngineInitialized", init);

    if(nlhs > 0) 
      plhs[0] = init;

    mexAtExit(RMatlab_closeRSession);
}


/*
  This routine is called to load the RMatlab package and
  to store a variable (named .MexInterface) in the R 
  work space to indicate that there is a Mex interface
  available rather than the Engine API. 
*/
static int
loadRMatlab()
{
  SEXP e, klass;
  int errorOccurred;

  PROTECT(e = allocVector(LANGSXP, 2));
  SETCAR(e, Rf_install("library"));
  SETCAR(CDR(e), mkString("RMatlab"));

  R_tryEval(e, R_GlobalEnv, &errorOccurred);
  UNPROTECT(1);

  if(errorOccurred)
    mexErrMsgTxt("Cannot load RMatlab package in R. Something is wrong with the installation of RMatlab.");

  PROTECT(e = ScalarLogical(1));
  PROTECT(klass = allocVector(STRSXP, 2));
  SET_STRING_ELT(klass, 0, mkChar("MexInterface"));
  SET_STRING_ELT(klass, 1, mkChar("MatlabInterface"));
  SET_CLASS(e, klass);

  defineVar(Rf_install(".MexInterface"), e, R_GlobalEnv);

  UNPROTECT(2);

  return(1);  
}


