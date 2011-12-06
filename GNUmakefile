
install: configure
	R CMD INSTALL .

build: configure
	(cd .. ; R CMD build RMatlab)

configure: configure.in
	autoconf

check:
	R CMD check .
