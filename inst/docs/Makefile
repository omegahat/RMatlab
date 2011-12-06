outline.pdf:

ifndef OMEGA_HOME
OMEGA_HOME=../../../..
endif
%.pdf: %.tex
	(export TEXINPUTS=${OMEGA_HOME}/Docs:: ; pdflatex $<)
	bibtex $*
	(export TEXINPUTS=${OMEGA_HOME}/Docs:: ; pdflatex $<)

