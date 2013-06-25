#include <stdio.h>
#include <string>
#include <map>
#include <vector>
#include <tr1/unordered_map>
#include <boost/regex.hpp>

using namespace std;
using namespace boost;

typedef tr1::unordered_map<string, int> IntMap;

static boost::regex garbageRe("(?:\\*)|(?:&[^;]+;)|(?:\\[\\[[^\\]]+:[^\\]]+\\]\\])");
static boost::regex bracketRe("[\\{\\}]");
static boost::regex splitterRe("\\w+");
static boost::regex numericRe("[-0-9]+");
static boost::regex textRe("<text[^>]+>([^<])*");
static boost::regex endTextRe("(.*)</text>");

static IntMap& badWords() {
  static IntMap* bad = NULL;
  if (!bad) {
    bad = new IntMap;
    (*bad)["after"] = 1;
    (*bad)["also"] = 1;
    (*bad)["article"] = 1; 
    (*bad)["date"] = 1; 
    (*bad)["defaultsort"] = 1; 
    (*bad)["external"] = 1; 
    (*bad)["first"] = 1; 
    (*bad)["from"] = 1; 
    (*bad)["have"] = 1; 
    (*bad)["html"] = 1; 
    (*bad)["http"] = 1; 
    (*bad)["image"] = 1; 
    (*bad)["infobox"] = 1; 
    (*bad)["links"] = 1; 
    (*bad)["name"] = 1; 
    (*bad)["other"] = 1; 
    (*bad)["preserve"] = 1; 
    (*bad)["references"] = 1; 
    (*bad)["reflist"] = 1; 
    (*bad)["space"] = 1; 
    (*bad)["that"] = 1; 
    (*bad)["this"] = 1; 
    (*bad)["title"] = 1; 
    (*bad)["which"] = 1; 
    (*bad)["with"] = 1; 
  }
  return *bad;
}

static IntMap* freqs = new IntMap();

class BZ2Reader {
private:
  FILE* bzIn;
  string docText;
public:
  BZ2Reader(const string& src) {
    bzIn = popen(("bzcat " + src).c_str(), "r");
  }
  vector<string> readDoc() {
    docText.clear();

    char buf[1024];        
    bool inText = false;
    cmatch tMatch;
    while (1) {
      fgets(buf, 1024, bzIn);      
      if (strstr(buf, "<text ") != NULL && regex_search(buf, tMatch, textRe)) {
        docText.append(tMatch[1].first, tMatch[1].length());
        inText = true;
      }

      if (strstr(buf, "</text") != NULL && regex_search(buf, tMatch, endTextRe)) {
        docText.append(tMatch[1].first, tMatch[1].length());
        break;
      }

      if (inText) { docText.append(buf); }
    }

    docText = regex_replace(docText, garbageRe, "");
    for (size_t i = 0; i < docText.size(); ++i) {
      docText[i] = tolower(docText[i]);
    }

    vector<string> out;
    sregex_iterator sstart(docText.begin(), docText.end(), splitterRe);
    sregex_iterator send;
    smatch sMatch;
    for (; sstart != send; ++sstart) {
      const string& v = (*sstart)[0].str();
      if (badWords().find(v) != badWords().end() ||
          regex_search(v, numericRe)) {
        out.push_back("");
      } else {
        out.push_back(v);
      }
    }

    return out;
  }
};

void ngrams(vector<string> words) {
  string accum;
  for (size_t i = 0; i < words.size(); ++i) {
    accum.clear();
    for (size_t j = i; j < min(i + 5, words.size()); ++j) {
      if (words[j] == "") { break; }
      accum += words[j] + " ";
      (*freqs)[accum] += 1;
    }
  }
}

int main() {
  BZ2Reader docs("./pages.xml.bz2");
  for (int i = 0; i < 500; ++i) {
    vector<string> s = docs.readDoc();
    ngrams(s);
    if (i % 10 == 0) {
      fprintf(stderr, "Working... %10d %10zd\r", i, s.size());
    }
  }

  FILE* fOut = fopen("/tmp/freqs.c.txt", "w");
  for (IntMap::iterator i = freqs->begin(); i != freqs->end(); ++i) {
    fprintf(fOut, "%s\t%d\n", i->first.c_str(), i->second);
  }
  fclose(fOut);
}
