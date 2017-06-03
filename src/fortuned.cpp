/* FaaS: Fortune as a Service
 *
 * Call it like this:
 *
 *     $ ./fortuned http:localhost:8080
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

#define ASIO_DISABLE_THREADS
#include <cxxhttp/httpd-options.h>
#include <cxxhttp/httpd-trace.h>

#include <fortuned/cli.h>
#include <fortuned/httpd-fortune.h>

/* FaaS main function
 * @argc Number of command line arguments.
 * @argv The actual command line arguments passed to the programme.
 *
 * This function fills the cache for the fortune programme using the default
 * locations for this on a Debian system, then starts serving fortune cookies
 * from that.
 *
 * @return zero on success, nonzero otherwise.
 */
int main(int argc, char* argv[]) {
  auto& fortunes = efgy::global<fortuned::fortune>();
  fortunes.load("/usr/share/games/fortunes");
  fortunes.load("/usr/share/games/fortunes/off", true);

  fortuned::cookiesTotal.set(fortunes.cookies.size());

  srand(time(0));

  return cxxhttp::main(argc, argv);
}
