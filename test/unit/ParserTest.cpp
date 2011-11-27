/* ================================================================== *
 * Diagnostics unit test
 * ================================================================== */

#include "gtest/gtest.h"

#include "mint/graph/Literal.h"
#include "mint/graph/Module.h"
#include "mint/graph/TypeRegistry.h"

#include "mint/parse/Parser.h"
#include "mint/support/Diagnostics.h"

#include <ostream>

namespace mint {

// Make nodes streamable so they can be printed in test results.
inline ::std::ostream & operator<<(::std::ostream & out, const Node * node) {
  mint::OStrStream stream;
  node->print(stream);
  std::string s(stream.str().data(), stream.str().size());
  out << s;
  return out;
}

// Make nodes streamable so they can be printed in test results.
inline ::std::ostream & operator<<(::std::ostream & out, Node::NodeKind kind) {
  out << Node::kindName(kind);
  return out;
}

class ParserTest : public testing::Test {
public:
  Module module;
  OStrStream errorStrm;

  ParserTest() : module(Node::NK_MODULE, "", NULL) {}

  Node * parse(Node * (Parser::*parseFunc)(), StringRef src) {
    OStream * saveStream = diag::setOutputStream(&errorStrm);

    TextBuffer buffer(src);
    Parser parser(&buffer);
    Node * result = (parser.*parseFunc)();

    diag::setOutputStream(saveStream);
    diag::reset();
    return result;
  }

  Node * parseExpression(const char * srctext) {
    return parse(&Parser::expression, srctext);
  }

