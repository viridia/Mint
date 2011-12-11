/* ================================================================== *
 * Diagnostics unit test
 * ================================================================== */

#include "gtest/gtest.h"

#include "mint/eval/Evaluator.h"

#include "mint/graph/GraphBuilder.h"
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
  Module module;
  OStrStream errorStrm;
  Fundamentals * fundamentals;

  EvaluatorTest() : module("", NULL) {
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
    Node * result = ev.eval(n, NULL);
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

TEST_F(EvaluatorTest, ArithmeticOperators) {
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
#endif
}

TEST_F(EvaluatorTest, ComparisonOperators) {
  Node * n;

  // Equal

  n = evalExpression("1==1");
  ASSERT_EQ(Node::NK_BOOL, n->nodeKind());
  EXPECT_NODE_EQ("true", n);

  n = evalExpression("1==2");
  ASSERT_EQ(Node::NK_BOOL, n->nodeKind());
  EXPECT_NODE_EQ("false", n);

  n = evalExpression("1==1.0");
  ASSERT_EQ(Node::NK_BOOL, n->nodeKind());
  EXPECT_NODE_EQ("true", n);

  n = evalExpression("1==2.0");
  ASSERT_EQ(Node::NK_BOOL, n->nodeKind());
  EXPECT_NODE_EQ("false", n);

  n = evalExpression("1.0==1");
  ASSERT_EQ(Node::NK_BOOL, n->nodeKind());
  EXPECT_NODE_EQ("true", n);

  n = evalExpression("1.0==2");
  ASSERT_EQ(Node::NK_BOOL, n->nodeKind());
  EXPECT_NODE_EQ("false", n);

  n = evalExpression("1.0==1.0");
  ASSERT_EQ(Node::NK_BOOL, n->nodeKind());
  EXPECT_NODE_EQ("true", n);

  n = evalExpression("1.0==2.0");
  ASSERT_EQ(Node::NK_BOOL, n->nodeKind());
  EXPECT_NODE_EQ("false", n);

  n = evalExpression("true==true");
  ASSERT_EQ(Node::NK_BOOL, n->nodeKind());
  EXPECT_NODE_EQ("true", n);

  n = evalExpression("true==false");
  ASSERT_EQ(Node::NK_BOOL, n->nodeKind());
  EXPECT_NODE_EQ("false", n);

  n = evalExpression("'a'=='a'");
  ASSERT_EQ(Node::NK_BOOL, n->nodeKind());
  EXPECT_NODE_EQ("true", n);

  n = evalExpression("'a'=='b'");
  ASSERT_EQ(Node::NK_BOOL, n->nodeKind());
  EXPECT_NODE_EQ("false", n);

  n = evalExpression("[0]==[0]");
  ASSERT_EQ(Node::NK_BOOL, n->nodeKind());
  EXPECT_NODE_EQ("true", n);

  n = evalExpression("[0]==[1]");
  ASSERT_EQ(Node::NK_BOOL, n->nodeKind());
  EXPECT_NODE_EQ("false", n);

  // Not equal (uses same code path as Equal, so no need to run through all permutations.)

  n = evalExpression("1!=1");
  ASSERT_EQ(Node::NK_BOOL, n->nodeKind());
  EXPECT_NODE_EQ("false", n);

  n = evalExpression("1!=2");
  ASSERT_EQ(Node::NK_BOOL, n->nodeKind());
  EXPECT_NODE_EQ("true", n);

  // Less

  n = evalExpression("1<0");
  ASSERT_EQ(Node::NK_BOOL, n->nodeKind());
  EXPECT_NODE_EQ("false", n);

  n = evalExpression("1<1");
  ASSERT_EQ(Node::NK_BOOL, n->nodeKind());
  EXPECT_NODE_EQ("false", n);

  n = evalExpression("1<2");
  ASSERT_EQ(Node::NK_BOOL, n->nodeKind());
  EXPECT_NODE_EQ("true", n);

  n = evalExpression("1<0.0");
  ASSERT_EQ(Node::NK_BOOL, n->nodeKind());
  EXPECT_NODE_EQ("false", n);

  n = evalExpression("1<1.0");
  ASSERT_EQ(Node::NK_BOOL, n->nodeKind());
  EXPECT_NODE_EQ("false", n);

  n = evalExpression("1<2.0");
  ASSERT_EQ(Node::NK_BOOL, n->nodeKind());
  EXPECT_NODE_EQ("true", n);

  n = evalExpression("1.0<0");
  ASSERT_EQ(Node::NK_BOOL, n->nodeKind());
  EXPECT_NODE_EQ("false", n);

  n = evalExpression("1.0<1");
  ASSERT_EQ(Node::NK_BOOL, n->nodeKind());
  EXPECT_NODE_EQ("false", n);

  n = evalExpression("1.0<2");
  ASSERT_EQ(Node::NK_BOOL, n->nodeKind());
  EXPECT_NODE_EQ("true", n);

  n = evalExpression("1.0<0.0");
  ASSERT_EQ(Node::NK_BOOL, n->nodeKind());
  EXPECT_NODE_EQ("false", n);

  n = evalExpression("1.0<1.0");
  ASSERT_EQ(Node::NK_BOOL, n->nodeKind());
  EXPECT_NODE_EQ("false", n);

  n = evalExpression("1.0<2.0");
  ASSERT_EQ(Node::NK_BOOL, n->nodeKind());
  EXPECT_NODE_EQ("true", n);

  n = evalExpression("true<true");
  ASSERT_EQ(Node::NK_BOOL, n->nodeKind());
  EXPECT_NODE_EQ("false", n);

  n = evalExpression("false<true");
  ASSERT_EQ(Node::NK_BOOL, n->nodeKind());
  EXPECT_NODE_EQ("true", n);

  n = evalExpression("'1'<'0'");
  ASSERT_EQ(Node::NK_BOOL, n->nodeKind());
  EXPECT_NODE_EQ("false", n);

  n = evalExpression("'1'<'1'");
  ASSERT_EQ(Node::NK_BOOL, n->nodeKind());
  EXPECT_NODE_EQ("false", n);

  n = evalExpression("'11'<'1'");
  ASSERT_EQ(Node::NK_BOOL, n->nodeKind());
  EXPECT_NODE_EQ("false", n);

  n = evalExpression("'1'<'11'");
  ASSERT_EQ(Node::NK_BOOL, n->nodeKind());
  EXPECT_NODE_EQ("true", n);

  n = evalExpression("'1'<'2'");
  ASSERT_EQ(Node::NK_BOOL, n->nodeKind());
  EXPECT_NODE_EQ("true", n);

  // Greater

  n = evalExpression("1>0");
  ASSERT_EQ(Node::NK_BOOL, n->nodeKind());
  EXPECT_NODE_EQ("true", n);

  n = evalExpression("1>1");
  ASSERT_EQ(Node::NK_BOOL, n->nodeKind());
  EXPECT_NODE_EQ("false", n);

  n = evalExpression("1>2");
  ASSERT_EQ(Node::NK_BOOL, n->nodeKind());
  EXPECT_NODE_EQ("false", n);

  n = evalExpression("1>0.0");
  ASSERT_EQ(Node::NK_BOOL, n->nodeKind());
  EXPECT_NODE_EQ("true", n);

  n = evalExpression("1>1.0");
  ASSERT_EQ(Node::NK_BOOL, n->nodeKind());
  EXPECT_NODE_EQ("false", n);

  n = evalExpression("1>2.0");
  ASSERT_EQ(Node::NK_BOOL, n->nodeKind());
  EXPECT_NODE_EQ("false", n);

  n = evalExpression("1.0>0");
  ASSERT_EQ(Node::NK_BOOL, n->nodeKind());
  EXPECT_NODE_EQ("true", n);

  n = evalExpression("1.0>1");
  ASSERT_EQ(Node::NK_BOOL, n->nodeKind());
  EXPECT_NODE_EQ("false", n);

  n = evalExpression("1.0>2");
  ASSERT_EQ(Node::NK_BOOL, n->nodeKind());
  EXPECT_NODE_EQ("false", n);

  n = evalExpression("1.0>0.0");
  ASSERT_EQ(Node::NK_BOOL, n->nodeKind());
  EXPECT_NODE_EQ("true", n);

  n = evalExpression("1.0>1.0");
  ASSERT_EQ(Node::NK_BOOL, n->nodeKind());
  EXPECT_NODE_EQ("false", n);

  n = evalExpression("1.0>2.0");
  ASSERT_EQ(Node::NK_BOOL, n->nodeKind());
  EXPECT_NODE_EQ("false", n);

  n = evalExpression("true>true");
  ASSERT_EQ(Node::NK_BOOL, n->nodeKind());
  EXPECT_NODE_EQ("false", n);

  n = evalExpression("false>true");
  ASSERT_EQ(Node::NK_BOOL, n->nodeKind());
  EXPECT_NODE_EQ("false", n);

  n = evalExpression("'1'>'0'");
  ASSERT_EQ(Node::NK_BOOL, n->nodeKind());
  EXPECT_NODE_EQ("true", n);

  n = evalExpression("'1'>'1'");
  ASSERT_EQ(Node::NK_BOOL, n->nodeKind());
  EXPECT_NODE_EQ("false", n);

  n = evalExpression("'11'>'1'");
  ASSERT_EQ(Node::NK_BOOL, n->nodeKind());
  EXPECT_NODE_EQ("true", n);

  n = evalExpression("'1'>'11'");
  ASSERT_EQ(Node::NK_BOOL, n->nodeKind());
  EXPECT_NODE_EQ("false", n);

  n = evalExpression("'1'>'2'");
  ASSERT_EQ(Node::NK_BOOL, n->nodeKind());
  EXPECT_NODE_EQ("false", n);

  // LessOrEqual

  n = evalExpression("1<=0");
  ASSERT_EQ(Node::NK_BOOL, n->nodeKind());
  EXPECT_NODE_EQ("false", n);

  n = evalExpression("1<=1");
  ASSERT_EQ(Node::NK_BOOL, n->nodeKind());
  EXPECT_NODE_EQ("true", n);

  n = evalExpression("1<=2");
  ASSERT_EQ(Node::NK_BOOL, n->nodeKind());
  EXPECT_NODE_EQ("true", n);

  n = evalExpression("1<=0.0");
  ASSERT_EQ(Node::NK_BOOL, n->nodeKind());
  EXPECT_NODE_EQ("false", n);

  n = evalExpression("1<=1.0");
  ASSERT_EQ(Node::NK_BOOL, n->nodeKind());
  EXPECT_NODE_EQ("true", n);

  n = evalExpression("1<=2.0");
  ASSERT_EQ(Node::NK_BOOL, n->nodeKind());
  EXPECT_NODE_EQ("true", n);

  n = evalExpression("1.0<=0");
  ASSERT_EQ(Node::NK_BOOL, n->nodeKind());
  EXPECT_NODE_EQ("false", n);

  n = evalExpression("1.0<=1");
  ASSERT_EQ(Node::NK_BOOL, n->nodeKind());
  EXPECT_NODE_EQ("true", n);

  n = evalExpression("1.0<=2");
  ASSERT_EQ(Node::NK_BOOL, n->nodeKind());
  EXPECT_NODE_EQ("true", n);

  n = evalExpression("1.0<=0.0");
  ASSERT_EQ(Node::NK_BOOL, n->nodeKind());
  EXPECT_NODE_EQ("false", n);

  n = evalExpression("1.0<=1.0");
  ASSERT_EQ(Node::NK_BOOL, n->nodeKind());
  EXPECT_NODE_EQ("true", n);

  n = evalExpression("1.0<=2.0");
  ASSERT_EQ(Node::NK_BOOL, n->nodeKind());
  EXPECT_NODE_EQ("true", n);

  n = evalExpression("true<=true");
  ASSERT_EQ(Node::NK_BOOL, n->nodeKind());
  EXPECT_NODE_EQ("true", n);

  n = evalExpression("false<=true");
  ASSERT_EQ(Node::NK_BOOL, n->nodeKind());
  EXPECT_NODE_EQ("true", n);

  n = evalExpression("'1'<='0'");
  ASSERT_EQ(Node::NK_BOOL, n->nodeKind());
  EXPECT_NODE_EQ("false", n);

  n = evalExpression("'1'<='1'");
  ASSERT_EQ(Node::NK_BOOL, n->nodeKind());
  EXPECT_NODE_EQ("true", n);

  n = evalExpression("'11'<='1'");
  ASSERT_EQ(Node::NK_BOOL, n->nodeKind());
  EXPECT_NODE_EQ("false", n);

  n = evalExpression("'1'<='11'");
  ASSERT_EQ(Node::NK_BOOL, n->nodeKind());
  EXPECT_NODE_EQ("true", n);

  n = evalExpression("'1'<='2'");
  ASSERT_EQ(Node::NK_BOOL, n->nodeKind());
  EXPECT_NODE_EQ("true", n);

  // GreaterOrEqual

  n = evalExpression("1>=0");
  ASSERT_EQ(Node::NK_BOOL, n->nodeKind());
  EXPECT_NODE_EQ("true", n);

  n = evalExpression("1>=1");
  ASSERT_EQ(Node::NK_BOOL, n->nodeKind());
  EXPECT_NODE_EQ("true", n);

  n = evalExpression("1>=2");
  ASSERT_EQ(Node::NK_BOOL, n->nodeKind());
  EXPECT_NODE_EQ("false", n);

  n = evalExpression("1>=0.0");
  ASSERT_EQ(Node::NK_BOOL, n->nodeKind());
  EXPECT_NODE_EQ("true", n);

  n = evalExpression("1>=1.0");
  ASSERT_EQ(Node::NK_BOOL, n->nodeKind());
  EXPECT_NODE_EQ("true", n);

  n = evalExpression("1>=2.0");
  ASSERT_EQ(Node::NK_BOOL, n->nodeKind());
  EXPECT_NODE_EQ("false", n);

  n = evalExpression("1.0>=0");
  ASSERT_EQ(Node::NK_BOOL, n->nodeKind());
  EXPECT_NODE_EQ("true", n);

  n = evalExpression("1.0>=1");
  ASSERT_EQ(Node::NK_BOOL, n->nodeKind());
  EXPECT_NODE_EQ("true", n);

  n = evalExpression("1.0>=2");
  ASSERT_EQ(Node::NK_BOOL, n->nodeKind());
  EXPECT_NODE_EQ("false", n);

  n = evalExpression("1.0>=0.0");
  ASSERT_EQ(Node::NK_BOOL, n->nodeKind());
  EXPECT_NODE_EQ("true", n);

  n = evalExpression("1.0>=1.0");
  ASSERT_EQ(Node::NK_BOOL, n->nodeKind());
  EXPECT_NODE_EQ("true", n);

  n = evalExpression("1.0>=2.0");
  ASSERT_EQ(Node::NK_BOOL, n->nodeKind());
  EXPECT_NODE_EQ("false", n);

  n = evalExpression("true>=true");
  ASSERT_EQ(Node::NK_BOOL, n->nodeKind());
  EXPECT_NODE_EQ("true", n);

  n = evalExpression("false>=true");
  ASSERT_EQ(Node::NK_BOOL, n->nodeKind());
  EXPECT_NODE_EQ("false", n);

  n = evalExpression("'1'>='0'");
  ASSERT_EQ(Node::NK_BOOL, n->nodeKind());
  EXPECT_NODE_EQ("true", n);

  n = evalExpression("'1'>='1'");
  ASSERT_EQ(Node::NK_BOOL, n->nodeKind());
  EXPECT_NODE_EQ("true", n);

  n = evalExpression("'11'>='1'");
  ASSERT_EQ(Node::NK_BOOL, n->nodeKind());
  EXPECT_NODE_EQ("true", n);

  n = evalExpression("'1'>='11'");
  ASSERT_EQ(Node::NK_BOOL, n->nodeKind());
  EXPECT_NODE_EQ("false", n);

  n = evalExpression("'1'>='2'");
  ASSERT_EQ(Node::NK_BOOL, n->nodeKind());
  EXPECT_NODE_EQ("false", n);
}

Node * methodIdentity(Location loc, Evaluator * ex, Function * fn, Node * self, NodeArray args) {
  return args[0];
}

TEST_F(EvaluatorTest, ArgumentCoercion) {
  Node * n;

  // Putting the 'fun' in fundamentals:
  // Add some functions of various types that simply return their inputs.
  GraphBuilder builder;

  // str_identity()
  fundamentals->attrs()[fundamentals->str("str_identity")] =
      builder.createFunction(Location(),
          TypeRegistry::stringType(), TypeRegistry::stringType(), methodIdentity);

  // int_identity()
  fundamentals->attrs()[fundamentals->str("int_identity")] =
      builder.createFunction(Location(),
          TypeRegistry::integerType(), TypeRegistry::integerType(), methodIdentity);

  // float_identity()
  fundamentals->attrs()[fundamentals->str("float_identity")] =
      builder.createFunction(Location(),
          TypeRegistry::floatType(), TypeRegistry::floatType(), methodIdentity);

  // strlist_identity()
  Type * strListType = TypeRegistry::get().getListType(TypeRegistry::stringType());
  fundamentals->attrs()[fundamentals->str("strlist_identity")] =
      builder.createFunction(Location(), strListType, strListType, methodIdentity);

  // String

  // str -> str
  n = evalExpression("str_identity('test')");
  ASSERT_EQ(Node::NK_STRING, n->nodeKind());
  EXPECT_NODE_EQ("'test'", n);

  // bool -> str
  n = evalExpression("str_identity(true)");
  ASSERT_EQ(Node::NK_STRING, n->nodeKind());
  EXPECT_NODE_EQ("'true'", n);

  // int -> str
  n = evalExpression("str_identity(1)");
  ASSERT_EQ(Node::NK_STRING, n->nodeKind());
  EXPECT_NODE_EQ("'1'", n);

  // float -> str
  n = evalExpression("str_identity(1.0)");
  ASSERT_EQ(Node::NK_STRING, n->nodeKind());
  //EXPECT_NODE_EQ("'1.0'", n); // silly floats are never exact

  // undefined -> str
  n = evalExpression("str_identity(undefined)");
  ASSERT_EQ(Node::NK_UNDEFINED, n->nodeKind());

  // Integer

  // int -> int
  n = evalExpression("int_identity(1)");
  ASSERT_EQ(Node::NK_INTEGER, n->nodeKind());
  EXPECT_NODE_EQ("1", n);

  // float -> int
  n = evalExpression("int_identity(1.0)");
  ASSERT_EQ(Node::NK_INTEGER, n->nodeKind());
  //EXPECT_NODE_EQ("'1.0'", n); // silly floats are never exact

  // Float

  // int -> float
  n = evalExpression("float_identity(1)");
  ASSERT_EQ(Node::NK_FLOAT, n->nodeKind());
  //EXPECT_NODE_EQ("1", n);

  // float -> float
  n = evalExpression("float_identity(1.0)");
  ASSERT_EQ(Node::NK_FLOAT, n->nodeKind());
  //EXPECT_NODE_EQ("'1.0'", n); // silly floats are never exact

  // String list

  // list[str] -> list[str]
  n = evalExpression("strlist_identity(['test'])");
  ASSERT_EQ(Node::NK_LIST, n->nodeKind());
  EXPECT_NODE_EQ("LIST('test')", n);

  // list[bool] -> list[str]
  n = evalExpression("strlist_identity([true])");
  ASSERT_EQ(Node::NK_LIST, n->nodeKind());
  EXPECT_NODE_EQ("LIST('true')", n);

  // list[int] -> list[str]
  n = evalExpression("strlist_identity([1])");
  ASSERT_EQ(Node::NK_LIST, n->nodeKind());
  EXPECT_NODE_EQ("LIST('1')", n);

  // list[float] -> list[str]
  n = evalExpression("strlist_identity([1.0])");
  ASSERT_EQ(Node::NK_LIST, n->nodeKind());
  //EXPECT_NODE_EQ("'1.0'", n); // silly floats are never exact
}

}
