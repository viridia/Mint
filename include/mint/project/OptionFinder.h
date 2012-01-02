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
  typedef SmallVectorImpl<Object *> OptionList;
  typedef OptionList::const_iterator const_iterator;

  /// Constructor
  OptionFinder(Project * project);

  /// Find all options visible to the project
  void visitModules();

  /// The list of all options found.
  const OptionList & options() const { return _options; }
  OptionList::const_iterator begin() const { return _options.begin(); }
  OptionList::const_iterator end() const { return _options.end(); }

  // overrides

  void visitObject(Object * obj);

private:
  Project * _project;
  Object * _optionProto;
  Table<Object, Node, ObjectPointerKeyTraits> _visited;
  SmallVector<Object *, 16> _options;
};

}

#endif // MINT_PROJECT_OPTIONFINDER_H
