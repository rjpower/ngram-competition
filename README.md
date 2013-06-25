# Ngram competition

For some reason I always want to extract n-grams from some documents.

I always write things first in Python, then regret it almost immediately
because it's slower then I want (which is unfair, because I'm never happy until
it's faster then I can blink).  So I wrote a few scripts in various languages
to see how fast I could realistically count n-grams.  For 500 documents, here's
how languages stack up:

    Python: 6.58 seconds
    LuaJIT: 8.61 seconds
    C++:    4.46 seconds
    NodeJS: 5.77 seconds

