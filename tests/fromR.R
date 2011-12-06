# This tests the Engine API

library(RMatlab)
e = .MatlabInit()

.Matlab("rand", 1, 1, engine = e)


e$rand(1, 2)
.MatlabEval("o = 1:3", engine = e)
.MatlabEval("p = magic(3)", engine = e)
e[["o"]]

e["o", "p"]

e["a"] <- "abc"




i = .Matlab('imfinfo', '/home/duncan/rggobi.png')

data = .Matlab('imread', '/home/duncan/rggobi.png')
