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

namespace mint {

#if HAVE_ERRNO_H
void printPosixFileError(StringRef path, int error);
#endif

}

#endif // MINT_SUPPORT_OSERROR_H
