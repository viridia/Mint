/* ================================================================== *
 * StringDict unit test
 * ================================================================== */

#include "gtest/gtest.h"
#include "mint/collections/SmallVector.h"
#include "mint/graph/StringDict.h"
#include "mint/graph/Literal.h"
#include "TestHelpers.h"

#include <algorithm>
#include <string.h>

namespace mint {

class StringDictTest : public testing::Test {
public:
  Literal<int> * makeInt(int value) {
    return new Literal<int>(Node::NK_INTEGER, Location(), NULL, value);
  }
};

TEST_F(StringDictTest, Empty) {
  StringDict<Node> st;
  ASSERT_TRUE(st.empty());
  ASSERT_EQ(0u, st.size());
  ASSERT_TRUE(st.begin() == st.end());
  ASSERT_FALSE(st.begin() != st.end());
}

TEST_F(StringDictTest, Insert) {
  StringDict<Literal<int > > st;
  st[String::create("Hello")] = makeInt(1);
  ASSERT_FALSE(st.empty());
  ASSERT_EQ(1u, st.size());
  EXPECT_FALSE(st.begin() == st.end());
  EXPECT_TRUE(st.begin() != st.end());

  StringDict<Literal<int> >::iterator it = st.find(String::create("Hello"));
  ASSERT_FALSE(it == st.end());
  ASSERT_EQ("Hello", (*it).first->value());
  ASSERT_EQ("Hello", it->first->value());
  ASSERT_EQ(1, (*it).second->value());
  ASSERT_EQ(1, it->second->value());

  it = st.find(String::create("Goodbye"));
  ASSERT_TRUE(it == st.end());
}

TEST_F(StringDictTest, InsertTwoKeys) {
  StringDict<Node> st;
  st[String::create("Hello")] = makeInt(1);
  st[String::create("Goodbye")] = makeInt(1);
  ASSERT_FALSE(st.empty());
  ASSERT_EQ(2u, st.size());
  ASSERT_FALSE(st.begin() == st.end());
  ASSERT_TRUE(st.begin() != st.end());
}

TEST_F(StringDictTest, InsertSameKeyTwice) {
  StringDict<Node> st;
  st[String::create("Hello")] = makeInt(1);
  st[String::create("Hello")] = makeInt(1);
  ASSERT_EQ(1u, st.size());
}

TEST_F(StringDictTest, Iterate) {
  StringDict<Node> st;
  st[String::create("Hello")] = makeInt(1);
  st[String::create("Goodbye")] = makeInt(1);
  unsigned count = 0;
  for (StringDict<Node>::iterator it = st.begin(); it != st.end(); ++it) {
    count++;
  }
  ASSERT_EQ(2u, count);

  count = 0;
  for (StringDict<Node>::iterator it = st.begin(); it != st.end(); it++) {
    count++;
  }
  ASSERT_EQ(2u, count);

  count = 0;
  for (StringDict<Node>::const_iterator it = st.begin(); it != st.end(); ++it) {
    count++;
  }
  ASSERT_EQ(2u, count);

  count = 0;
  for (StringDict<Node>::const_iterator it = st.begin(); it != st.end(); it++) {
    count++;
  }
  ASSERT_EQ(2u, count);
}

TEST_F(StringDictTest, InsertLots) {
  StringDict<Literal<int> > st;
  // Insert strings representing the hexidecimal values for the numbers 0..999
  int words[1000];
  for (int i = 0; i < 1000; ++i) {
    char word[16];
    size_t length = ::snprintf(word, sizeof(word), "%x", i);
    ASSERT_LE(int(length), 4);
    String * key = String::create(StringRef(word, length));
    ASSERT_EQ(Node::NK_STRING, key->nodeKind());
    st[key] = makeInt(i);
    words[i] = 0;
    for (StringDict<Literal<int> >::const_iterator it = st.begin(); it != st.end(); ++it) {
      if (it->first->size() >= 4) {
        FAIL() << i << " - after insertion of " << word << " key of " << it->first
            << " is invalid.";
      }
    }
  }

  // Now iterate through all keys and convert back to integer form.
  unsigned count = 0;
  for (StringDict<Literal<int> >::const_iterator it = st.begin(); it != st.end(); ++it, ++count) {
    SmallVector<char, 16> buf;
    buf.insert(buf.begin(), it->first->begin(), it->first->end());
    buf.push_back(0);
    long index = ::strtol(buf.data(), NULL, 16);
    if (words[index] != 0) {
      FAIL() << "Already visited entry: " << index << " with text: " << buf.data()
          << " in loop iteration " << count;
    }
    ASSERT_EQ(index, it->second->value());
    words[index]++;
  }

  // Make sure that every number was visited.
  for (int i = 0; i < 1000; ++i) {
    if (words[i] != 1) {
      FAIL() << "Entry: " << i << " has incorrect value: " << words[i];
    }
  }
}

}
