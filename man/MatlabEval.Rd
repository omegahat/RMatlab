\name{.MatlabEval}
\alias{.MatlabEval}
\title{Evaluate a Matlab command (from R)}
\description{
  This function allows the R programmer to send a Matlab command
to the Matlab engine to be evaluated.
This command is evaluated as if one typed it at the Matlab
prompt, including accessing variables and assigning results
to the Matlab workspace for use in subsequent calls.
}
\usage{
.MatlabEval(command, engine)
}
\arguments{
  \item{command}{the Matlab command to be evaluated as if it were typed at the Matlab prompt.}
  \item{engine}{the \code{MatlabEngine} reference object,  obtained via a call to 
  \code{\link{.MatlabInit}}. }
}

\value{
 An integer indicating whether the command
was evaluated without error or not.
(Currently, not certain if we can detect an error
from the result!) 
}
\references{
Matlab External Interface Guide, the Engine API.
Matlab User's Guide
}
\author{Duncan Temple Lang <duncan@wald.ucdavis.edu>}


\seealso{
 \code{\link{.MatlabPut}}
 \code{\link{.MatlabGet}}
 \code{\link{.Matlab}}
 \code{\link{.MatlabInit}}
}
\examples{
\dontrun{

}
}

\keyword{interface}
\concept{Inter-system interface}