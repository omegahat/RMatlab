\name{getMatlabGraphicsProperty}
\alias{setMatlabGraphicsProperty}
\alias{getMatlabGraphicsProperty}
%- Also NEED an '\alias' for EACH other topic documented here.
\title{Accessors for Matlab Graphics Handle properties}
\description{
  These functions provide vectorized accessors to
  get and set properties associated with Matlab graphics handle
  objects.
  These can only be called if we are interfacing to Matlab
  using the MEX interface, i.e. when R has originally
  been called from a top-level Matlab session.
  This does not work with the Engine interface (see \code{\link{.MatlabInit}}).


  These are vectorized functions in that one can access
  multiple properties in a single call
  using a character vector containing the names and a list
  for specifying the values.

  One can obtain the graphical handle object via a call to an
  appropriate Matlab function, e.g gcf, gco
}
\usage{
getMatlabGraphicsProperty(property, handle)
setMatlabGraphicsProperty(value, property, handle)
}
%- maybe also 'usage' for other objects documented here.
\arguments{
  \item{property}{a character vector giving the names of the properties.}
  \item{value}{a list containing the values to be used for the
    associated properties. It is good practice to
    omit the property argument and explicitly use the
    property names as the names of the elements of the list. }
  \item{handle}{the Matlab graphics handle object in which we
      access the properties.}
  }
\value{
  \code{getMatlabGraphicsProperty} returns a named list
  giving the current values for the specified properties.

  \code{setMatlabGraphicsProperty} returns a logical vector indicating
  the elements for which setting the property was successful.
}

\references{
Matlab External Interface Guide, the Engine API.
Matlab User's Guide
}
\author{Duncan Temple Lang <duncan@wald.ucdavis.edu>}

\seealso{\code{\link{.MatlabMexCall}}}
\examples{

}
\keyword{interface}
\concept{Inter-system interface}