  // Compare an AST node with its string representation
  static inline bool nodeEq(StringRef expected, const Node * actual) {
    if (actual != NULL) {
      OStrStream stream;
      actual->print(stream);
      //stream.flush();
      return stream.str() == expected;
    }

    return false;
  }
};

#define ASSERT_NODE_EQ(expected, actual) \
  ASSERT_PRED2(nodeEq, expected, actual);

#define EXPECT_NODE_EQ(expected, actual) \
  EXPECT_PRED2(nodeEq, expected, actual);

TEST_F(ParserTest, Terminals) {
  Node * n;

  // INTEGER
  n = parseExpression("10");
  ASSERT_EQ(Node::NK_INTEGER, n->nodeKind());
  EXPECT_PRED2(nodeEq, "10", n);
  EXPECT_EQ(0u, n->location().begin);
  EXPECT_EQ(2u, n->location().end);

  n = parseExpression("0x10");
  ASSERT_EQ(Node::NK_INTEGER, n->nodeKind());
  EXPECT_PRED2(nodeEq, "16", n);
  EXPECT_EQ(0u, n->location().begin);
  EXPECT_EQ(4u, n->location().end);

  n = parseExpression("0x100000000");
  ASSERT_EQ(Node::NK_INTEGER, n->nodeKind());
  EXPECT_PRED2(nodeEq, "4294967296", n);
  EXPECT_EQ(0u, n->location().begin);
  EXPECT_EQ(11u, n->location().end);

  // Floats
  n = parseExpression("1.0");
  ASSERT_EQ(Node::NK_FLOAT, n->nodeKind());
  ASSERT_FLOAT_EQ(1.0, static_cast<Literal<double> *>(n)->value());
  EXPECT_EQ(0u, n->location().begin);
  EXPECT_EQ(3u, n->location().end);

  n = parseExpression("5.0e3");
  ASSERT_EQ(Node::NK_FLOAT, n->nodeKind());
  ASSERT_FLOAT_EQ(5.0e3, static_cast<Literal<double> *>(n)->value());

  // Character literals
  n = parseExpression("'c'");
  ASSERT_EQ(Node::NK_STRING, n->nodeKind());
  EXPECT_PRED2(nodeEq, "'c'", n);
  EXPECT_EQ(0u, n->location().begin);
  EXPECT_EQ(3u, n->location().end);

  // String literals
  n = parseExpression("\"c\"");
  ASSERT_EQ(Node::NK_STRING, n->nodeKind());
  EXPECT_PRED2(nodeEq, "'c'", n);
  EXPECT_EQ(1u, n->location().begin);
  EXPECT_EQ(2u, n->location().end);

  // Boolean literals
  n = parseExpression("true");
  ASSERT_EQ(Node::NK_BOOL, n->nodeKind());
  EXPECT_PRED2(nodeEq, "true", n);
  EXPECT_EQ(0u, n->location().begin);
  EXPECT_EQ(4u, n->location().end);

  n = parseExpression("false");
  ASSERT_EQ(Node::NK_BOOL, n->nodeKind());
  EXPECT_PRED2(nodeEq, "false", n);

  // Identifier
  n = parseExpression("X");
  ASSERT_EQ(Node::NK_IDENT, n->nodeKind());
  EXPECT_PRED2(nodeEq, "X", n);

  // Type names

  n = parseExpression("any");
  ASSERT_EQ(Node::NK_TYPENAME, n->nodeKind());
  EXPECT_PRED2(nodeEq, "any", n);

  n = parseExpression("bool");
  ASSERT_EQ(Node::NK_TYPENAME, n->nodeKind());
  EXPECT_PRED2(nodeEq, "bool", n);

  n = parseExpression("int");
  ASSERT_EQ(Node::NK_TYPENAME, n->nodeKind());
  EXPECT_PRED2(nodeEq, "int", n);

  n = parseExpression("float");
  ASSERT_EQ(Node::NK_TYPENAME, n->nodeKind());
  EXPECT_PRED2(nodeEq, "float", n);

  n = parseExpression("string");
  ASSERT_EQ(Node::NK_TYPENAME, n->nodeKind());
  EXPECT_PRED2(nodeEq, "string", n);

  n = parseExpression("list");
  ASSERT_EQ(Node::NK_TYPENAME, n->nodeKind());
  EXPECT_EQ(TypeRegistry::genericListType(), n);

  n = parseExpression("dict");
  ASSERT_EQ(Node::NK_TYPENAME, n->nodeKind());
  EXPECT_EQ(TypeRegistry::genericDictType(), n);
}

TEST_F(ParserTest, TypeNames) {

  // Parsing tests for type names.
#if 0
  Node * n;
  // Function type
//  n = parseType("fn :int32 -> int32");
//  ASSERT_EQ(Node::NK_AnonFn, n->nodeType());
//  EXPECT_NODE_EQ("fn (:int32) -> int32", n);
//
//  // Function type
//  n = parseType("fn (x) -> int32");
//  ASSERT_EQ(Node::NK_AnonFn, n->nodeType());
//  EXPECT_NODE_EQ("fn (x) -> int32", n);
//
//  n = parseType("fn (:int32) -> int32");
//  ASSERT_EQ(Node::NK_AnonFn, n->nodeType());
//  EXPECT_NODE_EQ("fn (:int32) -> int32", n);
//
//  n = parseType("fn (x:int32) -> int32");
//  ASSERT_EQ(Node::NK_AnonFn, n->nodeType());
//  EXPECT_NODE_EQ("fn (x:int32) -> int32", n);
//
//  n = parseType("fn (xx:int32, y:int32) -> int32");
//  ASSERT_EQ(Node::NK_AnonFn, n->nodeType());
//  EXPECT_NODE_EQ("fn (xx:int32, y:int32) -> int32", n);
//
//  n = parseType("fn (:int32 -> int32", 1);
//  ASSERT_EQ(Node::NK_AnonFn, n->nodeType());
//  //ASSERT_EQ(NULL, n);
#endif
}

TEST_F(ParserTest, SimpleExpressions) {
  Node * n;

  n = parseExpression("1+1");
  ASSERT_EQ(Node::NK_ADD, n->nodeKind());
  EXPECT_NODE_EQ("ADD(1, 1)", n);
  EXPECT_EQ(0u, n->location().begin);
  EXPECT_EQ(3u, n->location().end);

  n = parseExpression("1-1");
  ASSERT_EQ(Node::NK_SUBTRACT, n->nodeKind());
  EXPECT_NODE_EQ("SUBTRACT(1, 1)", n);
  EXPECT_EQ(0u, n->location().begin);
  EXPECT_EQ(3u, n->location().end);

  n = parseExpression("1*1");
  ASSERT_EQ(Node::NK_MULTIPLY, n->nodeKind());
  EXPECT_NODE_EQ("MULTIPLY(1, 1)", n);
  EXPECT_EQ(0u, n->location().begin);
  EXPECT_EQ(3u, n->location().end);

  n = parseExpression("1/1");
  ASSERT_EQ(Node::NK_DIVIDE, n->nodeKind());
  EXPECT_NODE_EQ("DIVIDE(1, 1)", n);
  EXPECT_EQ(0u, n->location().begin);
  EXPECT_EQ(3u, n->location().end);

  n = parseExpression("1%1");
  ASSERT_EQ(Node::NK_MODULUS, n->nodeKind());
  EXPECT_NODE_EQ("MODULUS(1, 1)", n);
  EXPECT_EQ(0u, n->location().begin);
  EXPECT_EQ(3u, n->location().end);

//  n = parseExpression("1&1");
//  ASSERT_EQ(Node::NK_Call, n->nodeKind());
//  EXPECT_NODE_EQ("infixBitAnd(1, 1)", n);
//  EXPECT_EQ(0u, n->location().begin);
//  EXPECT_EQ(3u, n->location().end);
//
//  n = parseExpression("1|1");
//  ASSERT_EQ(Node::NK_Call, n->nodeKind());
//  EXPECT_NODE_EQ("infixBitOr(1, 1)", n);
//  EXPECT_EQ(0u, n->location().begin);
//  EXPECT_EQ(3u, n->location().end);
//
//  n = parseExpression("1^1");
//  ASSERT_EQ(Node::NK_Call, n->nodeKind());
//  EXPECT_NODE_EQ("infixBitXor(1, 1)", n);
//  EXPECT_EQ(0u, n->location().begin);
//  EXPECT_EQ(3u, n->location().end);

  n = parseExpression("1 and 1");
  ASSERT_EQ(Node::NK_AND, n->nodeKind());
  EXPECT_NODE_EQ("AND(1, 1)", n);
  EXPECT_EQ(0u, n->location().begin);
  EXPECT_EQ(7u, n->location().end);

  n = parseExpression("1 or 1");
  ASSERT_EQ(Node::NK_OR, n->nodeKind());
  EXPECT_NODE_EQ("OR(1, 1)", n);
  EXPECT_EQ(0u, n->location().begin);
  EXPECT_EQ(6u, n->location().end);

//  n = parseExpression("1 << 1");
//  ASSERT_EQ(Node::NK_Call, n->nodeKind());
//  EXPECT_NODE_EQ("infixLShift(1, 1)", n);
//
//  n = parseExpression("1 >> 1");
//  ASSERT_EQ(Node::NK_Call, n->nodeKind());
//  EXPECT_NODE_EQ("infixRShift(1, 1)", n);
//
//  n = parseExpression("1..1");
//  ASSERT_EQ(Node::NK_Range, n->nodeKind());
//  EXPECT_NODE_EQ("Range(1, 1)", n);

  n = parseExpression("1==1");
  ASSERT_EQ(Node::NK_EQUAL, n->nodeKind());
  EXPECT_NODE_EQ("EQUAL(1, 1)", n);

  n = parseExpression("1!=1");
  ASSERT_EQ(Node::NK_NOT_EQUAL, n->nodeKind());
  EXPECT_NODE_EQ("NOT_EQUAL(1, 1)", n);

  n = parseExpression("1<1");
  ASSERT_EQ(Node::NK_LESS, n->nodeKind());
  EXPECT_NODE_EQ("LESS(1, 1)", n);

  n = parseExpression("1>1");
  ASSERT_EQ(Node::NK_GREATER, n->nodeKind());
  EXPECT_NODE_EQ("GREATER(1, 1)", n);

  n = parseExpression("1<=1");
  ASSERT_EQ(Node::NK_LESS_EQUAL, n->nodeKind());
  EXPECT_NODE_EQ("LESS_EQUAL(1, 1)", n);

  n = parseExpression("1>=1");
  ASSERT_EQ(Node::NK_GREATER_EQUAL, n->nodeKind());
  EXPECT_NODE_EQ("GREATER_EQUAL(1, 1)", n);

  n = parseExpression("X.Y");
  ASSERT_EQ(Node::NK_GET_MEMBER, n->nodeKind());
  //EXPECT_PRED2(nodeEq, "X.Y", n);
}

TEST_F(ParserTest, CollectionLiterals) {
  Node * n;

  // Tuple
  n = parseExpression("(1, 1)");
  ASSERT_EQ(Node::NK_MAKE_TUPLE, n->nodeKind());
  EXPECT_NODE_EQ("MAKE_TUPLE(1, 1)", n);

  // Tuple error recovery
  n = parseExpression("(1, ? 1)");
  ASSERT_EQ(Node::NK_MAKE_TUPLE, n->nodeKind());
  EXPECT_NODE_EQ("MAKE_TUPLE(1)", n);

  n = parseExpression("(1, = 1)");
  ASSERT_EQ(Node::NK_MAKE_TUPLE, n->nodeKind());
  EXPECT_NODE_EQ("MAKE_TUPLE(1)", n);

  // List
  n = parseExpression("[1, 1]");
  ASSERT_EQ(Node::NK_MAKE_LIST, n->nodeKind());
  EXPECT_NODE_EQ("MAKE_LIST(1, 1)", n);

  // List error recovery
  n = parseExpression("[1, ? 1]");
  ASSERT_EQ(Node::NK_MAKE_LIST, n->nodeKind());
  EXPECT_NODE_EQ("MAKE_LIST(1)", n);

  n = parseExpression("[1, = 1]");
  ASSERT_EQ(Node::NK_MAKE_LIST, n->nodeKind());
  EXPECT_NODE_EQ("MAKE_LIST(1)", n);

  // Dict
  n = parseExpression("{1=1, 2=2}");
  ASSERT_EQ(Node::NK_MAKE_DICT, n->nodeKind());
  EXPECT_NODE_EQ("MAKE_DICT(1, 1, 2, 2)", n);

  // Dict error recovery
  n = parseExpression("{1=1, ? 2=2}");
  ASSERT_EQ(Node::NK_MAKE_DICT, n->nodeKind());
  EXPECT_NODE_EQ("MAKE_DICT(1, 1)", n);

  n = parseExpression("{1=1, = 2=2}");
  ASSERT_EQ(Node::NK_MAKE_DICT, n->nodeKind());
  EXPECT_NODE_EQ("MAKE_DICT(1, 1)", n);
}

TEST_F(ParserTest, StringInterpolation) {
  Node * n;

  // String interpolation
  n = parseExpression("\"ab${x}cd${y.z}ef\"");
  ASSERT_EQ(Node::NK_CONCAT, n->nodeKind());
  EXPECT_NODE_EQ("CONCAT('ab', x, 'cd', GET_MEMBER(y, z), 'ef')", n);
}

}
