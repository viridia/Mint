/* ================================================================== *
 * Mint: A refreshing approach to build configuration.
 * ================================================================== */

#ifndef MINT_GRAPH_GRAPHWRITER_H
#define MINT_GRAPH_GRAPHWRITER_H

#ifndef MINT_GRAPH_FUNCTION_H
#include "mint/graph/Function.h"
#endif

namespace mint {

class OStream;
class Node;
class Module;
class Object;
class Oper;

/** -------------------------------------------------------------------------
    Class to serialize a graph to an output stream.
 */
class GraphWriter {
public:
  /// Constructor.
  GraphWriter(OStream & strm)
    : _strm(strm)
    , _indentLevel(0)
  {}

  GraphWriter & write(Node * node);
  GraphWriter & write(Module * module);
  GraphWriter & write(Object * module);

private:
  void writeList(Oper * list);
  void writeDict(Oper * dict);

  OStream & _strm;
  unsigned _indentLevel;
};

}

#endif // MINT_GRAPH_GRAPHWRITER_H
