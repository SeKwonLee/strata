all: *.tex *.bib
	-rm history-flag
	echo "starting clean"
	-pdflatex -interaction=nonstopmode ms.tex > /tmp/log1
	echo "bibtex"
	-bibtex ms
	echo "pdflatex clean step one"
	-pdflatex -interaction=nonstopmode ms.tex > /tmp/log2
	echo "pdflatex clean step two"
	-pdflatex -interaction=nonstopmode ms.tex > /tmp/log3

clean:
	rm *.bbl *.blg *.aux *.log *.out ms.pdf

