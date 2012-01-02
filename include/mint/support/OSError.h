/* ================================================================== *
 * Functions for printing operating-system specific errors.
 * ================================================================== */

#ifndef MINT_SUPPORT_OSERROR_H
#define MINT_SUPPORT_OSERROR_H

#ifndef MINT_CONFIG_H
#include "mint/config.h"
#endif

#ifndef MINT_COLLECTIONS_STRINGREF_H
#include "mint/collections/StringRef.h"
#endif

#if _WIN32
#include <Windows.h>
#endif

namespace mint {

#if HAVE_ERRNO_H
void printPosixFileError(StringRef verb, StringRef path, int error);
#endif

#if _WIN32
void printWin32FileError(StringRef verb, StringRef path, DWORD error);
#endif

}

#endif // MINT_SUPPORT_OSERROR_H
