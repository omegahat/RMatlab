#include "RMatlabConvert.h"
#include <Rdefines.h>

/*
 See http://www.mathworks.com/access/helpdesk/help/techdoc/apiref/apiref.html
 for information about the Matlab External Interfaces API.
 And the Writing R Extensions manual.
*/

SEXP convertMatlabCell(const mxArray *m);
mxArray *convertFromRObject(SEXP obj);
SEXP convertMatlabStructToR(const mxArray *m);
SEXP convertMatlabCellToRList(const mxArray *m, int nels);
SEXP convertUint8ToR(const mxArray *m, int ndims, const int *dims, int nelements);

/*
 Convert an R object to one or more Matlab objects
 and insert them into the return array if specified.
*/
mxArray *
convertFromR(SEXP val, int nout, mxArray *output[])
{
  mxArray *ans = NULL;
  int len, i;

  int *dims = NULL;
  int ndims;

  if(nout < 1)
    return(NULL);

  ndims = Rf_length(GET_DIM(val));
  len = Rf_length(val);
  if(ndims) {
    dims = INTEGER(GET_DIM(val));
  }

  /* Have to check for matrices/arrays (i.e. having a dimension) before just looking at the mode. 
     We also want to implement an (externally) extensible converter mechanism as in the other
     inter-system interfaces.
   */

    /* First check if we have a matrix.
       Then if we have an object with a class we convert it to a structure.
     */
  if(TYPEOF(val) == VECSXP) { /*  && Rf_length(GET_CLASS(val)) */
    ans = convertFromRObject(val);
  } else if(IS_COMPLEX(val)) {
    double *real, *imaginary;

    if(ndims == 0)
      ans = mxCreateNumericMatrix(len, 1, mxDOUBLE_CLASS, mxCOMPLEX);
    else if(ndims == 2)
      ans = mxCreateNumericMatrix(dims[0], dims[1], mxDOUBLE_CLASS, mxCOMPLEX);
    else 
      ans = mxCreateNumericArray(ndims, dims, mxDOUBLE_CLASS, mxCOMPLEX);

    len =  Rf_length(val);
    real = mxGetPr(ans);
    imaginary = mxGetPi(ans);
    for(i = 0; i < len; i++) {
      Rcomplex el = COMPLEX(val)[i];
      real[i] = el.r;
      imaginary[i] = el.i;
    }

  } else if(IS_CHARACTER(val)) {

    if(ndims == 0)
      ans = mxCreateCellMatrix(len, 1);
    else if(ndims == 2)
      ans = mxCreateCellMatrix(dims[0], dims[1]);
    else 
      ans = mxCreateCellArray(ndims, dims);


    len =  Rf_length(val);
    for(i = 0; i < len; i++) {
      mxSetCell(ans, i, mxCreateString(CHAR(STRING_ELT(val, i))));
    }
  } else if(IS_NUMERIC(val) || IS_INTEGER(val)) {
    double *data;

    if(ndims == 0)
      ans = mxCreateDoubleMatrix(len, 1, mxREAL);
    else if(ndims == 2)
      ans = mxCreateDoubleMatrix(dims[0], dims[1], mxREAL);
    else 
      ans = mxCreateNumericArray(ndims, dims, mxDOUBLE_CLASS, mxREAL);

    len =  Rf_length(val);

    data = mxGetPr(ans);
    for(i = 0; i < len; i++) {
      data[i] = TYPEOF(val) == REALSXP ? REAL(val)[i] : (double) INTEGER(val)[i];

      /*	            ISNAN(INTEGER(val)[i]) ? mxGetNaN() : INTEGER(val)[i] ; */
    } 
  } else if(IS_LOGICAL(val)) {
    mxLogical *data;

    if(ndims == 0)
      ans = mxCreateLogicalMatrix(len, 1);
    else if(ndims == 2)
      ans = mxCreateLogicalMatrix(dims[0], dims[1]);
    else 
      ans = mxCreateLogicalArray(ndims, dims);


    len =  Rf_length(val);

    data = mxGetLogicals(ans);
    for(i = 0; i < len; i++) {
      data[i] = IS_NUMERIC(val) ? REAL(val)[i] : INTEGER(val)[i];
    } 
  } else {
    fprintf(stderr, "Unhandled conversion from R to Matlab %d\n", TYPEOF(val)); fflush(stderr);
    Rf_PrintValue(val);
    /* Fill in all the cases here.... */
  }

  /* Need a little more than this if nout > 1 */
  if(output)
    output[0]  = ans;

  return(ans);
}



