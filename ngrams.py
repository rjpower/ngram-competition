#!/usr/bin/env python2.7

import csv, re, gzip, zlib, os, sys
from random import randint
from collections import defaultdict

bad = set(['after', 'also', 'article', 'date', 'defaultsort', 'external', 'first', 'from',
          'have', 'html', 'http', 'image', 'infobox', 'links', 'name', 'other', 'preserve',
          'references', 'reflist', 'space', 'that', 'this', 'title', 'which', 'with',])

garbage_re = re.compile(r'(?:\*)|(?:&[^;]+;)|(?:\[\[[^\]]+:[^\]]+\]\])')
bracket_re = re.compile(r'[\{\}]')
splitter_re = re.compile(r'[^\w-]+')
numeric_re = re.compile(r'[-0-9]+')
index_freqs = defaultdict(int)
N = 5

class BZ2Reader(object):
    def __init__(self, src):
        self.b = os.popen('bzcat ' + src, 'r')
        self.d = []

    def read_doc(self):
      del self.d[:]

      while 1:
        l = self.b.readline()
        if "<id" in l: self.id = re.search('<id>(.*)</id>', l).group(1)
        if "<text" in l and not "</text" in l:
          self.d.append(l[l.find('>'):])
          break

      while 1:
        l = self.b.readline().strip()        
        if "</text" in l:
          self.d.append(l[:l.find('<')])
          break
        else:
          self.d.append(l)

      if not self.d:
        return None

      contents = ' '.join(self.d)
      contents = garbage_re.sub('', contents)
      contents = contents.lower()
      return contents

docs = BZ2Reader('./pages.xml.bz2')

def process_document2(d):
   words = splitter_re.split(d)
   for i in xrange(len(words)):
     w = words[i]
     if len(w) >= 12 or w in bad or numeric_re.search(w):
         words[i] = None
   
   for i in xrange(0, len(words)):
     for j in xrange(1, N+1):
       if not words[i+j-1]:
         break
       k = ' '.join(words[i:i+j])
       index_freqs[k] += 1
       
def dump(index_freqs, dump_count):
  print >>sys.stderr, 'Dumping frequencies...'
  f = open('/tmp/freqs.py.txt', 'w')
  for k, v in index_freqs.iteritems():
    print >>f, k, '\t', v
  f.close()

def main():
  global index_freqs
  
  dump_count = 0
  c = 0
  while 1:
    d = docs.read_doc()
    if d == None:
      break

    process_document2(d)
    c += 1
    
    if c == 500:
      break

  print 'Dumping...'
  dump(index_freqs, dump_count)
  

if __name__ == '__main__':
  main()

