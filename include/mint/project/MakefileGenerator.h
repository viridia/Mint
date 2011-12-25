/* ================================================================== *
 * Mint: A refreshing approach to build configuration.
 * ================================================================== */

#ifndef MINT_PROJECT_MAKEFILE_GENERATOR_H
#define MINT_PROJECT_MAKEFILE_GENERATOR_H

#ifndef MINT_GRAPH_FUNCTION_H
#include "mint/graph/Function.h"
#endif

namespace mint {

class OStream;
class Module;

/** -------------------------------------------------------------------------
    Class to serialize a module into a makefile.
 */
class MakefileGenerator {
public:
  /// Constructor.
  MakefileGenerator(OStream & strm, Module * module, TargetMgr * targetMgr)
    : _strm(strm)
    , _activeScope(module)
    , _module(module)
    , _targetMgr(targetMgr)
  {}

  void writeModule();

  /// Return the stream that this writes to.
  OStream & strm() { return _strm; }

protected:
  void writeTarget(Target * target);
  void writeAction(Oper * action);
  void writeRelativePath(StringRef path);
  void makeRelative(StringRef path, SmallVectorImpl<char> & result);

  Node * setActiveScope(Node * scope) {
    Node * prevScope = _activeScope;
    _activeScope = scope;
    return prevScope;
  }

  OStream & _strm;
  Node * _activeScope;
  Module * _module;
  TargetMgr * _targetMgr;
  SmallVector<String *, 16> _outputs;
};

}

#endif // MINT_PROJECT_MAKEFILE_GENERATOR_H
