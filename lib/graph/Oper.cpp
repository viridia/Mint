/* ================================================================== *
 * Oper
 * ================================================================== */

#include "mint/graph/Oper.h"

#include "mint/support/Assert.h"
#include "mint/support/OStream.h"

namespace mint {

Oper::Oper(NodeKind nk, Location loc, Type * type, NodeArray args)
  : Node(nk, loc, type)
  , _size(args.size())
{
  Node ** dst = _data;
  for (NodeArray::const_iterator it = args.begin(), itEnd = args.end(); it != itEnd; ++it) {
    Node * n = *it;
    RefCountable::acquire(n);
    *dst++ = n;
  }
}

Oper::~Oper() {
  for (NodeArray::const_iterator it = args().begin(), itEnd = args().end(); it != itEnd; ++it) {
    const Node * n = *it;
    RefCountable::release(n);
  }
}

/// Static creator function for Oper.
Oper * Oper::create(NodeKind nk, Location location, Type * type, NodeArray args) {
  size_t size = sizeof(Oper) + sizeof(Node *) * (args.size() - 1);
  char * mem = new char[size];
  return new (mem) Oper(nk, location, type, args);
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
  char * mem = new char[size];
  return new (mem) Oper(nk, location, type, args);
}

Node * Oper::arg(unsigned index) const {
  M_ASSERT(index < _size);
  return _data[index];
}

Oper * Oper::setArg(unsigned index, Node * value) {
  M_ASSERT(index < _size);
  Node * prev = _data[index];
  _data[index] = value;
  RefCountable::acquire(value);
  RefCountable::release(prev);
  return this;
}

void Oper::print(OStream & strm) const {
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
