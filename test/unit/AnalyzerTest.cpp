/* ================================================================== *
 * Diagnostics unit test
 * ================================================================== */

#include "gtest/gtest.h"
#if 0
#include "mint/eval/Analyzer.h"

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

class AnalyzerTest : public testing::Test {
public:
  Pool alloc;
  Module module;
  OStrStream errorStrm;
  Fundamentals * fundamentals;

  AnalyzerTest() : module(Node::NK_MODULE, "", NULL) {
    fundamentals = new Fundamentals();
    module.addImportScope(fundamentals);
  }

  Node * parse(Node * (Parser::*parseFunc)(), StringRef src) {
    OStream * saveStream = diag::setOutputStream(&errorStrm);

    TextBuffer * buffer = new TextBuffer(src);
    module.setTextBuffer(buffer);
    Parser parser(buffer);
    Node * result = (parser.*parseFunc)();

    diag::setOutputStream(saveStream);
    diag::reset();
    return result;
  }

  Node * analyze(Node * n, Type * expected = TypeRegistry::anyType()) {
    OStream * saveStream = diag::setOutputStream(&errorStrm);
    Analyzer ev(&module);
    Node * result = ev.analyze(n, expected);
    diag::setOutputStream(saveStream);
    diag::reset();
    return result;
  }

  Node * parseExpression(const char * srctext) {
    return parse(&Parser::expression, srctext);
  }

