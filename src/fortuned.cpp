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
#include <fortuned/fortuned.h>

int main(int argc, char* argv[]) {
  auto& fortunes = efgy::global<fortuned::fortune>();
  fortunes.prepare("/usr/share/games/fortunes/");
  fortunes.prepare("/usr/share/games/fortunes/off/", true);

  fortuned::cookiesTotal.set(fortunes.size());

  srand(time(0));

  return cxxhttp::main(argc, argv);
}
