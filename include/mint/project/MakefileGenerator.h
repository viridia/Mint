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
  void writeAction(Oper * action, StringDict<char> & depSet);
  void writeRelativePath(StringRef path);

  Node * setActiveScope(Node * scope) {
    Node * prevScope = _activeScope;
    _activeScope = scope;
    return prevScope;
  }

  /// Return true if we can access 'obj' by name from the current active scope.
//  bool hasRelativePath(Object * obj);

  OStream & _strm;
  Node * _activeScope;
  Module * _module;
  TargetMgr * _targetMgr;
};

}

#endif // MINT_PROJECT_MAKEFILE_GENERATOR_H
