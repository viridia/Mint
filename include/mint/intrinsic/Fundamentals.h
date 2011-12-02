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
  /// Base prototype of all objects.
  Object * object;

  /// Base prototype of all targets.
  Object * target;

  /// Base prototype for options.
  Object * option;

  /// Base prototype for lists.
  Object * list;

  /// Base prototype for dicts.
  Object * dict;

  /// Constructor for the root module.
  Fundamentals();

  /// Intern string function
  String * str(StringRef in);

  /// Set an attribute on this module.
  void setProperty(String * name, Node * value) { Module::setProperty(name, value); }
  void setProperty(StringRef name, Node * value) { Module::setProperty(str(name), value); }

  /// Create a named object in the fundamentals namespace.
  Object * createChildObject(StringRef name, Object * prototype = NULL);

  /// Create a named dictionary in the fundamentals namespace.
  Object * createChildScope(StringRef name);

  /// Return the global Fundamentals object.
  static Fundamentals & get();

  // Overrides

  void trace() const;

private:
  void defineObjectProto();
  void defineTargetProto();
  void defineOptionProto();
};

// Functions to initialize various built-in namespaces.

void initModuleType();

void initConsoleMethods(Fundamentals * fundamentals);
void initPathMethods(Fundamentals * fundamentals);
void initListMethods(Fundamentals * fundamentals);
void initSubprocessMethods(Fundamentals * fundamentals);
void initDirSearchMethods(Fundamentals * fundamentals);
void initFileMethods(Fundamentals * fundamentals);
void initRegExMethods(Fundamentals * fundamentals);

}

#endif // MINT_INTRINSIC_FUNDAMENTALS_H
