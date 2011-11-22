/* ================================================================== *
 * Intrinsic functions dealing with files and directories.
 * ================================================================== */

#ifndef MINT_INTRINSIC_FILESYSTEM_H
#define MINT_INTRINSIC_FILESYSTEM_H

#ifndef MINT_GRAPH_OPER_H
#include "mint/graph/Oper.h"
#endif

namespace mint {

class Evaluator;
class Function;

namespace fs {

/// Recursively walk the filesystem and return entries matching the input pattern.
///
/// Type signature: glob(string) -> list[string]
///
Node * methodGlob(Evaluator * ex, Function * fn, Node * self, NodeArray args);

}}

#endif // MINT_INTRINSIC_FILESYSTEM_H
