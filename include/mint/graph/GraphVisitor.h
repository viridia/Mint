/* ================================================================== *
 * Mint
 * ================================================================== */

#ifndef MINT_GRAPH_GRAPHVISITOR_H
#define MINT_GRAPH_GRAPHVISITOR_H

#ifndef MINT_GRAPH_MODULE_H
#include "mint/graph/Module.h"
#endif

#ifndef MINT_GRAPH_OBJECT_H
#include "mint/graph/Object.h"
#endif

#ifndef MINT_GRAPH_OPER_H
#include "mint/graph/Oper.h"
#endif

namespace mint {

/** -------------------------------------------------------------------------
    Utility class for visiting the nodes of the graph.
 */
template <typename T>
class GraphVisitor {
public:

  /// Constructor.
  GraphVisitor() {}

  T visit(Node * node);

  virtual T visitModule(Module * m);
  virtual T visitObject(Object * obj);
  virtual T visitList(Oper * list);
  virtual T visitDict(Object * dict);
};

template <class T>
T GraphVisitor<T>::visit(Node * node) {
  switch (node->nodeKind()) {
    case Node::NK_MODULE:
      return visitModule(static_cast<Module *>(node));
    case Node::NK_OBJECT:
      return visitObject(static_cast<Object *>(node));
    case Node::NK_LIST:
      return visitList(static_cast<Oper *>(node));
    case Node::NK_DICT:
      return visitDict(static_cast<Object *>(node));
    default:
      return T();
  }
}

template <class T>
T GraphVisitor<T>::visitModule(Module * m) {
  /// The order in which keys were added.
  const SmallVectorImpl<String *> & keyOrder = m->keyOrder();
  const Attributes & attrs = m->attrs();
  for (SmallVectorImpl<String *>::const_iterator
      it = keyOrder.begin(), itEnd = keyOrder.end(); it != itEnd; ++it) {
    String * key = *it;
    Attributes::const_iterator a = attrs.find(key);
    visit(a->second);
  }
  return T();
}

template <class T>
T GraphVisitor<T>::visitObject(Object * obj) {
  const Attributes & attrs = obj->attrs();
  for (Attributes::const_iterator it = attrs.begin(), itEnd = attrs.end();
      it != itEnd; ++it) {
    visit(it->second);
  }
  return T();
}

template <class T>
T GraphVisitor<T>::visitList(Oper * list) {
  for (NodeArray::const_iterator it = list->begin(), itEnd = list->end(); it != itEnd; ++it) {
    visit(*it);
  }
  return T();
}

template <class T>
T GraphVisitor<T>::visitDict(Object * dict) {
  const Attributes & attrs = dict->attrs();
  for (Attributes::const_iterator it = attrs.begin(), itEnd = attrs.end();
      it != itEnd; ++it) {
    visit(it->second);
  }
  return T();
}

}

#endif // MINT_GRAPH_GRAPHVISITOR_H
