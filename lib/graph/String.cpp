/* ================================================================== *
 * String
 * ================================================================== */

#include "mint/graph/String.h"
#include "mint/graph/TypeRegistry.h"

#include "mint/support/Assert.h"
#include "mint/support/OStream.h"

#if HAVE_NEW
#include <new>
#endif

namespace mint {

String::String(NodeKind nt, Location location, Type * ty, StringRef value)
  : Node(nt, location, ty)
  , _size(value.size())
{
  memcpy(_data, value.data(), value.size());
}

unsigned String::hash() const {
  return value().hash();
}

String * String::create(NodeKind nt, Location location, Type * ty, StringRef value) {
  size_t size = sizeof(String) + value.size() - 1;
  char * mem = new char[size];
  return new (mem) String(nt, location, ty, value);
}

String * String::create(Location location, StringRef value) {
  return create(Node::NK_STRING, location, TypeRegistry::stringType(), value);
}

String * String::create(StringRef value) {
  return create(Node::NK_STRING, Location(), TypeRegistry::stringType(), value);
}

String * String::cast(Node * from) {
  M_ASSERT(from->nodeKind() == Node::NK_STRING || from->nodeKind() == Node::NK_IDENT);
  return static_cast<String *>(from);
}

String * String::dyn_cast(Node * from) {
  if (from != NULL && (from->nodeKind() == Node::NK_STRING || from->nodeKind() == Node::NK_IDENT)) {
    return static_cast<String *>(from);
  }
  return NULL;
}

void String::print(OStream & strm) const {
  if (nodeKind() == Node::NK_IDENT) {
    strm << value();
  } else {
    strm << '\'' << value() << '\'';
  }
}

}
