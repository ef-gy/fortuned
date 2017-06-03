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

#if !defined(FORTUNED_HTTPD_FORTUNE_H)
#define FORTUNED_HTTPD_FORTUNE_H

#include <ef.gy/global.h>
#include <ef.gy/json.h>

#include <prometheus/httpd-metrics.h>

#include <fortuned/fortune.h>

namespace fortuned {
/* fortuned_cookies_served_total Prometheus metric.
 *
 * This metric counts the number of times a cookie was served through the HTTP
 * API. Each (valid) access below /fortune is increasing this counter.
 */
static prometheus::metric::counter cookiesServed(
    "fortuned_cookies_served_total",
    "How many fortune cookies have been sent back to clients.");

/* fortuned_cookies_total Prometheus metric.
 *
 * This metric is a simple count of all the cookies that the server knows about.
 * It must be set manually, and this is done in the `main` function.
 */
static prometheus::metric::gauge cookiesTotal(
    "fortuned_cookies_total",
    "How many unique fortune cookies we have loaded.");

/* `fortune` API entry point.
 * @session The HTTP session to reply on.
 * @matches Resource path regex matches. Used to extract the cookie ID, if set.
 *
 * This replies by either selecting a random cookie, or selecting the one with
 * the given ID and sending it back to the client that requested it.
 *
 * Supported output modes are plain text, XML and JSON. Selecting between these
 * modes is done through the `Accept:` header, see the servlet definition below
 * for the details.
 */
static void reply(cxxhttp::http::sessionData &session, std::smatch &matches) {
  using efgy::json::json;
  using efgy::json::to_string;
  using cxxhttp::http::error;

  auto &fortunes = efgy::global<fortuned::fortune>();
  std::string sid = matches[2];

  if (fortunes.cookies.size() == 0) {
    error(session).reply(503);
    return;
  }

  std::size_t id = sid.empty() ? fortunes.random() : std::stoll(sid);
  if (id >= fortunes.cookies.size()) {
    error(session).reply(404);
    return;
  }

  const auto &c = fortunes.cookies[id];
  const std::string &source = c.data;
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

    session.reply(200,
                  "<?xml version='1.0' encoding='utf-8'?>"
                  "<fortune xmlns='http://ef.gy/2012/fortune' sourceFile='" +
                      c.file + "' fileID='" + std::to_string(c.id) + "' id='" +
                      std::to_string(id) + "'>" + sc + "</fortune>");
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

/* `fortune` API servlet definition.
 *
 * Note the accepted MIME types, and see the <reply> function's documentation
 * for more details.
 */
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
