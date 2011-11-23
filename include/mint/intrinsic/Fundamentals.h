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
  Ref<Object> object;

  /// Base prototype of all targets.
  Ref<Object> target;
//  unsigned targetMake;      // Field index of target.make()

  /// Base prototype of all tools.
  Ref<Object> tool;
//  unsigned toolName;        // Field index of tool.name
//  unsigned toolExec;        // Field index of tool.exec
//  unsigned toolArgs;        // Field index of tool.args
//  unsigned toolRun;         // Field index of tool.run()

  /// Base prototype of all builders
  Ref<Object> builder;

  /// Base prototype for options.
  Ref<Object> option;

  /// Constructor for the root module.
  Fundamentals();

  /// Registry of all types
  TypeRegistry & typeRegistry() { return _typeRegistry; }

  /// Intern string function
  String * str(StringRef in);

  // Mint commands

private:
  void defineBuilderProto();
  void defineOptionProto();

  TypeRegistry _typeRegistry;
  StringDict<Node> _strings;
};

// Functions to initialize various built-in namespaces.

void initConsole(Fundamentals * fundamentals);
void initPath(Fundamentals * fundamentals);

}

#endif // MINT_INTRINSIC_FUNDAMENTALS_H
