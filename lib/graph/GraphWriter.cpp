/* ================================================================== *
 * Mint: A refreshing approach to build configuration.
 * ================================================================== */

#include "mint/graph/GraphWriter.h"
#include "mint/graph/Module.h"
#include "mint/graph/Object.h"
#include "mint/graph/Oper.h"
#include "mint/graph/Type.h"
#include "mint/graph/String.h"

#include "mint/support/Assert.h"

#if HAVE_ALGORITHM
#include <algorithm>
#endif

namespace mint {

struct StringDictComparator {
  inline bool operator()(
      const StringDict<Node>::value_type & lhs, const StringDict<Node>::value_type & rhs) {
    M_ASSERT(lhs.first != NULL);
    M_ASSERT(rhs.first != NULL);
    return lhs.first->value().compare(rhs.first->value()) > 0;
  }
};

GraphWriter & GraphWriter::write(Node * node) {
  switch (node->nodeKind()) {
    case Node::NK_UNDEFINED:
    case Node::NK_IDENT:
      break;

    case Node::NK_BOOL:
    case Node::NK_INTEGER:
    case Node::NK_FLOAT:
    case Node::NK_STRING:
    case Node::NK_TYPENAME:
      _strm << node;
      break;

    case Node::NK_LIST:
      writeList(static_cast<Oper *>(node));
      break;

    case Node::NK_DICT:
      writeDict(static_cast<Oper *>(node));
      break;

    case Node::NK_FUNCTION:
      break;

    case Node::NK_OBJECT:
    case Node::NK_OPTION:
      writeObject(static_cast<Object *>(node));
      break;

    default:
      console::err() << "Invalid node type for writing: " << node->nodeKind();
      break;
  }
  return *this;
}

GraphWriter & GraphWriter::write(Module * module) {
  _strm << "module " << module->filePath() << " {\n";
  ++_indentLevel;
  for (SmallVectorImpl<String *>::const_iterator
      it = module->keyOrder().begin(), itEnd = module->keyOrder().end(); it != itEnd; ++it) {
    _strm.indent(_indentLevel * 2);
    _strm << *it << " = ";
    write(module->properties()[*it]);
    _strm << "\n";
  }
  --_indentLevel;
  _strm << "}\n";
  return *this;
}

void GraphWriter::writeList(Oper * list) {
  _strm << "[\n";
  ++_indentLevel;
  for (Oper::const_iterator it = list->begin(), itEnd = list->end(); it != itEnd; ++it) {
    if (it != list->begin()) {
      _strm << ",\n";
    }
    _strm.indent(_indentLevel * 2);
    write(*it);
  }
  --_indentLevel;
  _strm << "\n";
  _strm.indent(_indentLevel * 2);
  _strm << "]";
}

void GraphWriter::writeDict(Oper * dict) {
  _strm << "{\n";
  ++_indentLevel;

  --_indentLevel;
  _strm.indent(_indentLevel * 2);
  _strm << "}";
}

void GraphWriter::writeObject(Object * obj) {
  if (obj->prototype() != NULL) {
    _strm << obj->prototype()->name() << " ";
  }
  _strm << "{\n";
  ++_indentLevel;
  SmallVector<StringDict<Node>::value_type, 64 > objectProperties;
  objectProperties.resize(obj->properties().size());
  std::copy(obj->properties().begin(), obj->properties().end(), objectProperties.begin());
  std::sort(objectProperties.begin(), objectProperties.end(), StringDictComparator());
  for (SmallVectorImpl<StringDict<Node>::value_type>::const_iterator
      it = objectProperties.begin(), itEnd = objectProperties.end(); it != itEnd; ++it) {
    _strm.indent(_indentLevel * 2);
    _strm << it->first << " = ";
    write(it->second);
    _strm << "\n";
  }
  --_indentLevel;
  _strm.indent(_indentLevel * 2);
  _strm << "}";
}

}
