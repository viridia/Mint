/* ================================================================== *
 * OSError
 * ================================================================== */

#include "mint/support/Assert.h"
#include "mint/support/OSError.h"

#if HAVE_ERRNO_H
#include <errno.h>
#endif

namespace mint {

#if HAVE_ERRNO_H
void printPosixFileError(StringRef path, int error) {
  using namespace mint::console;
  switch (error) {
    case ENOENT:
      err() << "Error accessing '" << path << "': entry not found.\n";
      break;

    case EACCES:
      err() << "Error accessing '" << path << "': permission denied.\n";
      break;

    case EFAULT:
      M_ASSERT(false) << "operation returned EFAULT";
      break;

    case EIO:
      err() << "Error accessing '" << path << "': I/O error.\n";
      break;

    case ELOOP:
      err() << "Error accessing '" << path << "': too many symbolic links.\n";
      break;

    case ENAMETOOLONG:
      err() << "Error accessing '" << path << "': name too long.\n";
      break;

    case ENOTDIR:
      err() << "Error accessing '" << path << "': a component in the path was not a directory.\n";
      break;

    case EOVERFLOW:
      err() << "Error accessing '" << path << "': internal error (overflow).\n";
      break;

    default:
      err() << "Error accessing '" << path << "': unknown error (" << error << ").\n";
      break;
  }
}
#endif

}
