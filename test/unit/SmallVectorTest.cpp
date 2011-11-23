/* ================================================================== *
 * StringRef unit test
 * ================================================================== */

#include "gtest/gtest.h"
#include "mint/collections/SmallVector.h"
#include "mint/collections/StringRef.h"

#include <algorithm>

namespace mint {

void appendString(SmallVectorImpl<char> & svi, StringRef sr) {
  svi.append(sr.begin(), sr.end());
}

StringRef asString(SmallVectorImpl<char> & svi) {
  return StringRef(svi.data(), svi.size());
}

TEST(SmallVectorTest, EmptyVector) {
  SmallVector<char, 16> sv;
  ASSERT_TRUE(sv.empty());
  ASSERT_EQ(0u, sv.size());
  ASSERT_TRUE(sv.begin() == sv.end());
  ASSERT_EQ(16u, sv.capacity());
}

TEST(SmallVectorTest, EmptySize0Vector) {
  SmallVector<char, 0> sv;
  ASSERT_TRUE(sv.empty());
  ASSERT_EQ(0u, sv.size());
  ASSERT_TRUE(sv.begin() == sv.end());
  ASSERT_EQ(0u, sv.capacity());
}

TEST(SmallVectorTest, PushBack) {
  SmallVector<char, 16> sv;
  sv.push_back('a');
  ASSERT_FALSE(sv.empty());
  ASSERT_EQ(1u, sv.size());
  ASSERT_FALSE(sv.begin() == sv.end());
  ASSERT_EQ(16u, sv.capacity());
  ASSERT_EQ('a', sv.front());
  ASSERT_EQ('a', sv.back());
  ASSERT_EQ('a', sv[0]);
}

TEST(SmallVectorTest, PushBackSize0Vector) {
  SmallVector<char, 16> sv;
  sv.push_back('a');
  ASSERT_FALSE(sv.empty());
  ASSERT_EQ(1u, sv.size());
  ASSERT_FALSE(sv.begin() == sv.end());
  ASSERT_TRUE(sv.capacity() > 0);
  ASSERT_EQ('a', sv.front());
  ASSERT_EQ('a', sv.back());
  ASSERT_EQ('a', sv[0]);
  ASSERT_EQ('a', *sv.begin());
}

TEST(SmallVectorTest, Clear) {
  SmallVector<char, 16> sv;
  sv.push_back('a');
  sv.clear();
  ASSERT_TRUE(sv.empty());
  ASSERT_EQ(0u, sv.size());
  ASSERT_TRUE(sv.begin() == sv.end());
}

TEST(SmallVectorTest, PopBack) {
  SmallVector<char, 16> sv;
  sv.push_back('a');
  sv.pop_back();
  ASSERT_TRUE(sv.empty());
  ASSERT_EQ(0u, sv.size());
  ASSERT_TRUE(sv.begin() == sv.end());
}

TEST(SmallVectorTest, ResizeUpward) {
  SmallVector<char, 16> sv;
  sv.resize(32);
  ASSERT_FALSE(sv.empty());
  ASSERT_EQ(32u, sv.size());
  ASSERT_EQ(0, sv.front());
  ASSERT_EQ(0, sv.back());
}

TEST(SmallVectorTest, ResizeUpwardFill) {
  SmallVector<char, 16> sv;
  sv.resize(32, 'a');
  ASSERT_FALSE(sv.empty());
  ASSERT_EQ(32u, sv.size());
  ASSERT_EQ('a', sv.front());
  ASSERT_EQ('a', sv.back());
}

TEST(SmallVectorTest, ReserveUpward) {
  SmallVector<char, 16> sv;
  sv.reserve(32);
  ASSERT_EQ(33u, sv.capacity());
}

TEST(SmallVectorTest, ReserveDownward) {
  SmallVector<char, 16> sv;
  sv.resize(32);
  sv.clear();
  sv.reserve(0);
  ASSERT_EQ(33u, sv.capacity());
}

TEST(SmallVectorTest, AppendIterPair) {
  SmallVector<char, 16> sv;
  appendString(sv, "abc");
  ASSERT_EQ("abc", asString(sv));
}

TEST(SmallVectorTest, AppendRepeated) {
  SmallVector<char, 16> sv;
  sv.append(3, 'a');
  sv.append(3, 'b');
  ASSERT_EQ("aaabbb", asString(sv));
}

TEST(SmallVectorTest, AssignRepeated) {
  SmallVector<char, 16> sv;
  sv.assign(3, 'a');
  sv.assign(3, 'b');
  ASSERT_EQ("bbb", asString(sv));
}

TEST(SmallVectorTest, EraseOne) {
  SmallVector<char, 16> sv;
  appendString(sv, "abc");
  sv.erase(sv.begin() + 1);
  ASSERT_EQ("ac", asString(sv));
}

TEST(SmallVectorTest, EraseRange) {
  SmallVector<char, 16> sv;
  appendString(sv, "abcde");
  sv.erase(sv.begin() + 1, sv.end() - 1);
  ASSERT_EQ("ae", asString(sv));
}

TEST(SmallVectorTest, InsertOneEmpty) {
  SmallVector<char, 16> sv;
  sv.insert(sv.begin(), 'b');
  ASSERT_EQ("b", asString(sv));
}

TEST(SmallVectorTest, InsertOne) {
  SmallVector<char, 16> sv;
  appendString(sv, "ac");
  sv.insert(sv.begin() + 1, 'b');
  ASSERT_EQ("abc", asString(sv));
}

#if 0

iterator insert(iterator insertPos, size_type count, const value_type & value) {
  if (insertPos == _end) {  // Important special case for empty vector.
    append(count, value);
    return _end - 1;
  }

  // Convert iterator to elt# to avoid invalidating iterator when we reserve()
  size_t offset = insertPos - _begin;

  // Ensure there is enough space.
  reserve(static_cast<unsigned>(this->size() + count));

  // Uninvalidate the iterator.
  insertPos = _begin + offset;

  // If there are more elements between the insertion point and the end of the
  // range than there are being inserted, we can use a simple approach to
  // insertion.  Since we already reserved space, we know that this won't
  // reallocate the vector.
  if (size_t(_end - insertPos) >= count) {
    T * oldEnd = _end;
    append(_end - count, _end);

    // Copy the existing elements that get replaced.
    std::copy_backward(insertPos, oldEnd-count, oldEnd);

    std::fill_n(insertPos, count, value);
    return insertPos;
  }

  // Otherwise, we're inserting more elements than exist already, and we're
  // not inserting at the end.

  // Copy over the elements that we're about to overwrite.
  T * oldEnd = _end;
  _end += count;
  size_t numOverwritten = oldEnd - insertPos;
  std::uninitialized_copy(insertPos, oldEnd, _end - numOverwritten);

  // Replace the overwritten part.
  std::fill_n(insertPos, numOverwritten, value);

  // Insert the non-overwritten middle part.
  std::uninitialized_fill_n(oldEnd, count-numOverwritten, value);
  return insertPos;
}

template<typename InputIterTy>
iterator insert(iterator insertPos, InputIterTy first, InputIterTy last) {
  if (insertPos == _end) {  // Important special case for empty vector.
    append(first, last);
    return _end - 1;
  }

  size_t count = std::distance(first, last);
  // Convert iterator to elt# to avoid invalidating iterator when we reserve()
  size_t offset = insertPos - _begin;

  // Ensure there is enough space.
  reserve(static_cast<unsigned>(this->size() + count));

  // Uninvalidate the iterator.
  insertPos = _begin + offset;

  // If there are more elements between the insertion point and the end of the
  // range than there are being inserted, we can use a simple approach to
  // insertion.  Since we already reserved space, we know that this won't
  // reallocate the vector.
  if (size_t(_end - insertPos) >= count) {
    T *oldEnd = _end;
    append(_end - count, _end);

    // Copy the existing elements that get replaced.
    std::copy_backward(insertPos, oldEnd-count, oldEnd);

    std::copy(first, last, insertPos);
    return insertPos;
  }

  // Otherwise, we're inserting more elements than exist already, and we're
  // not inserting at the end.

  // Copy over the elements that we're about to overwrite.
  T *oldEnd = _end;
  _end += count;
  size_t numOverwritten = oldEnd - insertPos;
  std::uninitialized_copy(insertPos, oldEnd, _end - numOverwritten);

  // Replace the overwritten part.
  for (; numOverwritten > 0; --numOverwritten) {
    *insertPos = *first;
    ++insertPos; ++first;
  }

  // Insert the non-overwritten middle part.
  std::uninitialized_copy(first, last, oldEnd);
  return insertPos;
}

const SmallVectorImpl & operator=(const SmallVectorImpl & rhs);

bool operator==(const SmallVectorImpl & rhs) const {
  if (this->size() != rhs.size()) {
    return false;
  }
  return std::equal(_begin, _end, rhs._begin);
}

bool operator!=(const SmallVectorImpl & rhs) const {
  return !(*this == rhs);
}

bool operator<(const SmallVectorImpl & rhs) const {
  return std::lexicographical_compare(_begin, _end, rhs._begin, rhs._end);
}

#endif

}
