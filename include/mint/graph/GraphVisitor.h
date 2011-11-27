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
  virtual T visitOption(Object * obj);
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
    case Node::NK_OPTION:
      return visitOption(static_cast<Object *>(node));
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
  const PropertyTable & properties = m->properties();
  for (PropertyTable::const_iterator it = properties.begin(), itEnd = properties.end();
      it != itEnd; ++it) {
    visit(it->second);
  }
  return T();
}

template <class T>
T GraphVisitor<T>::visitObject(Object * obj) {
  const PropertyTable & properties = obj->properties();
  for (PropertyTable::const_iterator it = properties.begin(), itEnd = properties.end();
      it != itEnd; ++it) {
    visit(it->second);
  }
  return T();
}

template <class T>
T GraphVisitor<T>::visitOption(Object * obj) {
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
  const PropertyTable & properties = dict->properties();
  for (PropertyTable::const_iterator it = properties.begin(), itEnd = properties.end();
      it != itEnd; ++it) {
    visit(it->second);
  }
  return T();
}

}

#endif // MINT_GRAPH_GRAPHVISITOR_H
