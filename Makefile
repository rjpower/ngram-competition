all: ngrams
	time python ngrams.py
	time luajit ngrams.lua
	time ./ngrams

ngrams: ngrams.cc Makefile
	g++ -Wall -g0 -O3 -funroll-loops --static ngrams.cc -o ngrams -lboost_regex -lpthread

clean:
	rm -f ngrams
