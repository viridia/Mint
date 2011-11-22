/* ================================================================== *
 * Diagnostics unit test
 * ================================================================== */

#include "gtest/gtest.h"

#include "mint/eval/Evaluator.h"

#include "mint/graph/Literal.h"
#include "mint/graph/Module.h"

#include "mint/intrinsic/Fundamentals.h"

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

class EvaluatorTest : public testing::Test {
public:
  Pool alloc;
  Module module;
  OStrStream errorStrm;
  Fundamentals * fundamentals;

  EvaluatorTest() : module(Node::NK_MODULE, "", NULL) {
    fundamentals = new Fundamentals();
    module.addImportScope(fundamentals);
  }

  Node * parse(Node * (Parser::*parseFunc)(), StringRef src) {
    OStream * saveStream = diag::setOutputStream(&errorStrm);

    TextBuffer buffer(src);
    Parser parser(&buffer);
    Node * result = (parser.*parseFunc)();

    diag::setOutputStream(saveStream);
    diag::reset();
    return result;
  }

  Node * eval(Node * n) {
    OStream * saveStream = diag::setOutputStream(&errorStrm);
    Evaluator ev(&module);
    Node * result = ev.eval(n);
    diag::setOutputStream(saveStream);
    diag::reset();
    return result;
  }

  Node * parseExpression(const char * srctext) {
    return parse(&Parser::expression, srctext);
  }

  Node * evalExpression(const char * srctext) {
    Node * n = parse(&Parser::expression, srctext);
    if (n != NULL) {
      n = eval(n);
    }
    return n;
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

TEST_F(EvaluatorTest, Terminals) {
  Node * n;

  // INTEGER
  n = evalExpression("10");
  ASSERT_EQ(Node::NK_INTEGER, n->nodeKind());
  ASSERT_PRED2(nodeEq, "10", n);
  EXPECT_EQ(0u, n->location().begin);
  EXPECT_EQ(2u, n->location().end);

  // ASTIdent
//  n = evalExpression("X");
//  ASSERT_EQ(Node::NK_IDENT, n->nodeKind());
//  ASSERT_PRED2(nodeEq, "X", n);
}

TEST_F(EvaluatorTest, SimpleExpressions) {
  Node * n;

  // Add

  n = evalExpression("1+1");
  ASSERT_EQ(Node::NK_INTEGER, n->nodeKind());
  ASSERT_EQ(2, static_cast<Literal<int> *>(n)->value());

  n = evalExpression("1.0+2.0");
  ASSERT_EQ(Node::NK_FLOAT, n->nodeKind());
  ASSERT_FLOAT_EQ(3.0, static_cast<Literal<double> *>(n)->value());

  // Subtract

  n = evalExpression("3-1");
  ASSERT_EQ(Node::NK_INTEGER, n->nodeKind());
  ASSERT_EQ(2, static_cast<Literal<int> *>(n)->value());

  n = evalExpression("3.0-1.0");
  ASSERT_EQ(Node::NK_FLOAT, n->nodeKind());
  ASSERT_FLOAT_EQ(2.0, static_cast<Literal<double> *>(n)->value());

  // Multiply

  n = evalExpression("2*2");
  ASSERT_EQ(Node::NK_INTEGER, n->nodeKind());
  ASSERT_EQ(4, static_cast<Literal<int> *>(n)->value());

  n = evalExpression("2.0*2.0");
  ASSERT_EQ(Node::NK_FLOAT, n->nodeKind());
  ASSERT_EQ(4.0, static_cast<Literal<double> *>(n)->value());

  // Divide

  n = evalExpression("8/2");
  ASSERT_EQ(Node::NK_INTEGER, n->nodeKind());
  ASSERT_EQ(4, static_cast<Literal<int> *>(n)->value());

  n = evalExpression("8.0/2.0");
  ASSERT_EQ(Node::NK_FLOAT, n->nodeKind());
  ASSERT_EQ(4.0, static_cast<Literal<double> *>(n)->value());

  // Modulus

  n = evalExpression("8%2");
  ASSERT_EQ(Node::NK_INTEGER, n->nodeKind());
  ASSERT_EQ(0, static_cast<Literal<int> *>(n)->value());

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

  n = parseExpression("1 or 1");
  ASSERT_EQ(Node::NK_OR, n->nodeKind());
  EXPECT_NODE_EQ("OR(1, 1)", n);

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
  //ASSERT_PRED2(nodeEq, "X.Y", n);
}

}
