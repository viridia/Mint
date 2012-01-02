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
    , _includeOptions(true)
    , _includeTargets(true)
  {}

  /// Whether to include options in the output
  bool includeOptions() const { return _includeOptions; }
  void setIncludeOptions(bool value) { _includeOptions = value; }

  /// Whether to include targets in the output
  bool includeTargets() const { return _includeTargets; }
  void setIncludeTargets(bool value) { _includeTargets = value; }

  /// Write out a node to the stream. If 'isDefinition' is true, it means that
  /// any objects should be written out literally; If false, it means that
  /// only a reference to the object's name (if it has one) should be written.
  GraphWriter & write(Node * node, bool isDefinition);
  GraphWriter & write(Module * module);
  GraphWriter & write(ArrayRef<Node *> nodes, bool isDefinition);
  GraphWriter & writeCachedVars(Module * module);

  /// Adjust the indentation level.
  GraphWriter & indent() { ++_indentLevel; return *this; }
  GraphWriter & unindent() { --_indentLevel; return *this; }

  /// Return the stream that this writes to.
  OStream & strm() { return _strm; }

protected:
  void writeCachedVars(Node * scope, String * name, Node * value);
  bool writeValue(Node * node, bool isDefinition = false);
  void writeList(Oper * list);
  void writeDict(Object * dict);
  bool writeObject(Object * obj, bool isDefinition);
  void writeObjectContents(Object * obj);
  void writeAssignedValue(Node * n);
  void writeRelativePath(Node * scope);

  virtual bool filter(Node * n);

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
  bool _includeOptions;
  bool _includeTargets;
};

}

#endif // MINT_GRAPH_GRAPHWRITER_H
