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
  MakefileGenerator(StringRef outputPath, Module * module, TargetMgr * targetMgr)
    : _outputPath(outputPath)
    , _activeScope(module)
    , _module(module)
    , _targetMgr(targetMgr)
  {}

  void writeModule();

  /// Return the stream that this writes to.
  OStream & strm() { return _strm; }

protected:
  void writeTarget(Target * target);
  void writeExternalTarget(Target * target);
  void writeAction(Oper * action);
  void writeRelativePath(StringRef path);
  void makeRelative(StringRef path, SmallVectorImpl<char> & result);

  String * uniqueName(String * stem);

  Node * setActiveScope(Node * scope) {
    Node * prevScope = _activeScope;
    _activeScope = scope;
    return prevScope;
  }

  OStrStream _strm;
  SmallString<32> _outputPath;
  Node * _activeScope;
  Module * _module;
  TargetMgr * _targetMgr;
  SmallVector<String *, 16> _outputs;
  StringDict<Node> _uniqueNames;
};

}

#endif // MINT_PROJECT_MAKEFILE_GENERATOR_H
