/* Fortune programme CLI parameter handling.
 *
 * Allows querying the fortune cookie database on the command line. This would
 * make it a simple replacement for the `fortune` programme, if so desired.
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

#if !defined(FORTUNED_CLI_H)
#define FORTUNED_CLI_H

#include <ef.gy/global.h>

#include <fortuned/fortune.h>

namespace fortuned {
/* CLI option to count cookies.
 *
 * Prints the number of cookies in the loaded database to stdout. Does not print
 * any actual fortune cookies, so this is fairly safe.
 */
static efgy::cli::option count(
    "-{0,2}count",
    [](std::smatch &m) -> bool {
      auto &fortunes = efgy::global<fortuned::fortune>();
      std::cout << fortunes.cookies.size() << " cookie(s) loaded\n";

      return true;
    },
    "Prints the number of fortune cookies in the database.");

/* CLI option to print cookies.
 *
 * Allows printing either a specific or a random fortune cookie. The cookie will
 * be sent to stdout, without further processing.
 *
 * While somewhat unsafe, this is actually how the regular `fortune` programme
 * works.
 */
static efgy::cli::option print(
    "-{0,2}print(:([0-9]+))?",
    [](std::smatch &m) -> bool {
      auto &fortunes = efgy::global<fortuned::fortune>();
      std::string sid = m[2];
      std::size_t id = sid.empty() ? fortunes.random() : std::stoll(sid);
      if (id >= fortunes.cookies.size()) {
        std::cerr << "Sorry, I don't have the cookie you want :(.\n";
      } else {
        std::cout << fortunes.cookies[id].data;
      }

      return true;
    },
    "Print a fortune cookie to stdout - a numerical parameter "
    "selects a specific cookie.");
}

#endif
