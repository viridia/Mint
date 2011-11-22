/* ================================================================== *
 * Base Assertions: This file allows types such as StringRef, which
 * are used by assertions, to themselves have assertions.
 * ================================================================== */

#ifndef MINT_SUPPORT_ASSERTBASE_H
#define MINT_SUPPORT_ASSERTBASE_H

#ifndef MINT_CONFIG_H
#include "mint/config.h"
#endif

#ifndef HAVE_STDBOOL_H
#include <stdbool.h>
#endif

namespace mint {

/** -------------------------------------------------------------------------
    Prints an error message and aborts.
 */

void assertionFailure(const char * expression, const char * fname, int lineno);

#define M_ASSERT_BASE(expression) \
    (expression) ? (void)0 : assertionFailure(#expression, __FILE__, __LINE__)

}

#endif // MINT_SUPPORT_ASSERTBASE_H
