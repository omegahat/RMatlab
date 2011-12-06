#include "RMatlabConvert.h"

#include "engine.h"

#include "Rdefines.h"

#ifdef MATLAB_MEX_FILE
#undef MATLAB_MEX_FILE
#endif

/* Could keep more than one of these and have a stack. */ 
static Engine *DefaultMatlabEngine;

/*
  Convert the pointer to the Matlab engine into 
  an R object -  an external reference object.
*/
SEXP
R_matlabEngine(Engine *eng)
{
  SEXP ans, klass;

  DefaultMatlabEngine = eng;
  
  PROTECT(ans = R_MakeExternalPtr((void *) eng, Rf_install("MatlabEngine"), R_NilValue));
  PROTECT(klass = allocVector(STRSXP, 2));
  SET_STRING_ELT(klass, 0, mkChar("MatlabEngine"));
  SET_STRING_ELT(klass, 1, mkChar("MatlabInterface"));
  SET_CLASS(ans, klass);

  UNPROTECT(2);

  return(ans);
}


/*
  From the R object, extract the Matlab Engine * reference.
  This handles NULL, external pointers and leaves everything
  else alone.  This means it can handle the MexInterface and
  MatlabEngine objects.
*/
Engine *
getEngine(SEXP rengine)
{
  Engine *eng;

  if(Rf_length(rengine) == 0) {
    PROBLEM "NULL value in Matlab interface"
    ERROR;
  }

  if(TYPEOF(rengine) == EXTPTRSXP) 
    eng = R_ExternalPtrAddr(rengine);

  return(eng);
}


/*
  An entry point to be called by R to get the 
  current default Engine reference, if it exists.
  Returns the external pointer object or the 
  R NULL value.
*/
SEXP
RMatlab_getDefaultEngine()
{
  if(DefaultMatlabEngine)
    return(R_matlabEngine(DefaultMatlabEngine));

  return(R_NilValue);
}


/*
  Open a Matlab Engine instance.
  This makes that engine the default.
*/
SEXP
RMatlab_init(SEXP arg)
{
 Engine *eng;

 eng =  engOpen(CHAR(STRING_ELT(arg, 0)));

 if(!eng) {
   PROBLEM  "Cannot start Matlab interpreter."
   ERROR;
 }

 return(R_matlabEngine(eng));
}


/*
  Close a Matlab Engine that we will no longer use to 
  perform computations or store data.
*/
SEXP
RMatlab_close(SEXP engine)
{
 Engine *eng;
 int status;

 eng = getEngine(engine);

 status = engClose(eng);

 DefaultMatlabEngine  = NULL;

 return(Rf_ScalarInteger(status));
}


/*
  Evaluate a Matlab command given as a string.
  The return value is not the result of the LHS of the
  command, but rather the status of the evaluation.
  One has to retrieve the value of the variable.
*/
SEXP
RMatlab_evalString(SEXP cmd, SEXP engine)
{
  int status;
  Engine *eng = NULL;

  eng = getEngine(engine);

  if(!eng)
    status = mexEvalString(CHAR(STRING_ELT(cmd, 0)));
  else
   status = engEvalString(eng, CHAR(STRING_ELT(cmd, 0)));

   return(Rf_ScalarInteger(status));
}


SEXP
RMatlab_getVariable(SEXP varNames, SEXP where, SEXP convert, SEXP engine)
{
  SEXP ans;
  int i, n;
  Engine *eng;

  eng = getEngine(engine);

  PROTECT(ans = allocVector(VECSXP, Rf_length(varNames)));
  n = Rf_length(varNames);
  for(i = 0; i < n ; i++) {
    SEXP tmp;
    const mxArray *el;
    if(!eng)
      el = mexGetVariablePtr(CHAR(STRING_ELT(varNames, i)), CHAR(STRING_ELT(where, 0))); 
    else
      el = engGetVariable(eng, CHAR(STRING_ELT(varNames, i)));

    if(LOGICAL_DATA(convert)[i])
      tmp = convertToR(el);
    else {
      /*XXX make this a separate routine and put class info on it. */
      tmp = R_MakeExternalPtr((void *) el, Rf_install("MatlabReference"), R_NilValue);
    }
    SET_VECTOR_ELT(ans, i, tmp);
  }
  UNPROTECT(1);
  return(ans);
}


