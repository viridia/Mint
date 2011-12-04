/* ================================================================== *
 * Mint
 * ================================================================== */

#include "mint/intrinsic/StringRegistry.h"
#include "mint/intrinsic/TypeRegistry.h"

namespace mint {

String * StringRegistry::makestr(StringRef in) {
  StringDict<Node>::const_iterator it = _strings.find_as(in);
  if (it != _strings.end()) {
    return it->first;
  }
  String * result = String::create(Node::NK_IDENT, Location(), TypeRegistry::stringType(), in);
  _strings[result] = result;
  return result;
}

StringRegistry & StringRegistry::get() {
  static GCPointerRoot<StringRegistry> instance(new StringRegistry());
  return *instance;
}

void StringRegistry::trace() const {
  _strings.trace();
}

}
