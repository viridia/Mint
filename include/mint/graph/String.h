/* ================================================================== *
 * String node class.
 * ================================================================== */

#ifndef MINT_GRAPH_STRING_H
#define MINT_GRAPH_STRING_H

#ifndef MINT_GRAPH_NODE_H
#include "mint/graph/Node.h"
#endif

#ifndef MINT_COLLECTIONS_STRINGREF_H
#include "mint/collections/StringRef.h"
#endif

namespace mint {

/** -------------------------------------------------------------------------
    Holds either a quoted string or an identifier.
 */
class String : public Node {
public:
  typedef StringRef::const_iterator iterator;

  /// Construct a new string instance.
  static String * create(NodeKind nt, Location location, Type * ty, StringRef value);

  /// Construct a new string instance of type STRING.
  static String * create(Location location, StringRef value);

  /// Construct a new string instance of type STRING.
  static String * create(StringRef value);

  /// Construct a new string instance of type IDENT.
  static String * createIdent(StringRef value);

  /// Dynamically cast 'from' into a string, if possible. Report an error if not possible.
  static String * cast(Node * from);

  /// Dynamically cast 'from' into a string, if possible. Return NULL if not possible.
  static String * dyn_cast(Node * from);

  /// Downcsat to string type
  String * asString() { return this; }

  /// The value of this string
  StringRef value() const { return StringRef(_data, _size); }

  /// Allows implicit casting of a String to a StringRef
  operator StringRef() { return value(); }

  /// The length of this string
  unsigned size() const { return _size; }

  /// Iterators
  iterator begin() const { return &_data[0]; }
  iterator end() const { return &_data[_size]; }

  /// Compute a hash value for this string
  unsigned hash() const;

  /// Print a readable version of this node to the stream.
  void print(OStream & strm) const;

  // some static strings
  static String * emptyString();
  static String * strUndefined();
  static String * strTrue();
  static String * strFalse();

private:
  String(NodeKind nt, Location location, Type * ty, StringRef value);

  unsigned _size;
  char _data[1];
};

/** -------------------------------------------------------------------------
    Functor for comparing strings.
 */
struct StringComparator {
  inline bool operator()(const String * lhs, const String * rhs) {
    return lhs->value().compare(rhs->value()) < 0;
  }
};

}

#endif // MINT_GRAPH_STRING_H
