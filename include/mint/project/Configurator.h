/* ================================================================== *
 * Mint
 * ================================================================== */

#ifndef MINT_PROJECT_CONFIGURATOR_H
#define MINT_PROJECT_CONFIGURATOR_H

#ifndef MINT_PROJECT_PROJECT_H
#include "mint/project/Project.h"
#endif

#ifndef MINT_GRAPH_GRAPHVISITOR_H
#include "mint/graph/GraphVisitor.h"
#endif

#ifndef MINT_EVAL_EVALUATOR_H
#include "mint/eval/Evaluator.h"
#endif

namespace mint {

/** -------------------------------------------------------------------------
    The built-in root module.
 */
class Configurator : public GraphVisitor<void> {
public:

  /// Constructor
  Configurator(Project * project, Module * module) : _project(project), _eval(module) {}

  /// Execute configuration-time actions.
  void performActions(Module * module);

  // overrides

  void visitObject(Object * obj);


private:
  Project * _project;
  Evaluator _eval;
};

}

#endif // MINT_PROJECT_CONFIGURATOR_H
