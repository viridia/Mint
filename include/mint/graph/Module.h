/* ================================================================== *
 * Module
 * ================================================================== */

#ifndef MINT_GRAPH_MODULE_H
#define MINT_GRAPH_MODULE_H

#ifndef MINT_GRAPH_NODE_H
#include "mint/graph/Node.h"
#endif

#ifndef MINT_GRAPH_STRINGDICT_H
#include "mint/graph/StringDict.h"
#endif

#ifndef MINT_COLLECTIONS_SMALLSTRING_H
#include "mint/collections/SmallString.h"
#endif

#ifndef MINT_SUPPORT_TEXTBUFFER_H
#include "mint/support/TextBuffer.h"
#endif

#if HAVE_STDDEF_H
#include <stddef.h>
#endif

namespace mint {

class Project;

/** -------------------------------------------------------------------------
    An instance of a configuration file.
 */
class Module : public Node {
public:

  /// Default constructor
  Module(NodeKind kind, StringRef filePath, Project * project)
    : Node(kind)
    , _filePath(filePath)
    , _parentScope(NULL)
    , _project(project)
  {
  }

  /// Absolute path to source file for this module.
  StringRef filePath() const { return _filePath; }

  /// For nodes that are scopes, this returns the enclosing scope.
  Node * parentScope() const { return _parentScope; }
  void setParentScope(Node * parentScope) { _parentScope = parentScope; }

  /// Any node can potentially be a scope for defined symbols.
  Node * getPropertyValue(String * name) const;

  /// Set a property on this module.
  void setProperty(String * name, Node * value);

  /// Table of properties for this module.
  const StringDict<Node> & properties() const { return _properties; }
  StringDict<Node> & properties() { return _properties; }

  /// Text buffer for this module
  TextBuffer * textBuffer() const { return _textBuffer.ptr(); }
  void setTextBuffer(TextBuffer * textBuffer) { _textBuffer = textBuffer; }

  /// The project that this module belongs to (or NULL).
  Project * project() const { return _project; }

  /// Add a node to the list of scopes to search for symbols.
  void addImportScope(Module * scope) {
    _importScopes.push_back(scope);
  }

private:
  typedef SmallVector<Module *, 4> ImportList;

  SmallString<64> _filePath;
  ImportList _importScopes;
  StringDict<Node> _properties;
  Node * _parentScope; // This should not be a Ref<> to avoid cycles.
  Ref<TextBuffer> _textBuffer;
  Project * _project;
};

}

#endif // MINT_GRAPH_MODULE_H
