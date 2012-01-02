/* ================================================================== *
 * OSError
 * ================================================================== */

#include "mint/support/Assert.h"
#include "mint/support/OSError.h"

#if HAVE_ERRNO_H
#include <errno.h>
#endif

#if HAVE_STDIO_H
#include <stdio.h>
#endif

namespace mint {

#if HAVE_ERRNO_H
void printPosixFileError(StringRef verb, StringRef path, int error) {
  using namespace mint::console;
  switch (error) {
    case ENOENT:
      err() << "Error " << verb << " '" << path << "': entry not found.\n";
      break;

    case EACCES:
      err() << "Error " << verb << " '" << path << "': permission denied.\n";
      break;

    case EFAULT:
      M_ASSERT(false) << "operation returned EFAULT";
      break;

    case EIO:
      err() << "Error " << verb << " '" << path << "': I/O error.\n";
      break;

    case ELOOP:
      err() << "Error " << verb << " '" << path << "': too many symbolic links.\n";
      break;

    case ENAMETOOLONG:
      err() << "Error " << verb << " '" << path << "': name too long.\n";
      break;

    case ENOTDIR:
      err() << "Error " << verb << " '" << path
          << "': a component in the path was not a directory.\n";
      break;

    case EOVERFLOW:
      err() << "Error " << verb << " '" << path << "': internal error (overflow).\n";
      break;

    default:
      err() << "Error " << verb << " '" << path << "': unknown error (" << error << ").\n";
      ::perror(NULL);
      break;
  }
}
#endif

#if _WIN32
void printWin32FileError(StringRef verb, StringRef path, DWORD error) {
  using namespace mint::console;
  switch (error) {
    case ERROR_FILE_NOT_FOUND:
      err() << "Error " << verb << " '" << path << "': entry not found.\n";
      break;

    //case EACCES:
    //  err() << "Error " << verb << " '" << path << "': permission denied.\n";
    //  break;

    //case EFAULT:
    //  M_ASSERT(false) << "operation returned EFAULT";
    //  break;

    //case EIO:
    //  err() << "Error " << verb << " '" << path << "': I/O error.\n";
    //  break;

    //case ELOOP:
    //  err() << "Error " << verb << " '" << path << "': too many symbolic links.\n";
    //  break;

    //case ENAMETOOLONG:
    //  err() << "Error " << verb << " '" << path << "': name too long.\n";
    //  break;

    //case ENOTDIR:
    //  err() << "Error " << verb << " '" << path
    //      << "': a component in the path was not a directory.\n";
    //  break;

    //case EOVERFLOW:
    //  err() << "Error " << verb << " '" << path << "': internal error (overflow).\n";
    //  break;

    default:
      err() << "Error " << verb << " '" << path << "': unknown error (" << error << ").\n";
      ::perror(NULL);
      break;
  }
}
#endif

}
