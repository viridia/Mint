/* ================================================================== *
 * Diagnostics unit test
 * ================================================================== */

#include "gtest/gtest.h"
#include "mint/collections/SmallString.h"
#include "mint/support/Path.h"
#include "TestHelpers.h"

namespace mint {

inline void appendStr(SmallVectorImpl<char> & first, StringRef second) {
  first.append(second.begin(), second.end());
}

TEST(PathTest, IsAbsolute) {
  ASSERT_TRUE(path::isAbsolute("/foo/bar"));
  ASSERT_TRUE(path::isAbsolute("/foo"));
  ASSERT_TRUE(path::isAbsolute("/"));
  ASSERT_FALSE(path::isAbsolute("foo/bar"));
  ASSERT_FALSE(path::isAbsolute("foo"));
  ASSERT_FALSE(path::isAbsolute(""));
  ASSERT_FALSE(path::isAbsolute("."));
}

TEST(PathTest, Filename) {
  ASSERT_EQ("", path::filename(""));
  ASSERT_EQ("foo", path::filename("foo"));
  ASSERT_EQ("foo", path::filename("/foo"));
  ASSERT_EQ("bar", path::filename("/foo/bar"));
  ASSERT_EQ("bar.txt", path::filename("/foo/bar.txt"));
  ASSERT_EQ("bar", path::filename("/foo.txt/bar"));
  ASSERT_EQ("", path::filename("/foo/"));
  ASSERT_EQ("", path::filename("/"));
}

TEST(PathTest, Parent) {
  ASSERT_EQ("", path::parent(""));
  ASSERT_EQ("", path::parent("foo"));
  ASSERT_EQ("/", path::parent("/foo"));
  ASSERT_EQ("/foo", path::parent("/foo/bar"));
  ASSERT_EQ("/foo", path::parent("/foo/bar.txt"));
  ASSERT_EQ("/foo.txt", path::parent("/foo.txt/bar"));
  ASSERT_EQ("/foo", path::parent("/foo/"));
  ASSERT_EQ("/", path::parent("/"));
}

TEST(PathTest, Normalize) {

  #define ASSERT_NORMALIZATION_EQ(expected, pathToNormalize) { \
    SmallVector<char, 16> npath; \
    appendStr(npath, pathToNormalize); \
    path::normalize(npath); \
    ASSERT_EQ(expected, StringRef(npath.begin(), npath.size())); \
  }

  ASSERT_NORMALIZATION_EQ("", "");
  ASSERT_NORMALIZATION_EQ("foo", "foo");
  ASSERT_NORMALIZATION_EQ("/", "/");
  ASSERT_NORMALIZATION_EQ("/foo", "/foo");
  ASSERT_NORMALIZATION_EQ("/bar", "/bar");
  ASSERT_NORMALIZATION_EQ("/foo/bar", "/foo/bar");
  ASSERT_NORMALIZATION_EQ("foo/bar", "foo/bar");
  ASSERT_NORMALIZATION_EQ("/", "/./");
  ASSERT_NORMALIZATION_EQ("/", "/.");
  ASSERT_NORMALIZATION_EQ("/foo", "/foo/./");
  ASSERT_NORMALIZATION_EQ("/foo", "/foo/.");
  ASSERT_NORMALIZATION_EQ("/foo/bar", "/foo/./bar");
  ASSERT_NORMALIZATION_EQ("/foo/bar", "/foo/./bar/");
  ASSERT_NORMALIZATION_EQ("/bar", "/foo/../bar");
  ASSERT_NORMALIZATION_EQ("/bar", "/foo/../bar/");
  ASSERT_NORMALIZATION_EQ("/foo/bar", "/foo/a/../bar");
  ASSERT_NORMALIZATION_EQ("/foo/bar", "/foo/a/../bar/");
  ASSERT_NORMALIZATION_EQ("/bar", "/foo/a/../../bar");
  ASSERT_NORMALIZATION_EQ("/bar", "/foo/a/../../bar/");
  ASSERT_NORMALIZATION_EQ("/bar", "/foo/a/.././../bar");
  ASSERT_NORMALIZATION_EQ("/bar", "/foo/a/.././../bar/");
  ASSERT_NORMALIZATION_EQ("/../bar", "/../bar");
  ASSERT_NORMALIZATION_EQ("/../bar", "/../bar/");
  ASSERT_NORMALIZATION_EQ("../bar", "../bar");
  ASSERT_NORMALIZATION_EQ("../bar", "../bar/");
  ASSERT_NORMALIZATION_EQ("/../../bar", "/../../bar");
  ASSERT_NORMALIZATION_EQ("/../../bar", "/../../bar/");
  ASSERT_NORMALIZATION_EQ("../../bar", "../../bar");
  ASSERT_NORMALIZATION_EQ("../../bar", "../../bar/");
}

