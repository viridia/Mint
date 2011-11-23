/* ================================================================== *
 * Utility class for building the graph
 * ================================================================== */

#ifndef MINT_GRAPH_GRAPHBUILDER_H
#define MINT_GRAPH_GRAPHBUILDER_H

#ifndef MINT_GRAPH_FUNCTION_H
#include "mint/graph/Function.h"
#endif

#ifndef MINT_GRAPH_TYPEREGISTRY_H
#include "mint/graph/TypeRegistry.h"
#endif

namespace mint {

/** -------------------------------------------------------------------------
    Utility class for building the graph.
 */
class GraphBuilder {
public:

  /// Constructor.
  GraphBuilder(TypeRegistry & typeRegistry)
    : _typeRegistry(typeRegistry)
  {}

  /// Create an empty list, given a list type.
  Node * createList(Location loc, Type * type);

  /// Create a list with one elements.
  Node * createList(Location loc, Type * type, Node * e0);

  /// Create a list with an array of elements.
  Node * createList(Location loc, Type * type, NodeArray elements);

  /// Create an empty list, given the element type.,
  Node * createListOf(Location loc, Type * elementType);

  /// Create a list with one elements.
  Node * createListOf(Location loc, Type * elementType, Node * e0);

  /// Create a list with an array of elements.
  Node * createListOf(Location loc, Type * elementType, NodeArray elements);

  /// Create a call operation with no arguments.
  Node * createCall(Location loc, Function * func);

  /// Create a call operation with 1 argument.
  Node * createCall(Location loc, Function * func, Node * arg0);

  /// Create a call operation with 2 argument.
  Node * createCall(Location loc, Function * func, Node * arg0, Node * arg1);

  /// Create a call operation with an array of arguments.
  Node * createCall(Location loc, Function * func, NodeArray args);

  /// Create a function with no arguments.
  Function * createFunction(Location loc, Type * returnType, MethodHandler * m);

  /// Create a function with 1 argument.
  Function * createFunction(Location loc, Type * returnType, Type * arg0Type, MethodHandler * m);

  /// Create a function with 2 arguments.
  Function * createFunction(Location loc, Type * returnType, Type * arg0Type, Type * arg1Type,
      MethodHandler * m);

  /// Create a function with an array of arguments.
  Function * createFunction(Location loc, Type * returnType, TypeArray argTypes, MethodHandler * m);
private:
  TypeRegistry & _typeRegistry;
};

}

#endif // MINT_GRAPH_GRAPHBUILDER_H
