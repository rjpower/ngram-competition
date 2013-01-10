# Ngram competition

For some reason I always want to extract n-grams from some documents.

I always write things first in Python, then regret it almost immediately
because it's slower then I want (which is unfair, because I'm never happy
until it's faster then I can blink).  So I wrote a few scripts in various
languages to see how fast I could realistically count n-grams.

    Python: 12.59 seconds / 500 documents
    Luajit: 14.65 seconds / 500 documents
    NodeJS: 11.06 seconds / 500 documents
    C++:    5.04 seconds / 500 documents

