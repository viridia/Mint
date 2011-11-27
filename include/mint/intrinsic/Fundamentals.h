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
//  unsigned targetMake;      // Field index of target.make()

  /// Base prototype of all tools.
  //  unsigned toolName;        // Field index of tool.name
  //  unsigned toolExec;        // Field index of tool.exec
  //  unsigned toolArgs;        // Field index of tool.args
  //  unsigned toolRun;         // Field index of tool.run()

  /// Base prototype for options.
  Object * option;

  /// Base prototype for lists.
  Object * list;

  /// Base prototype for dicts.
  Object * dict;

  /// Constructor for the root module.
  Fundamentals();

  /// Registry of all types
  TypeRegistry & typeRegistry() { return _typeRegistry; }

  /// Intern string function
  String * str(StringRef in);

  // Mint commands

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

void initConsole(Fundamentals * fundamentals);
void initPath(Fundamentals * fundamentals);
void initListType(Fundamentals * fundamentals);
void initSubprocess(Fundamentals * fundamentals);

}

#endif // MINT_INTRINSIC_FUNDAMENTALS_H
