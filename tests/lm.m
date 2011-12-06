# Example from Ping-Shi Wu
initializeR({'RMatlab' '--silent' '--vanilla'})
callR('assign', 'x', 1:10)
callR('assign','y',2:11)
d = callR('.REvalString', 'lm(y~x)')          


# Note that the terms & call fields are not transferred.
# Need to implement the opaque references.
 

