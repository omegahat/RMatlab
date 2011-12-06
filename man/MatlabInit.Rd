\name{.MatlabInit}
\alias{.MatlabInit}
\alias{.MatlabClose}
\title{Start or stop a Matlab engine from R}
\description{
 These functons create or close a Matlab session from within R that 
 can be used to evaluate Matlab commands and 
 manage the Matlab workspace directly from R.
}
\usage{
.MatlabInit(args = "\0 -nojvm -nosplash -nodisplay")
.MatlabClose(engine)
}
\arguments{
  \item{args}{the command line used to start Matlab.
    See the documentation for the Matlab routine engOpen in the Matlab Engine API
     for details. On Microsoft Windows, this is ignored. 
     One can create the matlab session in different ways, on different machines,
     etc. using these arguments.
   }
  \item{engine}{the \code{MatlabEngine} object that is a reference to the engine
   to be closed.}
}
\details{
}
\value{
 An object of class \code{MatlabEngine}
 that  is a reference to the C-level data structure
 that represents the engine. This is used in the
 other .Matlab functions to identify the particular engine.
}
\references{
Matlab External Interface Guide, the Engine API.
Matlab User's Guide
}
\author{Duncan Temple Lang <duncan@wald.ucdavis.edu>}

\seealso{ 
 \code{\link{.MatlabEval}}
 \code{\link{.MatlabPut}}
 \code{\link{.MatlabGet}}
}
\examples{

\dontrun{
 .MatlabInit()
}
}
\keyword{interface}
\concept{Inter-system interface}
