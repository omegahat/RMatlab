DOCS=index.html ../inst/docs/outline.pdf ../README ../Todo.html ../Changes.html ../Bugs.html ../FAQ.html

installDocs shipDocs: index.html
	scp $(DOCS) www.omegahat.org:/home3/WebSites/Omega/RMatlab

index.html: index.html.in configure ../DESCRIPTION
	./configure

configure: configure.in
	autoconf


%.pdf: %.tex
	$(MAKE) -C $(@D) $(@F)


