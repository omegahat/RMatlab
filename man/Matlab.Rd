\name{.Matlab}
\alias{.Matlab}
\alias{.MatlabMexCall}
\title{Call a Matlab function with R values as arguments}
\description{
These R functions allow one to invoke an arbitrary 
Matlab function from within R, passing R objects as the arguments to the Matlab
function.
The two functions are quite different in the model 
used to invoke the call.
\code{.Matlab} uses the more akward Engine API for 
interacting with Matlab. It allows access to 
Matlab as another process, on a different machine, etc.
but uses strings to evaluate the call and uses the 
Matlab workspace to temporarily store the
arguments and results. This has the (small) possibility 
of overwriting other values within the workspace and causing
erroneous results.
The \code{.MatlabMexCall} is more direct, invoking
a function directly without having to store values
in the workspace.  We can only use this mechanism
however when R was originally accessed from within
a Matlab session, and not when Matlab was embedded
within the R process.

}
\usage{
.Matlab(funcName, ..., .values = list(...), engine, .convert = TRUE, .resultNames = 1)
.MatlabMexCall(funcName, ..., .values = list(...), .nout = 1)
}
\arguments{
  \item{funcName}{a string giving the name of the Matlab function to invoke}
  \item{\dots}{the R values that are used as arguments to the Matlab function.
   These elements can be named  or not. Any named elements are assigned
   to the Matlab workspace and will be available in subsequent 
   calls to \code{\link{.MatlabEval}} via the specified names.
  
   For \code{.MatlabMexCall} does not use the names of the arguments.} 
  \item{.values}{this is an alternative way to specify the 
    arguments to the Matlab function. This is useful in programmatic
    (i.e. non-interactive) access to this function where one alread has the arguments
    in a list. }
  \item{engine}{a \code{MatlabEngine} object obtained from \code{\link{.MatlabInit}}.
   This is used to identify the particular engine to use, which potentially allows
    multiple instances of the Matlab engine to be used concurrently, e.g.
   on different machines}
  \item{.convert}{a logical value indicating whether to convert the result(s)
   to R objects or leave them as references to Matlab variables.
   Note that we could (and probably will) just use .resultNames to encode all the possible options
   of  convert or not,  or supress all results.
   }
  \item{.resultNames}{if specified, this can be either a number
   indicating how many values are expected (since Matlab can return
   multiple values) or the names of the variables in the
   Matlab workspace to use for storing the results.}
  \item{.nout}{an integer iving the number of results
   that are to be returned from the Matlab function.
   Since Matlab functions can have more than one 
   return value, this is a way to specify in  how many we
   are interested.
  }
}
\details{
Currently, this uses an akward mechanism via the Matlab engine API
to evaluate the function call rather than doing it directly in
the C code.
}
\value{

}
\references{
Matlab External Interface Guide, the Engine API.
Matlab User's Guide
}
\author{Duncan Temple Lang <duncan@wald.ucdavis.edu>}


\seealso{
 \code{\link{.MatlabInit}}
 \code{\link{.MatlabEval}}
 \code{\link{.MatlabGet}}
 \code{\link{.MatlabPut}}
}
\examples{
\dontrun{
  .Matlab('magic', 4)
}
}
\keyword{interface}
\concept{Inter-system interface}