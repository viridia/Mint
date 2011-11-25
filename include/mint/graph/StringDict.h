/* ================================================================== *
 * Dictionary class with string keys.
 * ================================================================== */

#ifndef MINT_GRAPH_STRINGDICT_H
#define MINT_GRAPH_STRINGDICT_H

#ifndef MINT_GRAPH_STRING_H
#include "mint/graph/String.h"
#endif

#ifndef MINT_COLLECTIONS_TABLE_H
#include "mint/collections/Table.h"
#endif

namespace mint {

/** -------------------------------------------------------------------------
    TableKeyTraits for Strings.
 */
struct StringKeyTraits {
  static inline unsigned hash(const String * key) {
    return key->hash();
  }

  static inline unsigned equals(const String * sl, const String * sr) {
    return sl == sr || sl->value() == sr->value();
  }

  static inline unsigned hash(StringRef key) {
    return key.hash();
  }

  static inline unsigned equals(const String * sl, StringRef sr) {
    return sl->value() == sr;
  }
};

/** -------------------------------------------------------------------------
    A map of strings to some value type.
 */
template<typename T>
class StringDict : public Table<String, T, StringKeyTraits> {
public:
  StringDict(size_t initialSize = 0) : Table<String, T, StringKeyTraits>(initialSize) {}
  void trace() const {
    for (typename StringDict::const_iterator it = this->begin(), itEnd = this->end(); it != itEnd;
        ++it) {
      it->first->mark();
      it->second->mark();
    }
  }
};

}

#endif // MINT_GRAPH_STRINGDICT_H
