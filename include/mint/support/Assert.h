/* ================================================================== *
 * Diagnostic functions and macros
 * ================================================================== */

#ifndef MINT_SUPPORT_ASSERT_H
#define MINT_SUPPORT_ASSERT_H

#ifndef MINT_SUPPORT_ASSERTBASE_H
#include "mint/support/AssertBase.h"
#endif

#ifndef MINT_SUPPORT_OSTREAM_H
#include "mint/support/OStream.h"
#endif

namespace mint {

/** -------------------------------------------------------------------------
    Assertion macro. Causes an assertion failure if 'expression' evaluates
    to false. The return value is a stream object which can be used to
    print a customized failure message:

       M_ASSERT(s != NULL) << "s cannot be NULL!";
 */
#define M_ASSERT(expression) \
    (expression) ? (void)0 : VoidResult() & AssertionFailureStream(#expression, __FILE__, __LINE__)

/** -------------------------------------------------------------------------
    Conditional diagnostics message - creates a stream which is only
    evaluated if 'condition' is true. Compiles to nothing if condition is a
    constant false.
 */

#define M_MSG(condition) (!condition) ? (void)0 : VoidResult() & diagnostics::DebugStream()

/** Used to transform a stream into a void result. */
class VoidResult {};

/** Transforms a value of stream type into a void result. */
inline void operator&(const VoidResult &, const OStream &) {}

/** Stream class which prints "Assertion failed" and aborts. */
class AssertionFailureStream : public OStrStream {
public:
  AssertionFailureStream(const char * expression, const char * fname, int lineno)
    : _expression(expression)
    , _fname(fname)
    , _lineno(lineno)
  {}

  AssertionFailureStream(const AssertionFailureStream & src)
    : _expression(src._expression)
    , _fname(src._fname)
    , _lineno(src._lineno)
  {}

  // The destructor is where all the real action happens. When the object is
  // destructed, the accumulated messages are written to the diagnostic output.
  ~AssertionFailureStream();

  void operator=(const AssertionFailureStream & src) {
    _expression = src._expression;
    _fname = src._fname;
    _lineno = src._lineno;
  }

private:
  const char * _expression;
  const char * _fname;
  int _lineno;
};

}

#endif // MINT_SUPPORT_DIAGNOSTICS_H
