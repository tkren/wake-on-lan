info_TEXINFOS = wol.texi
MAINTAINERCLEANFILES = Makefile.in texinfo.tex

wol.pdf:
	texi2pdf wol.texi

wol.html:
	texi2html -no-section_navigation wol.texi

clean-local:
	rm -f wol.pdf wol.html
