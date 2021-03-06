/* ================================================================== *
 * Oper
 * ================================================================== */

#include "mint/graph/Literal.h"
#include "mint/graph/Oper.h"

#include "mint/support/Assert.h"
#include "mint/support/Diagnostics.h"
#include "mint/support/OStream.h"

namespace mint {

Oper::Oper(NodeKind nk, Location loc, Type * type, NodeArray args)
  : Node(nk, loc, type)
  , _size(args.size())
{
  Node ** dst = _data;
  for (NodeArray::const_iterator it = args.begin(), itEnd = args.end(); it != itEnd; ++it) {
    Node * n = *it;
    *dst++ = n;
  }
}

/// Static creator function for Oper.
Oper * Oper::create(NodeKind nk, Location location, Type * type, NodeArray args) {
  size_t size = sizeof(Oper) + sizeof(Node *) * (args.size() - 1);
  return new (size) Oper(nk, location, type, args);
}

/// Static creator function for Oper.
Oper * Oper::create(NodeKind nk, Type * type, NodeArray args) {
  Location location;
  for (NodeArray::const_iterator it = args.begin(), itEnd = args.end(); it != itEnd; ++it) {
    if (*it) {
      location |= (*it)->location();
    }
  }
  size_t size = sizeof(Oper) + sizeof(Node *) * (args.size() - 1);
  return new (size) Oper(nk, location, type, args);
}

Oper * Oper::createEmptyList(Type * type) {
  return create(Node::NK_LIST, Location(), type, ArrayRef<Node *>());
}

Oper * Oper::createList(Location location, Type * type, NodeArray args) {
  return create(Node::NK_LIST, location, type, args);
}

Oper * Oper::createList(Location location, Type * type, Node * arg0) {
  return create(Node::NK_LIST, location, type, makeArrayRef(arg0));
}

Oper * Oper::createList(Location location, Type * type, Node * arg0, Node * arg1) {
  Node * args[] = { arg0, arg1 };
  return create(Node::NK_LIST, location, type, args);
}

Node * Oper::arg(unsigned index) const {
  M_ASSERT(index < _size);
  return _data[index];
}

Oper * Oper::setArg(unsigned index, Node * value) {
  M_ASSERT(index < _size);
  _data[index] = value;
  return this;
}

Node * Oper::getElement(Node * index) const {
  if (index->nodeKind() == Node::NK_INTEGER) {
    int i = static_cast<IntegerLiteral *>(index)->value();
    if (unsigned(i) >= _size) {
      diag::error(index->location()) << "Index out of range: " << i;
      return &Node::UNDEFINED_NODE;
    }
    return arg(i);
  }
  diag::error(index->location()) << "Invalid key type: " << (Node *)index->type();
  return &Node::UNDEFINED_NODE;
}

void Oper::print(OStream & strm) const {
  if (nodeKind() == NK_GET_MEMBER) {
    strm << arg(0) << "." << arg(1);
  } else if (nodeKind() == NK_GET_ELEMENT) {
      strm << arg(0) << "[" << arg(1) << "]";
  } else if (nodeKind() == NK_ACTION_COMMAND) {
    strm << "fundamentals.command(" << arg(0) << ", " << arg(1) << ")";
  } else if (nodeKind() == NK_ACTION_MESSAGE) {
    strm << "fundamentals.message."
        << diag::severityMethodName(diag::Severity(arg(0)->requireInt()))
        << "(" << arg(1) << ")";
  } else if (nodeKind() == NK_LIST) {
    strm << "[";
    for (const_iterator it = this->begin(); it != this->end(); ++it) {
      if (it != begin()) {
        strm << ", ";
      }
      if (*it == NULL) {
        strm << "<NULL>";
      } else {
        (*it)->print(strm);
      }
    }
    strm << "]";
  } else {
    strm << kindName(nodeKind()) << "(";
    for (const_iterator it = this->begin(); it != this->end(); ++it) {
      if (it != begin()) {
        strm << ", ";
      }
      if (*it == NULL) {
        strm << "<NULL>";
      } else {
        (*it)->print(strm);
      }
    }
    strm << ")";
  }
}

void Oper::dump() const {
  this->print(console::err());
  console::err() << "\n";
}

void Oper::trace() const {
  Node::trace();
  safeMarkArray(args());
}

}
