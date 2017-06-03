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

#include <ef.gy/json.h>

#include <cxxhttp/httpd.h>

#include <prometheus/httpd-metrics.h>

#include <fortuned/fortune.h>

namespace fortuned {
prometheus::metric::counter cookiesServed(
    "fortuned_cookies_served_total",
    "How many fortune cookies have been sent back to clients.");
prometheus::metric::gauge cookiesTotal(
    "fortuned_cookies_total",
    "How many unique fortune cookies we have loaded.");

static void reply(cxxhttp::http::sessionData &session, std::smatch &matches) {
  using efgy::json::json;
  using efgy::json::to_string;
  using cxxhttp::http::error;

  auto &fortunes = efgy::global<fortuned::fortune>();
  std::string sid = matches[2];

  if (fortunes.size() == 0) {
    error(session).reply(503);
    return;
  }

  std::size_t id = sid.empty() ? fortunes.random() : std::stoll(sid);
  if (id > fortunes.size()) {
    error(session).reply(404);
    return;
  }

  const auto &c = fortunes.get(id);
  const std::string &source = c;
  std::string sc;

  for (const auto &c : source) {
    if (c >= 0 && c < 0x20 && c != '\n' && c != '\t') {
      sc.push_back('^');
      sc.push_back('A' - 1 + c);
    } else {
      sc.push_back(c);
    }
  }

  const auto &type = session.outbound.header["Content-Type"];

  cookiesServed.inc();

  if (type == "text/xml" || type == "application/xml") {
    sc = "<![CDATA[" + sc + "]]>";

    session.reply(
        200,
        std::string("<?xml version='1.0' encoding='utf-8'?>"
                    "<fortune xmlns='http://ef.gy/2012/fortune' sourceFile='" +
                    c.file + "'>" + sc + "</fortune>"));
  } else if (type == "text/json") {
    json r;
    r.toObject();

    r("id") = (long double)(id);
    r("file-id") = (long double)(c.id);
    r("file") = c.file;
    r("cookie") = sc;

    session.reply(200, to_string(r));
  } else if (type == "text/plain") {
    session.reply(200, sc);
  }
}

static efgy::cli::option count(
    "-{0,2}count",
    [](std::smatch &m) -> bool {
      auto &fortunes = efgy::global<fortuned::fortune>();
      std::cout << fortunes.size() << " cookie(s) loaded\n";

      return true;
    },
    "Prints the number of fortune cookies in the database.");

static efgy::cli::option print(
    "-{0,2}print(:([0-9]+))?",
    [](std::smatch &m) -> bool {
      auto &fortunes = efgy::global<fortuned::fortune>();
      std::string sid = m[2];
      std::size_t id = sid.empty() ? fortunes.random() : std::stoll(sid);
      const auto &c = fortunes.get(id);
      std::cout << std::string(c);

      return true;
    },
    "Print a fortune to stdout - a numerical parameter "
    "selects a specific cookie.");

static cxxhttp::http::servlet servlet(
    "/fortune(/([0-9]+))?", fortuned::reply, "GET",
    {{"Accept",
      "text/plain,"
      "text/json;q=0.9,"
      "text/xml;q=0.9,application/xml;q=0.9"}},
    "Serve a fortune cookie from memory. If the path includes the / and an ID, "
    "it will serve the cookie with this ID, otherwise a random cookie is "
    "served.");
}

#endif
