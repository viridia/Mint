/* ================================================================== *
 * Mint: A refreshing approach to build configuration.
 * ================================================================== */

#ifndef MINT_PROJECT_PROJECTWRITERXML_H
#define MINT_PROJECT_PROJECTWRITERXML_H

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
class BuildConfiguration;

/** -------------------------------------------------------------------------
    Class to serialize a graph to an output stream in XML format.
 */
class ProjectWriterXml {
public:
  /// Constructor.
  ProjectWriterXml(OStream & strm)
    : _strm(strm)
    , _indentLevel(0)
    , _activeScope(NULL)
    , _activeModule(NULL)
  {}

  ProjectWriterXml & writeBuildConfiguration(BuildConfiguration * bc);
  ProjectWriterXml & writeProject(Project * proj, bool isMain);

  /// Write out a node to the stream. If 'isDefinition' is true, it means that
  /// any objects should be written out literally; If false, it means that
  /// only a reference to the object's name (if it has one) should be written.
  ProjectWriterXml & write(Node * node, bool isDefinition);
  ProjectWriterXml & write(Module * module);
  ProjectWriterXml & write(ArrayRef<Node *> nodes, bool isDefinition);
  ProjectWriterXml & writeCachedVars(Module * module);
  ProjectWriterXml & writeTargets(Module * module);

  /// Adjust the indentation level.
  ProjectWriterXml & indent() { ++_indentLevel; return *this; }
  ProjectWriterXml & unindent() { --_indentLevel; return *this; }

  /// Return the stream that this writes to.
  OStream & strm() { return _strm; }

protected:
  bool hasCachedVars(Object * obj);
  void writeCachedVars(Node * scope, String * name, Node * value);
  bool writeValue(Node * node, bool isDefinition = false);
  void writeList(Oper * list);
  void writeDict(Object * dict);
  bool writeObject(Object * obj, bool isDefinition);
  void writeObjectContents(Object * obj);
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
};

}

#endif // MINT_PROJECT_PROJECTWRITERXML_H
