# MINT Makefile

SRCDIR = $(dir ${CURDIR})
VPATH = ..

include ${SRCDIR}/Makefile.common
include ${SRCDIR}/Makefile.deps

LOCAL_INCLUDE_DIRS = \
	-I ${SRCDIR}include\
	-I ${SRCDIR}third_party/gtest-1.6.0/include\
	-I ${SRCDIR}third_party/gtest-1.6.0

.PHONY: clean deps

CXXFLAGS = -g -Werror -Wall
#CXX = clang

#%.o: %.cpp
#	${CXX} ${LOCAL_INCLUDE_DIRS} -c -o $@ $^

%.o: lib/collections/%.cpp
	${CXX} ${CXXFLAGS} ${LOCAL_INCLUDE_DIRS} -c -o ${PWD}/$@ $<

%.o: lib/eval/%.cpp
	${CXX} ${CXXFLAGS} ${LOCAL_INCLUDE_DIRS} -c -o ${PWD}/$@ $<

%.o: lib/graph/%.cpp
	${CXX} ${CXXFLAGS} ${LOCAL_INCLUDE_DIRS} -c -o ${PWD}/$@ $<

%.o: lib/intrinsic/%.cpp
	${CXX} ${CXXFLAGS} ${LOCAL_INCLUDE_DIRS} -c -o ${PWD}/$@ $<

%.o: lib/lex/%.cpp
	${CXX} ${CXXFLAGS} ${LOCAL_INCLUDE_DIRS} -c -o ${PWD}/$@ $<

%.o: lib/parse/%.cpp
	${CXX} ${CXXFLAGS} ${LOCAL_INCLUDE_DIRS} -c -o ${PWD}/$@ $<

%.o: lib/project/%.cpp
	${CXX} ${CXXFLAGS} ${LOCAL_INCLUDE_DIRS} -c -o ${PWD}/$@ $<

%.o: lib/support/%.cpp
	${CXX} ${CXXFLAGS} ${LOCAL_INCLUDE_DIRS} -c -o ${PWD}/$@ $<

%.o: test/unit/%.cpp
	${CXX} ${CXXFLAGS} ${LOCAL_INCLUDE_DIRS} -c -o ${PWD}/$@ $<

%.o: tools/mint/%.cpp
	${CXX} ${CXXFLAGS} ${LOCAL_INCLUDE_DIRS} -c -o ${PWD}/$@ $<

# Prelude path in source directory
stdpath.cpp:
	echo "namespace mint {\n  const char * SRC_PRELUDE_PATH = \"${SRCDIR}prelude\";\n}\n" > stdpath.cpp

stdpath.o: stdpath.cpp
	${CXX} ${CXXFLAGS} ${LOCAL_INCLUDE_DIRS} -c -o ${PWD}/$@ $<

mint.a: ${MINT_OBJECTS} stdpath.o
	rm -f $@
	ar -r $@ $^

mint: mint.o mint.a
	${CXX} -lstdc++ -o $@ $^

gtest-all.o: ${SRCDIR}/third_party/gtest-1.6.0/src/gtest-all.cc
	${CXX} ${LOCAL_INCLUDE_DIRS} -c -o $@ $<

unittest: ${MINT_UNITTEST_OBJECTS} mint.a gtest-all.o
	${CXX} -o $@ $^

deps: ${MINT_SOURCES} ${MINT_UNITTEST_SOURCES}
	${CXX} ${LOCAL_INCLUDE_DIRS} -MM $^ > ${SRCDIR}/Makefile.deps

check: unittest
	@./unittest

clean:
	@rm -rf *.o *.a unittest
