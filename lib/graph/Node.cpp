/* ================================================================== *
 * Node
 * ================================================================== */

#include "mint/graph/Literal.h"
#include "mint/graph/Node.h"
#include "mint/graph/String.h"
#include "mint/graph/TypeRegistry.h"

#include "mint/support/OStream.h"

namespace mint {

#ifdef NODE_KIND
#undef NODE_KIND
#endif

#define NODE_KIND(x) #x,
#define NODE_KIND_RANGE(x, start, end)

static const char * nodeKindNames[] = {
#include "mint/graph/NodeKind.def"
};

#undef NODE_KIND

Node Node::UNDEFINED_NODE(Node::NK_UNDEFINED, Location(), &TypeRegistry::UNDEFINED_TYPE);

const char * Node::kindName(NodeKind kind) {
  if (unsigned(kind) < unsigned(sizeof(nodeKindNames) / sizeof(nodeKindNames[0]))) {
    return nodeKindNames[unsigned(kind)];
  }
  return "<Invalid Node Kind>";
}

void Node::print(OStream & strm) const {
  switch (_nodeKind) {
    case NK_BOOL: {
      strm << (static_cast<const Literal<bool> *>(this)->value() ? "true" : "false");
      break;
    }

    case NK_INTEGER: {
      strm << static_cast<const Literal<long> *>(this)->value();
      break;
    }

    case NK_FLOAT: {
      strm << static_cast<const Literal<double> *>(this)->value();
      break;
    }

    case NK_UNDEFINED: {
      strm << "undefined";
      break;
    }

    default:
      strm << kindName(_nodeKind);
      break;
  }
}

void Node::dump() const {
  this->print(console::err());
  console::err() << "\n";
}

OStream & operator<<(OStream & strm, Node::NodeKind nk) {
  strm << Node::kindName(nk);
  return strm;
}

}
