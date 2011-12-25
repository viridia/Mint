/* ================================================================== *
 * Fundamentals - the built-in root module.
 * ================================================================== */

#ifndef MINT_INTRINSIC_FUNDAMENTALS_H
#define MINT_INTRINSIC_FUNDAMENTALS_H

#ifndef MINT_GRAPH_MODULE_H
#include "mint/graph/Module.h"
#endif

namespace mint {

class Object;

/** -------------------------------------------------------------------------
    The built-in root module.
 */
class Fundamentals : public Module {
public:
  /// Constructor for the root module.
  Fundamentals();

  /// Intern string function
  String * str(StringRef in);

  /// Set an attribute on this module.
  void setAttribute(String * name, Node * value) { Module::setAttribute(name, value); }
  void setAttribute(StringRef name, Node * value) { Module::setAttribute(str(name), value); }

  /// Create a named dictionary in the fundamentals namespace.
  Object * createChildScope(StringRef name);

  /// Return the global Fundamentals object.
  static Fundamentals & get();

  // Overrides

  void trace() const;

private:
  void initObjectType();
  void initTargetType();
  void initOptionType();
  void initActionType();
  void initStringType();
  void initPlatformVars();
};

// Functions to initialize various built-in namespaces.

void initModuleType();

void initConsoleMethods(Fundamentals * fundamentals);
void initPathMethods(Fundamentals * fundamentals);
void initListType();
void initSubprocessMethods(Fundamentals * fundamentals);
void initDirSearchMethods(Fundamentals * fundamentals);
void initFileMethods(Fundamentals * fundamentals);
void initRegExMethods(Fundamentals * fundamentals);

}

#endif // MINT_INTRINSIC_FUNDAMENTALS_H
