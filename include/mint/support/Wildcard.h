/* ================================================================ *
   Wildcard pattern matcher
 * ================================================================ */

#ifndef MINT_SUPPORT_WILDCARD_H
#define MINT_SUPPORT_WILDCARD_H

#ifndef MINT_COLLECTIONS_STRINGREF_H
#include "mint/collections/StringRef.h"
#endif

namespace mint {

/** -------------------------------------------------------------------------
    Class to perform wildcard matching on strings.
 */
class WildcardMatcher {
public:
  /// Constructor
  WildcardMatcher(StringRef pattern);

  /// Returns true if 'str' matches the wildcard pattern.
  bool match(StringRef str);

  /// Returns true if 'pattern' contains wildcard characters.
  static bool hasWildcardChars(StringRef pattern);

private:
  bool matchImpl(StringRef str, unsigned patternIndex, unsigned strIndex);
  unsigned countNonStarChars(unsigned startIndex);

  StringRef _pattern;
};

}

#endif // MINT_SUPPORT_WILDCARD_H