SEXP
RMatlab_setVariable(SEXP varNames, SEXP values, SEXP where, SEXP engine)
{
  SEXP ans = R_NilValue;
  int i, n;
  Engine *eng;

  eng = getEngine(engine);

  n = Rf_length(values);
  PROTECT(ans = allocVector(INTSXP, Rf_length(varNames)));
  for(i = 0; i < n ; i++) {
    mxArray *tmp = convertFromR(VECTOR_ELT(values, i), 1, NULL);
    if(!eng)
      INTEGER_DATA(ans)[i] = mexPutVariable(CHAR(STRING_ELT(where, 0)), CHAR(STRING_ELT(varNames, i)), tmp); 
    else
      INTEGER_DATA(ans)[i] = engPutVariable(eng, CHAR(STRING_ELT(varNames, i)), tmp); 

  }
  UNPROTECT(1);
  return(ans);
}





SEXP
RMatlab_invoke(SEXP fun, SEXP args, SEXP numOut)
{
 mxArray **plhs = NULL, **mxArgs = NULL;
 int nout = INTEGER_DATA(numOut)[0];
 int i, status;
 SEXP ans = R_NilValue;

 if(nout > 0)
   plhs = (mxArray **) R_alloc(nout, sizeof(mxArray *));

 if(Rf_length(args)) {
    int n = Rf_length(args);
    mxArgs = (mxArray **) R_alloc(nout, sizeof(mxArray *));
    for(i = 0; i < n; i++) 
       mxArgs[i] = convertFromR(VECTOR_ELT(args, i), 1, NULL);
  }

 mexSetTrapFlag(1); /* Ensure that any errors in the MEX call return us to here. */
 status = mexCallMATLAB(nout, plhs, Rf_length(args), mxArgs, CHAR(STRING_ELT(fun, 0)));

 if(status) {
   PROBLEM "Error calling matlab function %s", CHAR(STRING_ELT(fun, 0))
   ERROR;
 }

 if(nout) {
  PROTECT(ans = allocVector(VECSXP, nout));
  for(i = 0; i < nout ; i++)
     SET_VECTOR_ELT(ans, i,  convertToR(plhs[i]));
  UNPROTECT(1);
 }

 return(ans);
}

SEXP
RMatlab_getMexFunctionName()
{
 const char *name;
 SEXP ans;

 name = mexFunctionName();

 if(name)
   ans = mkString(name);

 return(ans);
}


/*
 Get the values for properties within a Matlab
 graphics "handle".
*/

SEXP
RMatlab_mexGetProperty(SEXP handle, SEXP props)
{
  double h = REAL(handle)[0];
  int i, n;
  SEXP ans;

  n = Rf_length(props);
  PROTECT(ans = allocVector(VECSXP, n));

  for(i = 0; i < n; i++) {
    const mxArray * el;
    const char *name;

    name = CHAR(STRING_ELT(props, i));
    el = mexGet(h, name);

    SET_VECTOR_ELT(ans, i, convertToR(el));
  }

  SET_NAMES(ans, props);
  UNPROTECT(1);

  return(ans);
}


SEXP
RMatlab_mexSetProperty(SEXP handle, SEXP props, SEXP values)
{
  double h = REAL(handle)[0];
  int i, n;
  SEXP ans;

  n = Rf_length(props);

  PROTECT(ans = allocVector(LGLSXP, n));

  for(i = 0; i < n; i++) {
    mxArray *el;
    const char *name;

    name = CHAR(STRING_ELT(props, i));
    el = convertFromR(VECTOR_ELT(values, i), 1, NULL);
    LOGICAL(ans)[0] = mexSet(h, name, el);
  }

  SET_NAMES(ans, props);
  UNPROTECT(1);

  return(ans);
}
