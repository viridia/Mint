/* ================================================================== *
 * Mint
 * ================================================================== */

#ifndef MINT_PROJECT_OPTIONFINDER_H
#define MINT_PROJECT_OPTIONFINDER_H

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
    Class to find all options in the graph.
 */
class OptionFinder : public GraphVisitor<void> {
public:

  /// Constructor
  OptionFinder(Project * project);

  // overrides

  void visitObject(Object * obj);

private:
  Project * _project;
  Object * _optionProto;
  Table<Object, Node> _visited;
};

}

#endif // MINT_PROJECT_OPTIONFINDER_H