/*
 Convert Matlab object to an R object.
*/
SEXP
convertToR(const mxArray *val)
{
  SEXP ans = R_NilValue;
  int i;

  int ndims, nelements;
  const int *dims;
  int numProtects = 0;

  if(!val)
    return(ans);

  if(mxIsCell(val)) {
    return(convertMatlabCell(val));
  } else if(mxIsStruct(val)) {
    return(convertMatlabStructToR(val));
  }


  ndims = mxGetNumberOfDimensions(val);
  dims = mxGetDimensions(val);

  nelements = mxGetNumberOfElements(val);

  if(mxIsUint8(val) || mxIsInt8(val) || mxIsInt16(val)
       || mxIsUint16(val) || mxIsInt32(val)  || mxIsUint32(val)
       || mxIsInt64(val)  || mxIsUint64(val)
       || mxIsSingle(val)) {
    ans = convertUint8ToR(val, ndims, dims, nelements);
  }
  else if(mxIsComplex(val) ||mxIsDouble(val) || mxIsLogical(val)) {

    int type;

    if(mxIsComplex(val)) {
      type = CPLXSXP;
    } else 
      type = mxIsLogical(val) ? LGLSXP : REALSXP;


      /* Allocate a vector or matrix or array */
    if(ndims == 2 && (dims[0] == 1 || dims[1] == 1)) {
      PROTECT(ans = allocVector(type, dims[0] * dims[1]));
      numProtects++;
    } else if(ndims == 2) {
      PROTECT(ans = allocMatrix(type, dims[0], dims[1]));
      numProtects++;
    } else {
      SEXP tmp;
      PROTECT(tmp = allocVector(INTSXP, ndims));
      numProtects++;

      for(i = 0; i < ndims; i++)
	INTEGER(tmp)[i] = dims[i];

      PROTECT(ans = allocArray(type, tmp));
      numProtects++;
    }


    for(i = 0; i < nelements; i++) {
      if(type == LGLSXP)
	LOGICAL(ans)[i] = mxGetLogicals(val)[i];
      else if(type == REALSXP)
	REAL(ans)[i] = mxGetPr(val)[i];
      else {
	COMPLEX(ans)[i].r = mxGetPr(val)[i];
	COMPLEX(ans)[i].i = mxGetPi(val)[i];
      }
    }

  } else if(mxIsChar(val)) {

    /*  mxCalcSingleSubscript() and mxGetNumberOfElements(val) work on the mxChar elements, 
        not the atomic strings. */

    char *tmp;
    int len, j, nels = 0;
    mxChar *els;

      /* Allocate a vector or matrix or array */
    if(ndims == 2 /* && (dims[0] == 1 || dims[1] == 1) */) {
      PROTECT(ans = allocVector(STRSXP, dims[0]));
      nels = dims[0];
      numProtects++;
    } else if(0 && ndims == 2) {
      PROTECT(ans = allocMatrix(STRSXP, dims[0], dims[1]));
      numProtects++;
    } else {
      int ctr;
      SEXP tmp;
      PROTECT(tmp = allocVector(INTSXP, ndims - 1));
      numProtects++;
      
      INTEGER(tmp)[0] = nels = dims[0];
      for(i = 2, ctr = 1; i < ndims; i++, ctr++) {
	INTEGER(tmp)[ctr] = dims[i];
        nels *= dims[i];
      }

      PROTECT(ans = allocArray(STRSXP, tmp));
      numProtects++;
    }

    len = dims[1];
    els = mxGetChars(val);

    tmp = (char *) R_alloc(len + 1, sizeof(char));

    for(i = 0 ; i < nels ; i++) {
      for(j = 0; j < len ; j++) 
	tmp[j] = els[dims[0]*j + i];
      tmp[len] = '\0';

      SET_STRING_ELT(ans, i, COPY_TO_USER_STRING(tmp));
    }

  } else {
    fprintf(stderr, "Haven't written converter for this Matlab type  %s yet\n",   mxGetClassName(val)); fflush(stderr);
  }

    UNPROTECT(numProtects);

  return(ans);
}





int
isMatlabPrimitiveType(mxClassID type)
{
  static mxClassID types[] = {mxCHAR_CLASS, mxLOGICAL_CLASS, mxDOUBLE_CLASS, 
                     mxINT8_CLASS
  };
  int i;


  for(i = 0; i < sizeof(types)/sizeof(types[0]); i++) {
    if(types[i] == type)
      return(1);
  }
  return(0);
}

