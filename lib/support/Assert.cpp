/* ================================================================== *
 * Diagnostic functions and macros
 * ================================================================== */

#include "mint/support/Assert.h"

#if HAVE_STDIO_H
#include <stdio.h>
#endif

namespace mint {

void assertionFailure(const char * expression, const char * fname, int lineno) {
  console::err() << fname << ":" << lineno << ": Assertion failed: " << expression << "\n";
  abort();
}

AssertionFailureStream::~AssertionFailureStream() {
  if (str().empty()) {
    (*this) << _expression;
  }
  console::err() << _fname << ":" << _lineno << ": Assertion failed: " << str() << "\n";
  abort();
}

}
