#
# Makefile for the SAND report Example files
# $Id: Makefile,v 1.10 2005/06/14 16:45:09 rolf Exp $
#
PFILES	= SANDExampleArticleNotstrict

FILES	= SANDExampleArticlestrict \
          SANDExampleReportNotstrict SANDExampleReportstrict

all:	pdf

pdf:	$(addsuffix .pdf, $(FILES) $(PFILES))

ps:	$(addsuffix .ps, $(FILES))

# Build one example using pdflatex
SANDExampleArticleNotstrict.pdf:	SANDExampleArticleNotstrict.tex SANDExample.bib SANDreport.cls
	pdflatex $<
	bibtex $(basename $<)
	pdflatex $<
	pdflatex $<

%.pdf:	%.ps
	ps2pdf13 $< $@

%.ps:	%.dvi
	dvips -Ppdf -o $@ $<

%.dvi:	%.tex SANDExample.bib SANDreport.cls
	latex $<
	bibtex $(basename $<)
	latex $<
	latex $<

clean:
	@rm -f $(addsuffix .aux, $(FILES) $(PFILES)) $(addsuffix .bbl, $(FILES) $(PFILES))
	@rm -f $(addsuffix .blg, $(FILES) $(PFILES)) $(addsuffix .lof, $(FILES) $(PFILES))
	@rm -f $(addsuffix .log, $(FILES) $(PFILES)) $(addsuffix .lot, $(FILES) $(PFILES))
	@rm -f $(addsuffix .toc, $(FILES) $(PFILES))
	@rm -f texput.log
	@rm -f Mark*.aux
	@rm -f SANDdistribution.aux

realclean:	clean
	@rm -f $(addsuffix .pdf, $(FILES) $(PFILES)) $(addsuffix .ps, $(FILES) $(PFILES))
	@rm -f $(addsuffix .dvi, $(FILES) $(PFILES)) $(addsuffix .out, $(FILES) $(PFILES))