/* Creates a list, but we need to be able to make this a vector
   if the elements are of a homogeneous basic type. 
*/
SEXP
convertMatlabCell(const mxArray *m)
{
  int nels, i, len;
  SEXP ans;
  mxClassID type, tmp;
  const mxArray *el;
  char *buf;

  nels = mxGetNumberOfElements(m);

  /* Check if we have a homegeneous collection of elements in the cell. */
  type = mxUNKNOWN_CLASS;
  for(i = 0; i < nels; i++) {
    el = mxGetCell(m, i);
    tmp = mxGetClassID(el);


    if((!mxIsChar(el) && mxGetM(el) * mxGetN(el) != 1 )
         || (type != mxUNKNOWN_CLASS && tmp != type) 
          || tmp == mxUNKNOWN_CLASS
           || !isMatlabPrimitiveType(tmp)) 
    {
      return(convertMatlabCellToRList(m, nels));
    }

    type = tmp;
  }

  switch(type) {

   case mxCHAR_CLASS:
     ans = allocVector(STRSXP, nels);
     break;
   case mxDOUBLE_CLASS:
     ans = allocVector(REALSXP, nels);
     break;
   case mxINT8_CLASS:
     ans = allocVector(INTSXP, nels);
     break;
   case mxLOGICAL_CLASS:
     ans = allocVector(LGLSXP, nels);
     break;
   default:
     break;
  }

  PROTECT(ans);

  for(i = 0; i < nels; i++) {
    el = mxGetCell(m, i);
    switch(type) {
      case mxCHAR_CLASS:
	len = mxGetN(el) * mxGetM(el) + 1;
	buf = (char *) R_alloc(len, sizeof(char)); 
	if(!buf) 
	  MATLAB_ERROR_MESSAGE("Cannot allocate space to hold Matlab string as R character vector");

	mxGetString(el, buf, len); 
	SET_STRING_ELT(ans, i, COPY_TO_USER_STRING(buf));
	break;

    case mxDOUBLE_CLASS:
      REAL(ans)[i] = mxGetPr(el)[0];
     break;
    case mxINT8_CLASS:
      INTEGER(ans)[i] = mxGetPr(el)[0];

     break;
    case mxLOGICAL_CLASS:
      LOGICAL(ans)[i] = mxGetLogicals(el)[0];
     break;
    default:
      break;
    }
  }

  UNPROTECT(1);

  return(ans);
}

SEXP
convertMatlabCellToRList(const mxArray *m, int nels)
{
  SEXP ans;
  const mxArray *el;
  int i;

  if(nels < 0)
    nels = mxGetNumberOfElements(m);

  PROTECT(ans = allocVector(VECSXP, nels));
  for(i = 0; i < nels; i++) {
    el = mxGetCell(m, i);
    SET_VECTOR_ELT(ans, i , convertToR(el));
  }

  UNPROTECT(1);

  return(ans);
}

/*
 This is for converting an S3 style object to Matlab.
 It doesn't deal with S4 objects.  We can deal with that
 separately and check the bit that indicates that a SEXP
 is an S4 object. 
*/
mxArray *
convertFromRObject(SEXP obj)
{
  char **names = NULL;
  int len, i;
  SEXP rnames;
  mxArray *ans;

  len = Rf_length(obj);
  rnames = GET_NAMES(obj);

  if(Rf_length(rnames)) {
    names = (char **) mxCalloc(len, sizeof(char*));
    for(i = 0; i < len; i++) {
      char *ptr;
      names[i] = CHAR(STRING_ELT(rnames, i));
      /* If the name has a ., then matlab will balk as it uses . for field within a structure.
         So we copy the name string and replace . with _ throughout. 
         Diagnosis came from Bitao Liu.
       */
#if 1
      if((ptr = strchr(names[i], '.'))) {
         int strLen = strlen(names[i]);
	 char *tmp;  
         /* we are modifying the contents of R values so we had better make certain
          we have a copy of them. Otherwise, we can create the copy in R with mxCalloc(), etc.
         */
	 tmp = mxCalloc(strLen+1, sizeof(char));
	 strcpy(tmp, names[i]);
	 ptr = tmp + (ptr - names[i]);

         while(ptr < tmp + strLen) {
           ptr[0] = '_';
           ptr = strchr(++ptr, '.');
         }
      }
#else
      if(strchr(names[i], '.')) {
        char *ptr;
	int strLen = strlen(names[i]);
	/* Hopefully Matlab will free these appropriately! */
        ptr = mxCalloc(strLen+1, sizeof(char));
        memcpy(ptr, names[i], strLen+1 * sizeof(char));
        names[i] = ptr;
	while(ptr < names[i] + strLen && (ptr = strchr(ptr, '.'))) {
	    ptr[0] = '_';
	    ptr++;
	}
      }
#endif
    }

    ans = mxCreateStructMatrix(1, 1, len, (const char **) names);
  } else {
    ans = mxCreateCellMatrix(1, len);
  }

  for(i = 0; i < len; i++) {
    mxArray *val;
    val = convertFromR(VECTOR_ELT(obj, i), 1, NULL);
    if(names)

      mxSetFieldByNumber(ans, 0, i, val);
    else
      mxSetCell(ans, i, val);
  }

  return(ans);
}


