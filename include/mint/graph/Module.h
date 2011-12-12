/* ================================================================== *
 * Module
 * ================================================================== */

#ifndef MINT_GRAPH_MODULE_H
#define MINT_GRAPH_MODULE_H

#ifndef MINT_GRAPH_OBJECT_H
#include "mint/graph/Object.h"
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
class Target;

/** -------------------------------------------------------------------------
    An instance of a configuration file.
 */
class Module : public Object {
public:
  typedef SmallVector<Object *, 4> ImportList;
  typedef SmallVector<Node *, 4> ActionList;

  /// Default constructor
  Module(StringRef moduleName, Project * project);

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

  /// Set an attribute on this module.
  void setAttribute(String * name, Node * value);

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
  void addImportScope(Object * scope) {
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
  SmallString<64> _sourceDir;
  SmallString<64> _buildDir;
  ImportList _importScopes;
  ActionList _actions;
  SmallVector<String *, 32> _keyOrder;
  TextBuffer * _textBuffer;
  Project * _project;
};

}

#endif // MINT_GRAPH_MODULE_H
