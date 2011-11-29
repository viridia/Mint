/* ================================================================== *
 * Fundamentals - the built-in root module.
 * ================================================================== */

#ifndef MINT_INTRINSIC_FUNDAMENTALS_H
#define MINT_INTRINSIC_FUNDAMENTALS_H

#ifndef MINT_GRAPH_MODULE_H
#include "mint/graph/Module.h"
#endif

#ifndef MINT_GRAPH_TYPEREGISTRY_H
#include "mint/graph/TypeRegistry.h"
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

  /// Base prototype for regular expression objects.
  Object * regex;

  /// Constructor for the root module.
  Fundamentals();

  /// Registry of all types
  TypeRegistry & typeRegistry() { return _typeRegistry; }

  /// Intern string function
  String * str(StringRef in);

  /// Return the global Fundamentals object.
  static Fundamentals * get();

  // Overrides

  void trace() const;

private:
  void defineObjectProto();
  void defineTargetProto();
  void defineOptionProto();

  TypeRegistry _typeRegistry;
  StringDict<Node> _strings;
};

// Functions to initialize various built-in namespaces.

void initConsoleMethods(Fundamentals * fundamentals);
void initPathMethods(Fundamentals * fundamentals);
void initListMethods(Fundamentals * fundamentals);
void initSubprocessMethods(Fundamentals * fundamentals);
void initDirSearchMethods(Fundamentals * fundamentals);
void initFileCopyMethods(Fundamentals * fundamentals);
void initRegExMethods(Fundamentals * fundamentals);

}

#endif // MINT_INTRINSIC_FUNDAMENTALS_H
