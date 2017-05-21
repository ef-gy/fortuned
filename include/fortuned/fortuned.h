/* `fortune`, as an HTTP API.
 *
 * This file implements a simple HTTP servlet that serves old-fashioned UNIX
 * fortune cookies.
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

#if !defined(FORTUNED_FORTUNED_H)
#define FORTUNED_FORTUNED_H

#include <cxxhttp/httpd.h>

#include <fortuned/fortune.h>

namespace fortuned {
static const std::string regex = "/fortune(/([0-9]+))?";

template <class transport>
static void reply(typename cxxhttp::http::server<transport>::session &session,
                  std::smatch &) {
  const auto &c = fortune::common().get();
  std::string sc = std::string(c);

  /* note: this escaping is not exactly efficient, but it's fairly simple
     and the std::strings are fairly short, so it shouldn't be much of an issue.
     */
  for (char i = 0; i < 0x20; i++) {
    if ((i == '\n') || (i == '\t')) {
      continue;
    }
    const char org[2] = {i, 0};
    const char rep[3] = {'^', (char)(('A' - 1) + i), 0};
    for (size_t pos = sc.find(org, pos); pos != std::string::npos;
         pos = sc.find(org, pos)) {
      sc.replace(pos, 2, rep);
      pos += 3;
    }
  }

  sc = "<![CDATA[" + sc + "]]>";

  session.reply(
      200,
      std::string("<?xml version='1.0' encoding='utf-8'?>"
                  "<fortune xmlns='http://ef.gy/2012/fortune' sourceFile='" +
                  c.file + "'>" + sc + "</fortune>"),
      {{"Content-Type", "text/xml; charset=utf-8"}});
}

static efgy::cli::option count(
    "-{0,2}count",
    [](std::smatch &m) -> bool {
      std::cout << fortune::common().size() << " cookie(s) loaded\n";

      return true;
    },
    "Prints the number of fortune cookies in the database.");

static efgy::cli::option print(
    "-{0,2}print(:([0-9]+))?",
    [](std::smatch &m) -> bool {
      if (m[2] != "") {
        std::cout << std::string(fortune::common().get(std::stoll(m[2])));
      } else {
        std::cout << std::string(fortune::common().get());
      }

      return true;
    },
    "Print a fortune to stdout - a numerical parameter "
    "selects a specific cookie.");

using cxxhttp::transport::tcp;
using cxxhttp::transport::unix;

static cxxhttp::httpd::servlet<tcp> TCP(fortuned::regex, fortuned::reply<tcp>);
static cxxhttp::httpd::servlet<unix> UNIX(fortuned::regex,
                                          fortuned::reply<unix>);
}

#endif
