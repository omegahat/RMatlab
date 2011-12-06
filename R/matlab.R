# These are the Matlab engine API functions.

.MatlabInit =
# Can use /Automation to connect to existing Matlab process. 
function(args = "matlab -nodisplay -nojvm -nosplash")
{
  e = .Call("RMatlab_init",  paste(args, collapse = " "), PACKAGE = "RMatlab")

  class(e) = c("MatlabEngine", "MatlabInterface")

  e
}

.MatlabClose =
function(engine)
{
  if(!inherits(engine, "MatlabEngine"))
    stop(".MatlabClose must be called with a MatlabEngine object")

  .Call("RMatlab_close", engine, PACKAGE = "RMatlab")
} 


.MatlabEval =
function(command, engine = getMatlabInterface())
{
  .Call("RMatlab_evalString", as.character(command), engine, PACKAGE = "RMatlab")
}


.MatlabGet =
function(what, engine = getMatlabInterface(), multi = FALSE, .convert = TRUE, where = "base")
{
  .convert = rep(as.logical(.convert), length = length(what))
  els = .Call("RMatlab_getVariable", as.character(what), as.character(where), 
                .convert, engine, PACKAGE = "RMatlab")

  if(!multi && length(els) == 1)
     els = els[[1]]
  else
     names(els) <- what

  els
}

.MatlabPut =
function(..., engine = getMatlabInterface(), .values = list(...), where = "base")
{
  if(length(names(.values)) == 0 || any(names(.values) == ""))
     stop("All elements must have names")

  .Call("RMatlab_setVariable", names(.values), .values, as.character(where), engine, PACKAGE = "RMatlab")
}




.Matlab =
  #
  # This is the engine form of the .Matlab function call.
  # It uses the workspace to temporarily hold the arguments
  # and the results as named Matlab variables. This is not
  # ideal!
  #
function(funcName, ..., .values = list(...), engine = getMatlabInterface(), 
          .convert = TRUE, .resultNames = 1)
{

  if(inherits(engine, "MexInterface")) {
    return(.MatlabMexCall(funcName, ..., .values = .values, .resultNames))
  }
  
   # Use the names in the ... if they are provided
   # as variable names in Matlab workspace. 
   # Those that are provided  are not removed after the call.
   # This allows them to be reused.
   # This is not terrifically  useful in that 
   # we would need to access these in a command
   # .MatlabEval().
  tempVars <- character(0)

  if(length(names(.values)) == 0) 
    w = rep(TRUE, length(.values))
  else 
    w = names(.values) == ""

   # Set the names of the arguments that don't have a name
  if(length(w) || any(w)) {
     tempVars <- names(.values)[w] <- paste("r_arg", 1:sum(w), sep = "_")
  }

    # Put these arguments into the Matlab workspace and we
    # will meet them on the other side!
  .MatlabPut(.values = .values, engine = engine)


    # But arrange to remove them when this call is complete.   
  if(length(tempVars))
    on.exit(.MatlabRemove(tempVars, engine = engine))


    # Now create the Matlab command to evaluate
    # referencing the arguments now in the workspace.

  if(!is.character(.resultNames))
   .resultNames = paste("r_result", 1:.resultNames, sep = "_")

  resultAssign = paste(paste(.resultNames, sep = ", " ), "=")

  cmd = paste(resultAssign, funcName, "(", 
                  paste(names(.values), collapse=", "),
               ");")
  .MatlabEval(cmd, engine = engine)

   # And now fetch the result.
  if(length(.resultNames)) {
     ans = .MatlabGet(.resultNames, engine = engine, .convert = .convert, multi = length(.resultNames) != 1)

     .MatlabRemove(.resultNames, engine = engine)
  
     ans
  } else
    invisible(NULL) 
}


#############################################################################################
# Methods for accessing the Matlab interpreter via a reference to it.
# e.g. e$foo(), e[["a"]], e["a", "b", "c"] and e["ab"] <- 1:3

"$.MatlabInterface" <-
function(x, name)
{
  function(...)
     .Matlab(name, ..., engine = x)

}

"[[.MatlabInterface" <-
function(x, i)
{
  .MatlabGet(as.character(i), engine = x, multi = FALSE)
}

"[.MatlabInterface" <-
function(x, i, j, ...)
{
  els <- sapply(list(...), as.character)
  if(!missing(i))
     els <- c(as.character(i), els)
  if(!missing(j))
     els <- c(as.character(j), els)  
  
  .MatlabGet(as.character(els), engine = x, multi = TRUE)
}  



"[<-.MatlabInterface" <-
function(x, i, j, ..., value)
{
  els <- sapply(list(...), as.character)
  if(!missing(i))
     els <- c(as.character(i), els)
  if(!missing(j))
     els <- c(as.character(j), els)

  if(length(els) > 1 && !is.list(value) && length(value) != length(els))
    stop("There must be as many variable names as values")

  if(length(els) == 1)
    value = list(value)

  names(value) <- els

  .MatlabPut(engine = x, .values = value,  multi = TRUE)

  x
}  




getMatlabInterface =
function()
{
 if(exists(".MexInterface", env = globalenv()))
   return(get(".MexInterface", env = globalenv()))

 .Call("RMatlab_getDefaultEngine")
}


#############################################################################################

.MatlabRemove =
function(varNames, engine = getMatlabInterface())
{
  cmd <- paste("clear", paste(varNames, collapse = " "))
  .MatlabEval(cmd, engine = engine)
}


.REvalString =
function(cmd, .convert = TRUE, env = globalenv())
{
   eval(parse(text = cmd), env = env)
}

###############################################################################################

# Mex interface, i.e. when calling Matlab from R but only when R was started from 
# withhin the Matlab process. In other words, this requires that Matlab was started
# first and the only way we got to call an R function was via a MEX function in Matlab.   


.MatlabMexCall =
#
# This can only be called if R was called from a MEX file,
# and not if we embed Matlab in R by starting the Matlab engine.
#
function(funcName, ..., .values = list(...), .nout = 1)
{

 ans = .Call("RMatlab_invoke", as.character(funcName), .values, as.integer(.nout), PACKAGE = "RMatlab")
 if(missing(.nout) && length(ans) == 1)
   ans = ans[[1]]

 ans
}


#############################################################
# Graphics handles.

getMatlabGraphicsProperty <-
function(property, handle)
{

  checkMex()
  
  if(!inherits(handle, "MatlabGraphicsHandle"))
    stop("Need a MatlabGraphicsHandle object")
  
  ans = .Call("RMatlab_mexGetProperty", as.numeric(handle), as.character(property))

  names(ans) <- as.character(property)
  
  ans
}

setMatlabGraphicsProperty <-
function(value, property, handle)
{
  checkMex()

  if(!is.list(value))
    stop("value must be a list")


  if(missing(property)) {
    if(length(names(value)) && all(names(value) != ""))
      property = names(value)
    else
      stop("You must supply names for the properties being set")
  }
  
  if(!inherits(handle, "MatlabGraphicsHandle"))
    stop("Need a MatlabGraphicsHandle object")
  
  .Call("RMatlab_mexSetProperty", as.numeric(handle), as.character(property), value)
}


checkMex <-
  # internal
function()
{
  if(!exists(".MexInterface", env = globalenv()))
    stop("Matlab is not being accessed via the MEX interface")
}
