/* ================================================================== *
 * Diagnostics unit test
 * ================================================================== */

#include "gtest/gtest.h"
#include "mint/collections/IntrusiveList.h"

#include <algorithm>

namespace mint {

class TestData : public IntrusiveList<TestData>::Node {
public:
  int index;
};

class TestDataEq {
public:
  TestDataEq(int value) : value_(value) {}

  bool operator()(const TestData & a0) {
    return a0.index == value_;
  }

private:
  int value_;
};

template <class RandomAccessIterator, class StrictWeakOrdering>
void sort(RandomAccessIterator first, RandomAccessIterator last,
          StrictWeakOrdering comp);

class IntrusiveListTest : public testing::Test {
protected:
  typedef IntrusiveList<TestData> TestList;

  TestList tlist;
  TestData node1;
  TestData node2;
  TestData node3;

  void SetUp() {
    node1.index = 1;
    node2.index = 2;
    node3.index = 3;
  }
};

TEST_F(IntrusiveListTest, EmptyListTest) {
  EXPECT_TRUE(tlist.empty());
  EXPECT_TRUE(tlist.begin() == tlist.end());
  EXPECT_FALSE(tlist.begin() != tlist.end());
}

TEST_F(IntrusiveListTest, PushFrontTest) {
  tlist.push_front(node1);
  EXPECT_FALSE(tlist.empty());
  EXPECT_FALSE(tlist.begin() == tlist.end());
  EXPECT_EQ(1, tlist.front().index);
  EXPECT_EQ(1, tlist.back().index);

  tlist.push_front(node2);
  EXPECT_FALSE(tlist.empty());
  EXPECT_FALSE(tlist.begin() == tlist.end());
  EXPECT_EQ(2, tlist.front().index);
  EXPECT_EQ(1, tlist.back().index);

  tlist.pop_front();
  EXPECT_FALSE(tlist.empty());
  EXPECT_FALSE(tlist.begin() == tlist.end());
  EXPECT_EQ(1, tlist.front().index);
  EXPECT_EQ(1, tlist.back().index);

  tlist.pop_front();
  EXPECT_TRUE(tlist.empty());
  EXPECT_TRUE(tlist.begin() == tlist.end());
}

TEST_F(IntrusiveListTest, PushBackTest) {
  tlist.push_back(node1);
  EXPECT_FALSE(tlist.empty());
  EXPECT_FALSE(tlist.begin() == tlist.end());
  EXPECT_EQ(1, tlist.front().index);
  EXPECT_EQ(1, tlist.back().index);

  tlist.push_back(node2);
  EXPECT_EQ(1, tlist.front().index);
  EXPECT_EQ(2, tlist.back().index);

  tlist.pop_back();
  EXPECT_FALSE(tlist.empty());
  EXPECT_FALSE(tlist.begin() == tlist.end());
  EXPECT_EQ(1, tlist.front().index);
  EXPECT_EQ(1, tlist.back().index);

  tlist.pop_back();
  EXPECT_TRUE(tlist.empty());
  EXPECT_TRUE(tlist.begin() == tlist.end());
}

TEST_F(IntrusiveListTest, IteratorTest) {
  tlist.push_back(node1);
  tlist.push_back(node2);
  tlist.push_back(node3);

  TestList::iterator it = tlist.begin();
  EXPECT_EQ(1, it->index);
  EXPECT_EQ(1, (*it).index);

  ++it;
  EXPECT_EQ(2, it->index);
  EXPECT_EQ(2, (*it).index);

  it++;
  EXPECT_EQ(3, it->index);
  EXPECT_EQ(3, (*it).index);

  --it;
  EXPECT_EQ(2, it->index);
  EXPECT_EQ(2, (*it).index);

  it--;
  EXPECT_EQ(1, it->index);
  EXPECT_EQ(1, (*it).index);

  it = tlist.begin();
  EXPECT_EQ(1, (*it++).index);

  it = tlist.begin();
  EXPECT_EQ(2, (*++it).index);

  it = tlist.end();
  EXPECT_EQ(3, (*--it).index);
  EXPECT_EQ(3, (*it--).index);
}

TEST_F(IntrusiveListTest, ConstIteratorTest) {
  tlist.push_back(node1);
  tlist.push_back(node2);
  tlist.push_back(node3);

  TestList::const_iterator it = tlist.begin();
  EXPECT_EQ(1, it->index);
  EXPECT_EQ(1, (*it).index);

  ++it;
  EXPECT_EQ(2, it->index);
  EXPECT_EQ(2, (*it).index);

  it++;
  EXPECT_EQ(3, it->index);
  EXPECT_EQ(3, (*it).index);

  --it;
  EXPECT_EQ(2, it->index);
  EXPECT_EQ(2, (*it).index);

  it--;
  EXPECT_EQ(1, it->index);
  EXPECT_EQ(1, (*it).index);

  it = tlist.begin();
  EXPECT_EQ(1, (*it++).index);

  it = tlist.begin();
  EXPECT_EQ(2, (*++it).index);

  it = tlist.end();
  EXPECT_EQ(3, (*--it).index);
  EXPECT_EQ(3, (*it--).index);
}

TEST_F(IntrusiveListTest, InsertTest) {
  TestList::iterator it = tlist.begin();
  tlist.insert(it, node1);
  EXPECT_EQ(1, tlist.front().index);
  EXPECT_EQ(1, tlist.back().index);

  it = tlist.end();
  tlist.insert(it, node2);
  EXPECT_EQ(1, tlist.front().index);
  EXPECT_EQ(2, tlist.back().index);
}

TEST_F(IntrusiveListTest, RemoveTest) {
  tlist.push_back(node1);
  tlist.push_back(node2);
  tlist.push_back(node3);

  node1.remove();
  node3.remove();

  EXPECT_EQ(2, tlist.front().index);
  EXPECT_EQ(2, tlist.back().index);
}

TEST_F(IntrusiveListTest, EraseTest) {
  tlist.push_back(node1);
  tlist.push_back(node2);
  tlist.push_back(node3);

  TestList::iterator it;
  it = tlist.erase(tlist.begin());
  EXPECT_EQ(2, it->index);

  it = tlist.erase(++it);
  EXPECT_TRUE(it == tlist.end());

  EXPECT_EQ(2, tlist.front().index);
  EXPECT_EQ(2, tlist.back().index);
}

TEST_F(IntrusiveListTest, FindTest) {
  // See if we can use standard STL algorithms on intrusive list.
  tlist.push_back(node2);
  tlist.push_back(node1);
  tlist.push_back(node3);

  // STL 'find_if' algorithm.
  EXPECT_TRUE(tlist.end() == std::find_if(tlist.begin(), tlist.end(), TestDataEq(0)));
  EXPECT_TRUE(tlist.begin() == std::find_if(tlist.begin(), tlist.end(), TestDataEq(2)));

#if 0
  // STL 'count_if' algorithm
  EXPECT_EQ(0, std::count_if(tlist.begin(), tlist.end(), TestDataEq(0)));
  EXPECT_EQ(1, std::count_if(tlist.begin(), tlist.end(), TestDataEq(1)));
  EXPECT_EQ(1, std::count_if(tlist.begin(), tlist.end(), TestDataEq(2)));
  EXPECT_EQ(1, std::count_if(tlist.begin(), tlist.end(), TestDataEq(3)));
#endif
}

} // namespace mint
