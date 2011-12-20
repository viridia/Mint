# MINT Makefile

SRCDIR = $(dir ${CURDIR})
VPATH = ..

include ${SRCDIR}/Makefile.common
-include ${SRCDIR}/Makefile.deps

LOCAL_INCLUDE_DIRS = \
	-I include\
	-I ${SRCDIR}include\
	-I ${SRCDIR}third_party/gtest-1.6.0/include\
	-I ${SRCDIR}third_party/gtest-1.6.0 \
	-I ${SRCDIR}third_party/re2

.PHONY: clean deps

CXXFLAGS = -g -Werror -Wall
#CXX = clang

%.o: lib/build/%.cpp
	${CXX} ${CXXFLAGS} ${LOCAL_INCLUDE_DIRS} -c -o ${PWD}/$@ $<

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
	echo -e "namespace mint {\n  const char * SRC_PRELUDE_PATH = \"${SRCDIR}prelude\";\n}\n" > stdpath.cpp

LIBRE2 =  ${SRCDIR}third_party/re2/obj/libre2.a

stdpath.o: stdpath.cpp
	${CXX} ${CXXFLAGS} ${LOCAL_INCLUDE_DIRS} -c -o ${PWD}/$@ $<

mint.a: ${MINT_OBJECTS} stdpath.o
	rm -f $@
	ar -r $@ $^

${LIBRE2}:
	cd ${SRCDIR}third_party/re2/ && $(MAKE)

mint: mint.o mint.a ${LIBRE2}
	${CXX} -lstdc++ -o $@ $^

gtest-all.o: ${SRCDIR}/third_party/gtest-1.6.0/src/gtest-all.cc
	${CXX} ${LOCAL_INCLUDE_DIRS} -c -o $@ $<

unittest: ${MINT_UNITTEST_OBJECTS} mint.a gtest-all.o ${LIBRE2}
	${CXX} -o $@ -lpthread $^

deps: ${MINT_SOURCES} ${MINT_UNITTEST_SOURCES}
	${CXX} ${LOCAL_INCLUDE_DIRS} -MM $^ > ${SRCDIR}/Makefile.deps

check: unittest
	@./unittest

clean:
	@rm -rf *.o *.a unittest
