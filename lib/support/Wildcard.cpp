/* ================================================================ *
   Path manipulation functions
 * ================================================================ */

#include "mint/support/Wildcard.h"
#include "mint/support/OStream.h"

namespace mint {

WildcardMatcher::WildcardMatcher(StringRef pattern) : _pattern(pattern) {}

bool WildcardMatcher::match(StringRef str) {
  return matchImpl(str, 0, 0);
}

unsigned WildcardMatcher::countNonStarChars(unsigned startIndex) {
  unsigned count = 0;
  for (unsigned j = startIndex; j < _pattern.size(); ++j) {
    if (_pattern[j] != '*') {
      ++count;
    }
  }
  return count;
}

bool WildcardMatcher::matchImpl(StringRef str, unsigned patternIndex, unsigned strIndex) {
  while (patternIndex < _pattern.size()) {
    char ch = _pattern[patternIndex++];
    if (ch == '*') {
      if (patternIndex >= _pattern.size()) {
        // Wildcard at end matches rest of input string.
        return true;
      }

      // Count the number of non-wildcard characters remaining in the pattern.
      // this is how long the rest of the input string needs to be.
      unsigned suffixLength = countNonStarChars(patternIndex);

      // Simulate a greedy search: Start at the last possible position where
      // a match could happen and call this function recursively on the trailing
      // part of the pattern (after the star) and the trailing part of the input
      // string (at the start of the suffix.) If that fails, then back up 1 character
      // and try again. Repeat until we reach the position where this wildcard
      // was found - the point where the wildcard matches 0 characters.
      // Yes, this is N^2 - or worse if there are multiple stars in the pattern -
      // but the setup time is small, and most input strings will fail early.
      for (int i = str.size() - suffixLength; i >= int(strIndex); --i) {
        if (matchImpl(str, patternIndex, i)) {
          return true;
        }
      }
      return false;
    }

    // Either the input string is too short or the character doesn't match.
    if (strIndex >= str.size() || (ch != '?' && ch != str[strIndex])) {
      return false;
    }
    ++strIndex;
  }

  // We must have gotten to the end of the input string.
  return strIndex == str.size();
}

bool WildcardMatcher::hasWildcardChars(StringRef str) {
  for (StringRef::const_iterator it = str.begin(), itEnd = str.end(); it != itEnd; ++it) {
    if (*it == '*' || *it == '?') {
      return true;
    }
  }
  return false;
}

}
