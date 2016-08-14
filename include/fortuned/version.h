#if !defined(FORTUNED_VERSION_H)
#define FORTUNED_VERSION_H

/**\file
 * \brief Version information
 *
 * This header contains the library's version number and some additional
 * documentation that didn't seem to fit in anywhere else.
 *
 * \copyright
 * This file is part of the fortuned project, which is released as open source
 * under the terms of an MIT/X11-style licence, described in the COPYING file.
 *
 * \see Project Documentation: https://ef.gy/documentation/fortuned
 * \see Project Source Code: https://github.com/ef-gy/fortuned
 * \see Licence Terms: https://github.com/ef-gy/fortuned/blob/master/COPYING
 */

/**\dir include/fortuned
 * \brief Library headers
 *
 * The library headers are all located at include/fortuned in the repository.
 * Since this is a template libary these headers also contain the actual
 * implementation of the library, much like Pascal code tends to include both
 * the interface and the implementation in a single file.
 *
 * To "install" the library, all you really need to do is to copy this
 * directory to your own "include" directory - either globally or in your
 * project. Better yet, symlink this location to a checkout of the repository,
 * then you can update that with git every now and then and enjoy the latest
 * documentation and features of the library.
 */

/**\brief Base namespace
 *
 * This is the base namespace for all the headers contained within fortuned.
 * This namespace is used to prevent namespace clashes with your own code - if
 * you don't want to use fully qualified names in your code, use the 'using'
 * directive after including the headers, like so:
 *
 * \code
 * using namespace fortuned;
 * \endcode
 */
namespace fortuned {
/**\brief Library version
 *
 * This is the version of the header files you're including. You could test
 * this if you expect trouble with certain versions, or you know that your
 * code requires a very specific version of these headers.
 */
static const unsigned int version = 1;
}

#endif
