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
class Object;

typedef StringDict<Node> Attributes;

/** -------------------------------------------------------------------------
    An instance of a configuration file.
 */
class Module : public Node {
public:
  typedef SmallVector<Node *, 4> ImportList;
  typedef SmallVector<Node *, 4> ActionList;

  /// Default constructor
  Module(NodeKind kind, StringRef moduleName, Project * project);

  /// The relative path to this module from the source root.
  StringRef moduleName() const { return _moduleName; }

  /// Return the current source directory for this module (absolute).
  StringRef sourceDir() const { return _sourceDir; }
  void setSourceDir(StringRef dir) {
    _sourceDir = dir;
  }

  /// Return the current build directory for this module (absolute).
  StringRef buildDir() const { return _buildDir; }
  void setBuildDir(StringRef dir) {
    _buildDir = dir;
  }

  /// For nodes that are scopes, this returns the enclosing scope.
  Node * parentScope() const { return _parentScope; }
  void setParentScope(Node * parentScope) { _parentScope = parentScope; }

  /// Set an attribute on this module.
  void setProperty(String * name, Node * value);

  /// Table of properties for this module.
  const Attributes & attrs() const { return _properties; }
  Attributes & attrs() { return _properties; }

  /// List of scopes to search for imported symbols.
  const ImportList & importsScopes() const { return _importScopes; }

  /// List of configuration actions to perform.
  const ActionList & actions() const { return _actions; }

  /// Text buffer for this module
  TextBuffer * textBuffer() const { return _textBuffer; }
  void setTextBuffer(TextBuffer * textBuffer) { _textBuffer = textBuffer; }

  /// The project that this module belongs to (or NULL).
  Project * project() const { return _project; }

  /// The order in which keys were added.
  const SmallVectorImpl<String *> & keyOrder() const { return _keyOrder; }

  /// Add a node to the list of scopes to search for symbols.
  void addImportScope(Node * scope) {
    _importScopes.push_back(scope);
  }

  /// Add a node to the list of actions.
  void addAction(Node * action) {
    _actions.push_back(action);
  }

  // Overrides

  Node * getAttributeValue(StringRef name) const;
  bool getAttribute(StringRef name, AttributeLookup & result) const;
  void dump() const;
  void trace() const;

private:
  SmallString<64> _moduleName;
  SmallString<64> _sourceDir;
  SmallString<64> _buildDir;
  ImportList _importScopes;
  Attributes _properties;
  ActionList _actions;
  SmallVector<String *, 32> _keyOrder;
  Node * _parentScope;
  TextBuffer * _textBuffer;
  Project * _project;
};

}

#endif // MINT_GRAPH_MODULE_H
