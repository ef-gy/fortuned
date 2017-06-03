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

#include <cxxhttp/httpd.h>

#include <dirent.h>
#include <cstdlib>
#include <fstream>
#include <iostream>

namespace fortuned {
/* A fortune cookie.
 *
 * Contains all the data we keep track of around a fortune cookie - including
 * the cookie itself.
 */
struct cookie {
  /* Source file.
   *
   * Where the cookie came from on the local file system.
   */
  std::string file;

  /* Cookie number in the file.
   *
   * The number identifying which cookie in the source file this is.
   */
  std::size_t id;

  /* Cookie text.
   *
   * The actual text of the cookie. May have been decoded if it had been stored
   * with a ROT13 encoding.
   */
  std::string data;
};

/* Fortune programme data.
 *
 * Contains a member with all the cookies we know and care about, as well as
 * functions to load these cookies from the filesystem and decode them.
 */
class fortune {
 public:
  /* Our cookies database.
   *
   * Contains all the cookies that were read into and added to this object.
   * These can then be served to users of the programme, possibly via the HTTP
   * API in `httpd-fortune.h`.
   */
  std::vector<cookie> cookies;

  /* Load every file in a directory as cookies.
   * @dir The directory to load from.
   * @doROT13 Expect the input to be ROT13-encoed.
   *
   * Load all the fortune cookies from a directory, optionally applying the
   * ROT13 cipher, and then put all the cookies found this way into the
   * <cookies> member.
   *
   * Offensive cookies are ROT13-encoded, so as to prevent people from
   * accidentally reading those. For this programme, we don't rightly care about
   * all that, so we just load them with the rest of it. They are usually in a
   * separate directory, though, so the `doROT13` flag makes sense at a
   * directory scope.
   *
   * @return The number of cookies that were loaded.
   */
  std::size_t load(const std::string &dir, bool doROT13 = false) {
    static const std::regex dataFile(".*/[a-zA-Z-]+");
    std::size_t c = 0;
    DIR *d = opendir(dir.c_str());

    if (d) {
      struct dirent *en;

      while ((en = readdir(d)) != 0) {
        const std::string e = dir + "/" + en->d_name;

        if (regex_match(e, dataFile)) {
          std::ifstream t(e);
          std::stringstream buffer;
          buffer << t.rdbuf();

          c += load(doROT13, e, buffer.str());
        }
      }

      closedir(d);
    }

    return c;
  }

  /* Load all cookies in a file.
   * @doROT13 Whether to apply the ROT13 encoding while reading.
   * @file Where `data` came from.
   * @data The data that was read from `file`.
   *
   * This function will load all cookies in the given `data` string and append
   * them to the <cookies> member.
   *
   * @return The number of cookies loaded from the `data` string.
   */
  std::size_t load(bool doROT13, const std::string &file,
                   const std::string &data) {
    enum { stN, stNL, stNLP } state = stN;
    fortuned::cookie cookie{file, 0};

    for (const auto &ch : data) {
      if ((state == stN || state == stNL) && ch == '\n') {
        // if we encounter a newline not preceded by a newline and percent sign,
        // we just set the state accordingly.
        state = stNL;
      } else if (state == stNLP && ch == '\n') {
        // if we encounter a series of NL, '%', NL, then that marks the end of
        // a cookie, so append this cookie to our cache. After adding a newline
        // to the end of it, as we'd skipped this previously.
        cookie.data.push_back('\n');

        cookies.emplace_back(cookie);
        state = stN;

        // set up the temporary cookie for the next one in the file.
        cookie.id++;
        cookie.data.clear();
        continue;
      } else if (state == stNL && ch == '%') {
        // cookies are delimited by lines with only the percent-sign on them.
        state = stNLP;
      } else if (ch == '\r') {
        // don't change the state if we encounter a CR, in fact just ignore it.
      } else if (state == stNL) {
        // we previously read a newline, which we didn't put in the out, so put
        // it there now.
        state = stN;
        cookie.data.push_back('\n');
      } else if (state == stNLP) {
        // we just read a newline and a percent sign, but now we didn't get
        // another newline. So we put this back in the output.
        state = stN;
        cookie.data.push_back('\n');
        cookie.data.push_back('%');
      } else {
        // read a normal character, so reset to that state.
        state = stN;
      }

      if (state == stN) {
        if (doROT13 && ((ch >= 'a' && ch <= 'm') || (ch >= 'A' && ch <= 'M'))) {
          cookie.data.push_back(ch + 13);
        } else if (doROT13 &&
                   ((ch >= 'n' && ch <= 'z') || (ch >= 'N' && ch <= 'Z'))) {
          cookie.data.push_back(ch - 13);
        } else {
          cookie.data.push_back(ch);
        }
      }
    }
    return cookie.id;
  }

  /* Get the ID of a random cookie.
   *
   * Generates a random cookie ID. Since we use std::rand(), there's no
   * guarantees about the result not being biased.
   *
   * @return A number in [0..N), where N is cookies.size().
   */
  std::size_t random(void) const { return std::rand() % cookies.size(); }
};
}

#endif
