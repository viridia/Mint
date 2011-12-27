/* ================================================================== *
 * Mint
 * ================================================================== */

#ifndef MINT_PROJECT_TARGETFINDER_H
#define MINT_PROJECT_TARGETFINDER_H

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

class TargetMgr;
class Target;

/** -------------------------------------------------------------------------
    Class to find all targets in the graph.
 */
class TargetFinder : public GraphVisitor<void> {
public:

  /// Constructor
  TargetFinder(TargetMgr * targetMgr, Project * project);

  // overrides

  void visitObject(Object * obj);

private:
  void addDependenciesToTarget(Target * target, Oper * list);
  void addSourcesToTarget(Target * target, Oper * list, StringRef baseDir);
  void addOutputsToTarget(Target * target, Oper * list, StringRef baseDir);
  String * makeAbsolute(String * filepath, StringRef baseDir);

  TargetMgr * _targetMgr;
  Project * _project;
  Object * _targetProto;
};

}

#endif // MINT_PROJECT_TARGETFINDER_H
