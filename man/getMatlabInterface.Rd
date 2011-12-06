\name{getMatlabInterface}
\alias{getMatlabInterface}
\title{Get object representing R interface to Matlab}
\description{
  This function returns an object that identifies the
  interface from R to Matlab. This is either a reference
  to an internal (i.e. C-level) Matlab Engine object
  or a regular R object and the class of the object
  identifies whether the R-to-Matlab interface is
  using MEX (the direct interface) or
  the Engine API. 
}
\usage{
getMatlabInterface()
}
\details{
  If the variable \code{.MexInterface} exists
  in the R work space (global environment),
  then that object is returned.
  Otherwise, we access the C code
  for this package and return a reference to the
  default, or last created, Engine object.
}
\value{
  If the MEX interface is being used, this returns
  an object of class \code{.MexInterface}.
  If the Engine API is being used, this returns
  a reference to the internal Engine data structure
  and makes it an object of class \code{MatlabEngine}.

  Both objects are opaque data types and it is not intended
  that the value within the object be used directly in the R code. 
  Also both objects are of class (or extend) \code{MatlabInterface}
  and this allows the resulting object to be used with the operator
  overloading for \code{$}, \code{[[}, \code{[} and \code{[<-}.
}

\references{
  Matlab External Interface Guide
  \url{http://www.mathworks.com/access/helpdesk/help/techdoc/apiref/apiref.html}
   The Engine API.
 Matlab User's Guide
}
\author{Duncan Temple Lang <duncan@wald.ucdavis.edu>}

\seealso{
 \code{\link{.Matlab}}
 \code{\link{.MatlabMexCall}}  
 \code{\link{.MatlabInit}}
 }

\examples{
\dontrun{
 e = getMatlabInterface()

  # Asssign the vector in R to the Matlab variable "a"
 e["a"] <- 1:3

  # Assign the two R objects in the list to variables in Matlab.
 e["b", "xyz"] <- list(rnorm(10), letters)

  # Retrieve the value of the Matlab variable "a"
 e[["a"]]

  # Retrieve the value of the two Matlab variables "b" and "xyz"
 e["b", "xyz"]

  # Call the Matlab function magic
 e$magic(3)
}
}

\keyword{interface}
\concept{Inter-system interface}
