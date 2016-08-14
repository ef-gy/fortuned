/**\file
 * \ingroup example-programmes
 * \brief FaaS: Fortune as a Service
 *
 * Call it like this:
 * \code
 * $ ./fortuned http:localhost:8080
 * \endcode
 *
 * With localhost and 8080 being a host name and port of your choosing. Then,
 * while the programme is running.
 *
 * \copyright
 * This file is part of the fortuned project, which is released as open source
 * under the terms of an MIT/X11-style licence, described in the COPYING file.
 *
 * \see Project Documentation: https://ef.gy/documentation/fortuned
 * \see Project Source Code: https://github.com/ef-gy/fortuned
 * \see Licence Terms: https://github.com/ef-gy/fortuned/blob/master/COPYING
 */

#define ASIO_DISABLE_THREADS
#include <fortuned/fortuned.h>

using namespace efgy;
using fortuned::fortune;

namespace tcp {
using asio::ip::tcp;
static httpd::servlet<tcp> fortune(fortuned::regex, fortuned::reply<tcp>);
static httpd::servlet<tcp> quit("/quit", httpd::quit<tcp>);
}

namespace unix {
using asio::local::stream_protocol;
static httpd::servlet<stream_protocol>
    fortune(fortuned::regex, fortuned::reply<stream_protocol>);
static httpd::servlet<stream_protocol> quit("/quit",
                                            httpd::quit<stream_protocol>);
}

int main(int argc, char *argv[]) {
  fortune::common().prepare("/usr/share/games/fortunes/");
  fortune::common().prepare("/usr/share/games/fortunes/off/", true);

  srand(time(0));

  return io::main(argc, argv);
}