// split

TEST(PathTest, Split) {
  ASSERT_EQ("", path::split("").first);
  ASSERT_EQ("", path::split("").second);
  ASSERT_EQ("", path::split("foo").first);
  ASSERT_EQ("foo", path::split("foo").second);
  ASSERT_EQ("", path::split("/foo").first);
  ASSERT_EQ("foo", path::split("/foo").second);
  ASSERT_EQ("/foo", path::split("/foo/bar").first);
  ASSERT_EQ("bar", path::split("/foo/bar").second);
  ASSERT_EQ("/foo", path::split("/foo/bar.txt").first);
  ASSERT_EQ("bar.txt", path::split("/foo/bar.txt").second);
  ASSERT_EQ("/foo.txt", path::split("/foo.txt/bar").first);
  ASSERT_EQ("bar", path::split("/foo.txt/bar").second);
  ASSERT_EQ("/foo", path::split("/foo/").first);
  ASSERT_EQ("", path::split("/foo/").second);
  ASSERT_EQ("", path::split("/").first);
  ASSERT_EQ("", path::split("/").second);
}

TEST(PathTest, Extension) {
  ASSERT_EQ("txt", path::extension("foo.txt"));
  ASSERT_EQ("txt", path::extension("foo.cpp.txt"));
  ASSERT_EQ("", path::extension("foo"));
  ASSERT_EQ("", path::extension("/bar.txt/foo"));
  ASSERT_EQ("", path::extension(""));
}

TEST(PathTest, ChangeExtension) {

  #define ASSERT_CHANGE_EXT(expected, base, ext) { \
    SmallVector<char, 16> npath; \
    appendStr(npath, base); \
    path::changeExtension(npath, ext); \
    ASSERT_EQ(expected, StringRef(npath.begin(), npath.size())); \
  }

  ASSERT_CHANGE_EXT("foo.cpp", "foo.h", "cpp");
  ASSERT_CHANGE_EXT("foo.cpp", "foo", "cpp");
}

TEST(PathTest, Concat) {

  #define ASSERT_CONCAT_EQ(expected, base, newpath) { \
    SmallVector<char, 16> npath; \
    appendStr(npath, base); \
    path::concat(npath, newpath); \
    ASSERT_EQ(expected, StringRef(npath.begin(), npath.size())); \
  }

  ASSERT_CONCAT_EQ("foo/bar", "foo", "bar");
  ASSERT_CONCAT_EQ("/foo/bar", "/foo", "bar");
  ASSERT_CONCAT_EQ("/bar", "foo", "/bar");
  ASSERT_CONCAT_EQ("foo/../bar", "foo", "../bar");
}

TEST(PathTest, Combine) {

  #define ASSERT_COMBINE_EQ(expected, base, ext) { \
    SmallString<16> npath(base); \
    path::combine(npath, ext); \
    ASSERT_EQ(StringRef(expected), StringRef(npath.begin(), npath.size())); \
  }

  ASSERT_COMBINE_EQ("foo/bar", "foo", "bar");
  ASSERT_COMBINE_EQ("/foo/bar", "/foo", "bar");
  ASSERT_COMBINE_EQ("/bar", "foo", "/bar");
  ASSERT_COMBINE_EQ("bar", "foo", "../bar");
}

TEST(PathTest, MakeRelative) {

  #define EXPECT_MAKEREL_EQ(expected, base, p) { \
    SmallString<16> npath; \
    path::makeRelative(base, p, npath); \
    EXPECT_EQ(StringRef(expected), StringRef(npath.begin(), npath.size())); \
  }

  //EXPECT_MAKEREL_EQ("/bar", "/foo", "/bar");
  EXPECT_MAKEREL_EQ(".", "/foo", "/foo");
  EXPECT_MAKEREL_EQ(".", "/foo/bar", "/foo/bar");
  EXPECT_MAKEREL_EQ("..", "/foo/bar", "/foo");
  EXPECT_MAKEREL_EQ("../..", "/foo/bar/baz", "/foo");
  EXPECT_MAKEREL_EQ("bar", "/foo", "/foo/bar");
  EXPECT_MAKEREL_EQ("../abc", "/foo/bar", "/foo/abc");
  EXPECT_MAKEREL_EQ("../../abc/def", "/foo/bar/baz", "/foo/abc/def");
}
}