  Node * analyzeExpression(const char * srctext) {
    Node * n = parse(&Parser::expression, srctext);
    if (n != NULL) {
      n = analyze(n);
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

TEST_F(AnalyzerTest, Terminals) {
  Node * n;

  // BOOL
  n = analyzeExpression("true");
  ASSERT_EQ(Node::NK_BOOL, n->nodeKind());
  ASSERT_EQ(Type::BOOL, n->type()->typeKind());
  ASSERT_PRED2(nodeEq, "true", n);

  // INTEGER
  n = analyzeExpression("10");
  ASSERT_EQ(Node::NK_INTEGER, n->nodeKind());
  ASSERT_EQ(Type::INTEGER, n->type()->typeKind());
  ASSERT_PRED2(nodeEq, "10", n);

  // FLOAT
  n = analyzeExpression("10.0");
  ASSERT_EQ(Node::NK_FLOAT, n->nodeKind());
  ASSERT_EQ(Type::FLOAT, n->type()->typeKind());
  ASSERT_FLOAT_EQ(10.0, static_cast<Literal<double> *>(n)->value());

  // STRING
  n = analyzeExpression("\"a\"");
  ASSERT_EQ(Node::NK_STRING, n->nodeKind());
  ASSERT_EQ(Type::STRING, n->type()->typeKind());

  // UNDEFINED_VALUE
//  n = analyzeExpression("undefined");
//  ASSERT_EQ(Node::UNDEFINED, n->nodeKind());
//  ASSERT_EQ(Type::ANY, n->type()->typeKind());
}

TEST_F(AnalyzerTest, Coercions) {
  Analyzer an(&module);
  Node * n;

  // Constant int <-> float

  n = an.coerce(analyzeExpression("1"), TypeRegistry::integerType());
  ASSERT_TRUE(n->type()->isIntegerType());
  ASSERT_EQ(1, static_cast<Literal<int> *>(n)->value());

  n = an.coerce(analyzeExpression("1"), TypeRegistry::floatType());
  ASSERT_TRUE(n->type()->isFloatType());
  ASSERT_FLOAT_EQ(1.0, static_cast<Literal<double> *>(n)->value());

  n = an.coerce(analyzeExpression("1.0"), TypeRegistry::integerType());
  ASSERT_TRUE(n->type()->isIntegerType());
  ASSERT_EQ(1, static_cast<Literal<int> *>(n)->value());

  n = an.coerce(analyzeExpression("1.0"), TypeRegistry::floatType());
  ASSERT_TRUE(n->type()->isFloatType());
  ASSERT_FLOAT_EQ(1.0, static_cast<Literal<double> *>(n)->value());
}

TEST_F(AnalyzerTest, SimpleExpressions) {
  Node * n;

  // Add

  n = analyzeExpression("1+1");
  ASSERT_EQ(Node::NK_INTEGER, n->nodeKind());
  ASSERT_TRUE(n->type()->isIntegerType());
  ASSERT_EQ(2, static_cast<Literal<int> *>(n)->value());

  n = analyzeExpression("1+1.0");
  ASSERT_EQ(Node::NK_FLOAT, n->nodeKind());
  ASSERT_TRUE(n->type()->isFloatType());
  ASSERT_FLOAT_EQ(2.0, static_cast<Literal<double> *>(n)->value());

  n = analyzeExpression("1.0+1");
  ASSERT_EQ(Node::NK_FLOAT, n->nodeKind());
  ASSERT_TRUE(n->type()->isFloatType());
  ASSERT_FLOAT_EQ(2.0, static_cast<Literal<double> *>(n)->value());

  n = analyzeExpression("1.0+2.0");
  ASSERT_EQ(Node::NK_FLOAT, n->nodeKind());
  ASSERT_TRUE(n->type()->isFloatType());
  ASSERT_FLOAT_EQ(3.0, static_cast<Literal<double> *>(n)->value());

  // Subtract

  n = analyzeExpression("3-1");
  ASSERT_EQ(Node::NK_INTEGER, n->nodeKind());
  ASSERT_TRUE(n->type()->isIntegerType());
  ASSERT_EQ(2, static_cast<Literal<int> *>(n)->value());

  n = analyzeExpression("3.0-1");
  ASSERT_EQ(Node::NK_FLOAT, n->nodeKind());
  ASSERT_TRUE(n->type()->isFloatType());
  ASSERT_FLOAT_EQ(2.0, static_cast<Literal<double> *>(n)->value());

  n = analyzeExpression("3-1.0");
  ASSERT_EQ(Node::NK_FLOAT, n->nodeKind());
  ASSERT_TRUE(n->type()->isFloatType());
  ASSERT_FLOAT_EQ(2.0, static_cast<Literal<double> *>(n)->value());

  n = analyzeExpression("3.0-1.0");
  ASSERT_EQ(Node::NK_FLOAT, n->nodeKind());
  ASSERT_TRUE(n->type()->isFloatType());
  ASSERT_FLOAT_EQ(2.0, static_cast<Literal<double> *>(n)->value());

  // Multiply

  n = analyzeExpression("2*2");
  ASSERT_EQ(Node::NK_INTEGER, n->nodeKind());
  ASSERT_TRUE(n->type()->isIntegerType());
  ASSERT_EQ(4, static_cast<Literal<int> *>(n)->value());

  n = analyzeExpression("2.0*2");
  ASSERT_EQ(Node::NK_FLOAT, n->nodeKind());
  ASSERT_TRUE(n->type()->isFloatType());
  ASSERT_EQ(4.0, static_cast<Literal<double> *>(n)->value());

  n = analyzeExpression("2*2.0");
  ASSERT_EQ(Node::NK_FLOAT, n->nodeKind());
  ASSERT_TRUE(n->type()->isFloatType());
  ASSERT_EQ(4.0, static_cast<Literal<double> *>(n)->value());

  n = analyzeExpression("2.0*2.0");
  ASSERT_EQ(Node::NK_FLOAT, n->nodeKind());
  ASSERT_TRUE(n->type()->isFloatType());
  ASSERT_EQ(4.0, static_cast<Literal<double> *>(n)->value());

  // Divide

  n = analyzeExpression("8/2");
  ASSERT_EQ(Node::NK_INTEGER, n->nodeKind());
  ASSERT_TRUE(n->type()->isIntegerType());
  ASSERT_EQ(4, static_cast<Literal<int> *>(n)->value());

  n = analyzeExpression("8.0/2");
  ASSERT_EQ(Node::NK_FLOAT, n->nodeKind());
  ASSERT_EQ(4.0, static_cast<Literal<double> *>(n)->value());

  n = analyzeExpression("8/2.0");
  ASSERT_EQ(Node::NK_FLOAT, n->nodeKind());
  ASSERT_EQ(4.0, static_cast<Literal<double> *>(n)->value());

  n = analyzeExpression("8.0/2.0");
  ASSERT_EQ(Node::NK_FLOAT, n->nodeKind());
  ASSERT_EQ(4.0, static_cast<Literal<double> *>(n)->value());

  // Modulus

  n = analyzeExpression("8%2");
  ASSERT_EQ(Node::NK_INTEGER, n->nodeKind());
  ASSERT_EQ(0, static_cast<Literal<int> *>(n)->value());

#if 0
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
#endif
}
#endif
}
