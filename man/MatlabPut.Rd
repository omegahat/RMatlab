\name{.MatlabPut}
\alias{.MatlabPut}
\alias{.MatlabGet}
\title{Access the Matlab workspace and its variables.}
\description{
  These functions allow the R user to assign or retrieve 
  one or more Matlab variables from within R.
  One can assign R objects to the Matlab workspace
  and retrieve the values of existing Matlab variables
  as R objects.
}
\usage{
.MatlabPut(..., engine, .values = list(...), where = "base")
.MatlabGet(what, engine, multi = FALSE, .convert = TRUE, where = "base")
}
\arguments{
  \item{\dots}{named R objects that are marshalled to Matlab and assigned in the 
    Matlab workspace using the names given in the R call,
    e.g. \code{.MatlabPut(x = 1, myStrings = c("a", "b"))}
   }
 \item{what}{names of the Matlab variables whose values are to be retrieved.}
  \item{.values}{a named list of R objects to store as Matlab variables.
    This is provided as an equivalent to the \dots but to be used for programmatic access to 
    this function rather than interactive use.}
  \item{engine}{the Matlab engine reference used to identify the engine in which the 
   variables are to be stored or retrieved.}
  \item{where}{ currently ignored but may be used if we 
    implement a MEX-based interface rather than an Engine-based interface,
    i.e. using the different Matlab external interfaces.}
  \item{multi}{ a logical value indicating whether the results should be left
   in a list (\code{TRUE}), or if there is only one variable being requested
   to return that directly rather than as the only element in a list.
   \code{FALSE} is useful for interactive use, but \code{multi = TRUE}
   is the same regardless of the number of variables requested and useful
   in programmatic use. 
   }
  \item{.convert}{a logical vector indicating which variables to 
    convert to R objects and which to leave as references to Matlab objects.
    This is a vector parallel to the \code{what} vector of names
    so that the caller can specify which variables to convert and which to leave
    as references in a single call.
    This is recycled to have the same length as \code{what}.}
}
\details{

}
\value{
 If  \code{multi} is \code{TRUE}, a list
 with as many elements as variables requested.
 If \code{multi} is \code{FALSE} and the number of variables
  requested is 1, then just that object is returned.
}

\references{
Matlab External Interface Guide, the Engine API.
Matlab User's Guide
}
\author{Duncan Temple Lang <duncan@wald.ucdavis.edu>}

\seealso{
 \code{\link{.Matlab}}
 \code{\link{.MatlabEval}}
 \code{\link{.MatlabInit}}
 }
\examples{
}
\keyword{interface}
\concept{Inter-system interface}

