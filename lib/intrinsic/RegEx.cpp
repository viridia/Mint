/* ================================================================== *
 * Mint
 * ================================================================== */

#include "mint/eval/Evaluator.h"

#include "mint/intrinsic/Fundamentals.h"
#include "mint/intrinsic/StringRegistry.h"
#include "mint/intrinsic/TypeRegistry.h"

#include "mint/graph/Object.h"
#include "mint/graph/Oper.h"
#include "mint/graph/String.h"

#include "mint/support/Assert.h"
#include "mint/support/Diagnostics.h"
#include "mint/support/Path.h"
#include "mint/support/OStream.h"

#include "re2/re2.h"

namespace mint {

static inline re2::StringPiece toStringPiece(StringRef str) {
  return re2::StringPiece(str.data(), str.size());
}

static Object * getRegExType() {
  static GCPointerRoot<Object> instance = Object::makeDict(NULL, "regex");
  return instance;
}

static Object * getMatchType() {
  static GCPointerRoot<Object> instance = Object::makeDict(NULL, "match");
  return instance;
}

static Type * getStrListType() {
  static Type * type = TypeRegistry::get().getListType(TypeRegistry::stringType());
  return type;
}

static String * getStrGroup() {
  static String * str = StringRegistry::get().str("group");
  return str;
}

/** -------------------------------------------------------------------------
    A regular expression object.
 */
class RegExNode : public Node {
public:
  typedef SmallVector<re2::StringPiece, 8> Captures;
  typedef SmallVector<Node *, 8> Groups;

  struct MatchState {
    re2::StringPiece text;
    Captures captures;
    Groups groups;
    int pos;

    int matchStart() const { return captures[0].begin() - text.begin(); }
    int matchLength() const { return captures[0].length(); }
  };

  RegExNode(Location loc, StringRef pattern)
    : Node(Node::NK_REGEX, loc, getRegExType())
    , _regexp(re2::StringPiece(pattern.data(), pattern.size()))
  {}

  /// The regular expression
  const re2::RE2 & regexp() const { return _regexp; }
  re2::RE2 & regexp() { return _regexp; }

  /// Prepare the match state.
  void initMatchState(MatchState & ms, StringRef text) {
    int numCaptures = _regexp.NumberOfCapturingGroups();
    ms.text = toStringPiece(text);
    ms.captures.resize(numCaptures + 1);
    ms.groups.resize(numCaptures + 1);
    ms.pos = 0;
  }

  /// Match function - returns either a match object or NULL.
  Object * match(Location loc, MatchState & ms) {
    bool matched = _regexp.Match(
        ms.text, ms.pos, ms.text.length(), re2::RE2::UNANCHORED, ms.captures.data(), ms.captures.size());

    if (!matched) {
      return NULL;
    }

    Groups::iterator gi = ms.groups.begin();
    for (Captures::const_iterator it = ms.captures.begin(), itEnd = ms.captures.end(); it != itEnd; ++it) {
      *gi++ = String::create(loc, StringRef(it->data(), it->length()));
    }
    Oper * groups = Oper::create(Node::NK_LIST, loc, getStrListType(), ms.groups);
    Object * match = new Object(Node::NK_OBJECT, loc, getMatchType());
    match->attrs()[getStrGroup()] = groups;
    return match;
  }

private:

  re2::RE2 _regexp;
};

Node * methodRegExFind(Location loc, Evaluator * ex, Function * fn, Node * self, NodeArray args) {
  M_ASSERT(args.size() == 2);
  M_ASSERT(self->nodeKind() == Node::NK_REGEX);
  return NULL;
}

Node * methodRegExSubst(Location loc, Evaluator * ex, Function * fn, Node * self, NodeArray args) {
  M_ASSERT(args.size() == 2);
  M_ASSERT(self->nodeKind() == Node::NK_REGEX);
  return NULL;
}

Node * methodRegExSubstAll(
    Location loc, Evaluator * ex, Function * fn, Node * self, NodeArray args) {
  M_ASSERT(args.size() == 2);
  M_ASSERT(self->nodeKind() == Node::NK_REGEX);
  RegExNode * rn = static_cast<RegExNode *>(self);
  String * textStr = String::cast(args[0]);
  Node * replacement = args[1];
  StringRef text = textStr->value();

  if (replacement->nodeKind() == Node::NK_FUNCTION || replacement->nodeKind() == Node::NK_CLOSURE) {

    RegExNode::MatchState ms;
    SmallString<0> result;
    int numReplacements = 0;
    rn->initMatchState(ms, text);

    for (;;) {
      Object * match = rn->match(loc, ms);
      if (match != NULL) {
        int matchStart = ms.matchStart();
        int matchEnd = matchStart + ms.matchLength();
        Node * args[] = { match };
        String * replacementStr = static_cast<String *>(ex->coerce(
            ex->call(loc, replacement, NULL, args),
            TypeRegistry::stringType()));
        if (replacementStr->nodeKind() != Node::NK_STRING) {
          diag::error(replacement->location()) << "Replacement function should return a string.";
          return &Node::UNDEFINED_NODE;
        }
        result.append(text.substr(ms.pos, matchStart - ms.pos));
        result.append(replacementStr->value());
        ms.pos = matchEnd;
        ++numReplacements;
      } else {
        result.append(text.substr(ms.pos, ms.text.length() - ms.pos));
        if (numReplacements == 0) {
          return textStr;
        } else {
          return String::create(loc, result);
        }
      }
    }
  } else {
    String * replacementStr = String::dyn_cast(ex->coerce(replacement, TypeRegistry::stringType()));
    if (replacementStr == NULL) {
      diag::error(replacement->location()) << "Incorrect type for replacement string: "
          << replacement->type();
      return &Node::UNDEFINED_NODE;
    }

    re2::StringPiece replacementText = toStringPiece(replacementStr->value());
    std::string error;
    if (!rn->regexp().CheckRewriteString(replacementText, &error)) {
      diag::error(replacement->location()) << "Invalid replacement string: " << error;
    }

    std::string newText;
    newText.assign(text.data(), text.size());
    int numReplacements = re2::RE2::GlobalReplace(&newText, rn->regexp(), replacementText);
    if (numReplacements == 0) {
      return textStr;
    } else {
      return String::create(loc, newText);
    }
  }

  return NULL;
}

Node * methodReCompile(Location loc, Evaluator * ex, Function * fn, Node * self, NodeArray args) {
  M_ASSERT(args.size() == 1);
  String * pattern = String::cast(args[0]);
  RegExNode * result = new RegExNode(loc, pattern->value());
  if (!result->regexp().ok()) {
    diag::error(pattern->location()) << StringRef(result->regexp().error());
  }
  return result;
}

void initRegExMethods(Fundamentals * fundamentals) {
  Type * stringType = TypeRegistry::stringType();
  Type * anyType = TypeRegistry::anyType();

  // Regular expression type
  Object * regexType = getRegExType();
  if (regexType->attrs().empty()) {
    regexType->defineMethod("find", stringType, stringType, methodRegExFind);
    regexType->defineMethod("subst", stringType, stringType, anyType, methodRegExSubst);
    regexType->defineMethod("subst_all", stringType, stringType, anyType, methodRegExSubstAll);
  }

  // Match type
  //Object * matchType = getMatchType();

  // Regular expression module
  Object * re = fundamentals->createChildScope("re");
  re->defineMethod("compile", regexType, stringType, methodReCompile);
}

}
