# Location of the R installation.
ifndef R_HOME
  R_HOME=$(shell R RHOME)
endif

# Where to find R header files
R_MEX_CFLAGS=-I$(R_HOME)/include
# Where to find R shared library.
R_MEX_LIBS=-L$(R_HOME)/lib -lR 

# Not used:  -leng


# Used to use  -f ./mexopts.sh
# but we just need to remove the use of the 
#   -Wl,--version-script,$TMW_ROOT/extern/lib/$Arch/$MAPFILE
# which makes the variables local.
#
#XXX For Solaris, this should be -G -mt
# or more accurately, the value of LDFLAGS
# without the text  -[^ ]*.map
R_SO_MEX_CFLAGS=LDFLAGS='-pthread -shared -m32 '


# Arguments for the mex compiler
MEX_ARGS= -argcheck $(R_MEX_CFLAGS) 

# the mex compiler to create the loadable objects.
MEX=/usr/local/bin/mex

MEX_LD_EXTENSION=mexglx


##################################################################################


all: RMatlab.so callR initializeR callNamedR  installMex

installMex: callR initializeR callNamedR 

.PHONY: callR initializeR callNamedR
initializeR: initializeR.o

callR.c callNamedR.c: wrapper.c

callR: callR.c convert.c
	$(MEX) $(MEX_ARGS) $(R_MEX_LIBS) $^

callNamedR: callNamedR.c convert.c 
	$(MEX) $(MEX_ARGS) $(R_MEX_LIBS) $^

installMex:
	cp *.$(MEX_LD_EXTENSION) ../inst/mex

%.o: %.c
	$(MEX) $(MEX_ARGS) $(R_MEX_LIBS) $<

RMatlab.o: RMatlab.c
	$(R_HOME)/bin/R CMD COMPILE $^

Rconvert.o: convert.c
	$(R_HOME)/bin/R CMD COMPILE $^


USE_MEX_FOR_R=1


ifdef USE_MEX_FOR_R

# Do we need to use -f engopts.sh here. We do on Solaris.
# Or we have to specify the location of the library.
# How do we find the location of engopts.sh. Need to find 

RMatlab.so: RMatlab.c convert.c RMatlabConvert.h
	@echo "Creating RMatlab.so"
	$(MEX) -output $@  RMatlab.c convert.c $(R_MEX_LIBS) $(R_SO_MEX_CFLAGS) $(MEX_ARGS) -leng
	mv RMatlab.so.$(MEX_LD_EXTENSION) $@
else
RMatlab.so: RMatlab.o Rconvert.o RMatlabConvert.h
	$(R_HOME)/bin/R CMD SHLIB -o $@ RMatlab.o convert.o
endif



clean:
	-rm *.mexglx *.o *.so
