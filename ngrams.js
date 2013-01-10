var util = require('util');
var process = require('child_process');
var fs = require('fs');

var bz = process.spawn('bzcat', ['/scratch/enwiki-latest-pages-articles.xml.bz2']);
var reTitle = /<title>([^<]+)/;
var reText = /<text[^>]+>([^<]+)/;
var reTextSplit = /<\/text>/;
var reNotWord = /[^\w]+/;
var reGarbage = reNotWord;
var reNumber = /[0-9]/;
var numCalls = 0;
var counts = {};

function ngrams(title, doc) {
  for (var i = 0; i < doc.length; ++i) {
    var w = doc[i];
    if (w.length >= 12 || reNumber.test(w)) { 
      w = null;
    }
    doc[i] = w;
  }

  for (var i = 0; i < doc.length; ++i) {
    for (var j = i; j < Math.min(doc.length, i + 6); ++j) {
      if (!doc[j]) {
        break;
      }
      var k = doc.slice(i, j).join(" ");
      c = 0;
      if (k in counts) { c = counts[k]; }
      counts[k] = c + 1;
    }
  }    
}

var buffer = new Buffer(10 * 1000 * 1000);
var bPos = 0;
bz.stdout.on('data', function (data) {
    data.copy(buffer, targetStart=bPos);
    bPos += data.length;
    
    var blob = buffer.toString('binary', 0, bPos).replace(reGarbage, ' ').toLowerCase();
    var docs = blob.split(reTextSplit);
    bPos = 0;

    for (idx in docs) {
      var text = docs[idx];
      var titleMatch = reTitle.exec(text);
      var textMatch = reText.exec(text);

      if (!titleMatch || !textMatch) {
        continue;
      }

      ngrams(titleMatch[1], textMatch[1].split(reNotWord));

      numCalls += 1;

      if (numCalls == 500) { 
        var countFD = fs.openSync('/tmp/freqs.v8.txt', 'w');
        var blob = "";
        for (var k in counts) {
          blob += k + '\t' + counts[k] + '\n';
          if (blob.length > 1000000) {
            fs.writeSync(countFD, blob);
            blob = '';
          }
        }
        fs.writeSync(countFD, blob);
        fs.closeSync(countFD);
        counts = {}    
        bz.stdout.destroy();
        return;
      }
    }
});
bz.stdin.end();