/*
 Convert a Matlab struct object to an R object
 by creating a list with the Matlab fields as
 elements of the R list.
*/
SEXP
convertMatlabStructToR(const mxArray *m)
{
  SEXP ans, names;
  int len, i;
  len = mxGetNumberOfFields(m);
  PROTECT(ans = allocVector(VECSXP, len));
  PROTECT(names = allocVector(STRSXP, len));

  for(i = 0; i < len ; i++) {
    const char *name;
    mxArray *el;

    name = mxGetFieldNameByNumber(m, i);
    fprintf(stderr, "%d) field name %s\n", i+1, name);fflush(stderr);
    SET_STRING_ELT(names, i, COPY_TO_USER_STRING(name));
/*XXX should this 0 be i */
    el = mxGetFieldByNumber(m, 0, i);
    if(el)
      SET_VECTOR_ELT(ans, i, convertToR(el));
  }

  SET_NAMES(ans, names);

    /*XXX Class name from Matlab but need more potentially! */
  SET_CLASS(ans, Rf_mkString(mxGetClassName(m)));

  UNPROTECT(2);
  return(ans);
}


#ifdef HAVE_MATLAB_COMPILER
#include "mclmcr.h"
#endif

/*
  Covers INT8, UINT8, INT16, UINT16, INT32, (map to integer)
         UINT32, INT64, UINT64  (map to numeric).
  SINGLE goes to numeric and we tag the Csingle attribute of TRUE onto the 
  resulting object.
*/
SEXP
convertUint8ToR(const mxArray *m, int ndims, const int *dims, int nelements)
{
  int numProtects = 0;
  SEXP ans;
  int type = INTSXP, i;
  void *els;
  mxClassID mtype;  

  mtype = mxGetClassID(m);

  /* Figure out which type/mode of primtive R object we need. */
  if(mtype == mxSINGLE_CLASS || mtype == mxUINT32_CLASS || mtype == mxINT64_CLASS || type == mxUINT64_CLASS)
    type = REALSXP;

  /* Allocate a vector, matrix or array in R to represent this object. */
  if(ndims == 2 && (dims[0] == 1 || dims[1] == 1)) {
      PROTECT(ans = allocVector(type, dims[0] * dims[1]));
      numProtects++;
  } else if(ndims == 2) {
      PROTECT(ans = allocMatrix(type, dims[0], dims[1]));
      numProtects++;
  } else {
      SEXP tmp;
      PROTECT(tmp = allocVector(INTSXP, ndims));
      numProtects++;

      for(i = 0; i < ndims; i++)
	INTEGER(tmp)[i] = dims[i];

      PROTECT(ans = allocArray(type, tmp));
      numProtects++;
  }

  /* Now fill in the elements. */
    els = mxGetData(m);
    for(i = 0; i < nelements; i++) {
      switch(mtype) {
#ifdef HAVE_MATLAB_COMPILER
      case mxINT32_CLASS:
  	INTEGER(ans)[i] = ((mxInt32 *) els)[i];
	break;
      case mxUINT16_CLASS:
  	INTEGER(ans)[i] = ((mxUint16 *) els)[i];
	break;
      case mxINT8_CLASS:
  	INTEGER(ans)[i] = ((mxInt8 *) els)[i];
	break;
      case mxUINT8_CLASS:
  	INTEGER(ans)[i] = ((mxUint8 *) els)[i];
	break;
      case mxINT16_CLASS:
  	INTEGER(ans)[i] = ((mxInt16 *) els)[i];
	break;
      case mxUINT32_CLASS:
  	REAL(ans)[i] = ((mxUint32 *) els)[i];
	break;
      case mxINT64_CLASS:
  	REAL(ans)[i] = ((mxInt64 *) els)[i];
	break;
      case mxUINT64_CLASS:
  	REAL(ans)[i] = ((mxUint64 *) els)[i];
	break;
      case mxSINGLE_CLASS:
  	REAL(ans)[i] = ((mxSingle *) els)[i];
	break;
#else
      case mxINT32_CLASS:
       INTEGER(ans)[i] = ( (int *)  els)[i];
        break;
      case mxSINGLE_CLASS:
	REAL(ans)[i] = ( (double *)  els)[i];
        break;
#endif
      default:
        PROBLEM "Unhandled conversion type from Matlab to R (%d)", mtype
        ERROR;  
        break;
      }
    }

    /* Put an attribute on this object identifying the original type in Matlab.
       This may be important to some applications.  */
    Rf_setAttrib(ans, Rf_install("MatlabMode"), mkString(mxGetClassName(m)));

    if(mtype == mxSINGLE_CLASS)
        Rf_setAttrib(ans, Rf_install("Csingle"), ScalarLogical(1));

    UNPROTECT(numProtects);
    return(ans);
}
