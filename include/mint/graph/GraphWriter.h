/* ================================================================== *
 * Mint: A refreshing approach to build configuration.
 * ================================================================== */

#ifndef MINT_GRAPH_GRAPHWRITER_H
#define MINT_GRAPH_GRAPHWRITER_H

#ifndef MINT_GRAPH_FUNCTION_H
#include "mint/graph/Function.h"
#endif

namespace mint {

class OStream;
class Node;
class Module;
class Object;
class Oper;
class Project;

/** -------------------------------------------------------------------------
    Class to serialize a graph to an output stream.
 */
class GraphWriter {
public:
  /// Constructor.
  GraphWriter(OStream & strm)
    : _strm(strm)
    , _indentLevel(0)
    , _activeScope(NULL)
    , _activeModule(NULL)
  {}

  /// Write out a node to the stream. If 'isDefinition' is true, it means that
  /// any objects should be written out literally; If false, it means that
  /// only a reference to the object's name (if it has one) should be written.
  GraphWriter & write(Node * node, bool isDefinition);
  GraphWriter & write(Module * module);
  GraphWriter & write(ArrayRef<Node *> nodes, bool isDefinition);

  /// Adjust the indentation level.
  GraphWriter & indent() { ++_indentLevel; return *this; }
  GraphWriter & unindent() { --_indentLevel; return *this; }

  /// Return the stream that this writes to.
  OStream & strm() { return _strm; }

private:
  void writeList(Oper * list);
  void writeDict(Object * dict);
  void writeObject(Object * obj, bool isDefinition);
  void writeObjectContents(Object * obj);
  void writeOptionContents(Object * obj);
  void writeRelativePath(Node * scope);

  Node * setActiveScope(Node * scope) {
    Node * prevScope = _activeScope;
    _activeScope = scope;
    return prevScope;
  }

  /// Return true if we can access 'obj' by name from the current active scope.
  bool hasRelativePath(Object * obj);

  OStream & _strm;
  unsigned _indentLevel;
  Node * _activeScope;
  Node * _activeModule;
};

}

#endif // MINT_GRAPH_GRAPHWRITER_H
