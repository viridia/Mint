/* ================================================================== *
 * Mint
 * ================================================================== */

#ifndef MINT_INTRINSIC_STRINGREGISTRY_H
#define MINT_INTRINSIC_STRINGREGISTRY_H

#ifndef MINT_GRAPH_STRINGDICT_H
#include "mint/graph/StringDict.h"
#endif

namespace mint {

/** -------------------------------------------------------------------------
    Registry of common strings in Mint.
 */
class StringRegistry : public GC {
public:

  /// Intern string function (static).
  static String * str(StringRef in) {
    return get().makestr(in);
  }

  /// Return the StringRegistry singleton.
  static StringRegistry & get();

  // Overrides

  void trace() const;

private:
  /// Intern string function
  String * makestr(StringRef in);

  StringDict<Node> _strings;
};

/** -------------------------------------------------------------------------
    Classes that want to can use this namespace and just call 'str()'.
 */
namespace strings {
  inline String * str(StringRef in) {
    return StringRegistry::str(in);
  }
}

}

#endif // MINT_INTRINSIC_STRINGREGISTRY_H
