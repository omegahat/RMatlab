# These are simple functions to test the conversion of 
# different R objects to Matlab objects.

# To use this, source this into R (from within Matlab) and then call
# the functions from Matlab 
#

################################################################
#
# Just for testing what goes across the converter.
# These will go into the tests/ directory later.

Identity =
function(...)
{
  args = list(...)
  cat("# of arguments =", length(args), "\n")
  print(args)

  print(lapply(args, dim))

  if(exists(".MexInterface", env = globalenv()))
    cat("Called from", .Call("RMatlab_getMexFunctionName"), "\n")
  
  TRUE
}


s3Test =
function()
{
 
  ans = list(firstName = "Duncan",
   	     lastName = "Temple Lang",
             SSN = 123456789,
             email = c("duncan@wald.ucdavis.edu", "duncan@r-project.org"))

  class(ans) <- "MyS3Class"
  ans
}

m = 
function()
{
 matrix(letters[6], 3, 2)
}


l =
function(withNames = TRUE)
{

  ans =
    list(i = as.integer(1),
       real = pi,
       str = c("abc", "defghi"),
       logical = rep(c(TRUE, FALSE), 3),
       matrix = matrix(1:12, 4, 3))

  if(!withNames)
    names(ans) <- NULL

  ans
}

nas =
  # Test NAs and Inf in numeric and integer values.
function(asInteger = FALSE)
{
  ans = c(1, 2.3, NA, 2, 1./0)

  if(asInteger)
    ans = as.integer(ans)

  print(ans)
  
  ans
}

complex =
  function()
{
   rep(1:4,len=9) + 1i*(9:1)
}

naStrings =
function()
{
  c(NA, "a", "bcde")
}  

int =
function()
  1:5



narray =
function(type = "integer")
{
  x = switch(type, integer =  1:24,
               numeric = as.numeric(1:24),
               character = as.character(1:24),
               logical = rep(c(TRUE, FALSE), length = 24),
               complex = 1:24 + 1i*rep(c(.5, 1.3), length = 24))

  array(x, c(4, 3, 2))
}  

