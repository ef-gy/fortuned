/* Fortune cookie handling.
 *
 * The code in these files allows parsing fortune cookies from their on-disk
 * representation.
 *
 * See also:
 * * Project Documentation: https://ef.gy/documentation/fortuned
 * * Project Source Code: https://github.com/ef-gy/fortuned
 * * Licence Terms: https://github.com/ef-gy/fortuned/blob/master/COPYING
 *
 * @copyright
 * This file is part of the fortuned project, which is released as open source
 * under the terms of an MIT/X11-style licence, described in the COPYING file.
 */

#if !defined(FORTUNED_FORTUNE_H)
#define FORTUNED_FORTUNE_H

#include <ef.gy/global.h>

#include <cxxhttp/httpd.h>

#include <dirent.h>
#include <cstdlib>
#include <fstream>
#include <iostream>

namespace fortuned {
class cookie {
 public:
  const std::string file;
  const std::size_t id;

  cookie(bool pROT13, const std::string &pData, const std::string &pFile,
         const std::size_t pID)
      : file(pFile), id(pID), rot13(pROT13), data(pData) {}

  operator std::string(void) const {
    std::string r = data;

    if (rot13) {
      for (size_t i = 0; i < r.size(); i++) {
        char c = r[i];

        if ((c >= 'a' && c <= 'm') || (c >= 'A' && c <= 'M')) {
          r[i] = c + 13;
        }
        if ((c >= 'n' && c <= 'z') || (c >= 'N' && c <= 'Z')) {
          r[i] = c - 13;
        }
      }
    }

    return r;
  }

 protected:
  bool rot13;
  const std::string data;
};

class fortune {
 public:
  fortune(void) : cookies(), data() {}

  bool prepare(const std::string &dir, const bool doROT13 = false) {
    static const std::regex dataFile(".*/[a-zA-Z-]+");
    std::smatch matches;
    DIR *d = opendir(dir.c_str());

    if (!d) {
      return false;
    }

    struct dirent *en;

    while ((en = readdir(d)) != 0) {
      std::string e = dir + en->d_name;

      if (regex_match(e, matches, dataFile)) {
        std::ifstream t(e);
        std::stringstream buffer;
        buffer << t.rdbuf();

        data[e] = buffer.str();

        const std::string &p = data[e];
        const char *data = p.c_str();
        size_t start = 0;
        enum { stN, stNL, stNLP } state = stN;

        for (size_t c = 0, fc = 0; c < p.size(); c++) {
          switch (data[c]) {
            case '\n':
              switch (state) {
                case stN:
                  state = stNL;
                  break;
                case stNLP:
                  cookies.push_back(cookie(
                      doROT13, std::string(p.data() + start, c - start - 1), e,
                      fc++));
                  start = c + 1;
                default:
                  state = stN;
                  break;
              }
              break;

            case '%':
              switch (state) {
                case stNL:
                  state = stNLP;
                  break;
                default:
                  state = stN;
                  break;
              }
              break;

            case '\r':
              break;

            default:
              state = stN;
              break;
          }
        }
      }
    }
    closedir(d);

    return true;
  }

  const std::size_t size(void) const { return cookies.size(); }

  std::size_t random(void) const { return std::rand() % size(); }

  const cookie &get(std::size_t i) const {
    if (i < cookies.size()) {
      return cookies[i];
    }

    return get(random());
  }

 protected:
  std::vector<cookie> cookies;
  std::map<std::string, std::string> data;
};
}

#endif
