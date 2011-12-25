# -----------------------------------------------------------------------------
# Makefile for main module
# -----------------------------------------------------------------------------

VPATH = ..

.PHONY: all clean

all: mint test/unit/unittest lib/gtest.a lib/mint.a

lib/build/Target.cpp.o : \
	lib/build/Target.cpp
	@echo "Compiling /Users/talin/Projects/mint/mint/lib/build/Target.cpp"
	@gcc -c -Werror -I /Users/talin/Projects/mint/mint/build.osx/include -I /Users/talin/Projects/mint/mint/include -I /Users/talin/Projects/mint/mint/third_party/gtest-1.6.0/include -I /Users/talin/Projects/mint/mint/third_party/gtest-1.6.0 -I /Users/talin/Projects/mint/mint/third_party/re2 -o /Users/talin/Projects/mint/mint/build.osx/lib/build/Target.cpp.o /Users/talin/Projects/mint/mint/lib/build/Target.cpp

test/unit/WildcardMatcherTest.cpp.o : \
	test/unit/WildcardMatcherTest.cpp \
	lib/mint.a \
	third_party/re2/re2.a \
	lib/gtest.a
	@echo "Compiling /Users/talin/Projects/mint/mint/test/unit/WildcardMatcherTest.cpp"
	@gcc -c -Werror -I /Users/talin/Projects/mint/mint/build.osx/include -I /Users/talin/Projects/mint/mint/include -I /Users/talin/Projects/mint/mint/third_party/gtest-1.6.0/include -I /Users/talin/Projects/mint/mint/third_party/gtest-1.6.0 -I /Users/talin/Projects/mint/mint/third_party/re2 -o /Users/talin/Projects/mint/mint/build.osx/test/unit/WildcardMatcherTest.cpp.o /Users/talin/Projects/mint/mint/test/unit/WildcardMatcherTest.cpp

lib/eval/Evaluator.cpp.o : \
	lib/eval/Evaluator.cpp
	@echo "Compiling /Users/talin/Projects/mint/mint/lib/eval/Evaluator.cpp"
	@gcc -c -Werror -I /Users/talin/Projects/mint/mint/build.osx/include -I /Users/talin/Projects/mint/mint/include -I /Users/talin/Projects/mint/mint/third_party/gtest-1.6.0/include -I /Users/talin/Projects/mint/mint/third_party/gtest-1.6.0 -I /Users/talin/Projects/mint/mint/third_party/re2 -o /Users/talin/Projects/mint/mint/build.osx/lib/eval/Evaluator.cpp.o /Users/talin/Projects/mint/mint/lib/eval/Evaluator.cpp

lib/project/BuildConfiguration.cpp.o : \
	lib/project/BuildConfiguration.cpp
	@echo "Compiling /Users/talin/Projects/mint/mint/lib/project/BuildConfiguration.cpp"
	@gcc -c -Werror -I /Users/talin/Projects/mint/mint/build.osx/include -I /Users/talin/Projects/mint/mint/include -I /Users/talin/Projects/mint/mint/third_party/gtest-1.6.0/include -I /Users/talin/Projects/mint/mint/third_party/gtest-1.6.0 -I /Users/talin/Projects/mint/mint/third_party/re2 -o /Users/talin/Projects/mint/mint/build.osx/lib/project/BuildConfiguration.cpp.o /Users/talin/Projects/mint/mint/lib/project/BuildConfiguration.cpp

lib/graph/Module.cpp.o : \
	lib/graph/Module.cpp
	@echo "Compiling /Users/talin/Projects/mint/mint/lib/graph/Module.cpp"
	@gcc -c -Werror -I /Users/talin/Projects/mint/mint/build.osx/include -I /Users/talin/Projects/mint/mint/include -I /Users/talin/Projects/mint/mint/third_party/gtest-1.6.0/include -I /Users/talin/Projects/mint/mint/third_party/gtest-1.6.0 -I /Users/talin/Projects/mint/mint/third_party/re2 -o /Users/talin/Projects/mint/mint/build.osx/lib/graph/Module.cpp.o /Users/talin/Projects/mint/mint/lib/graph/Module.cpp

lib/intrinsic/Console.cpp.o : \
	lib/intrinsic/Console.cpp
	@echo "Compiling /Users/talin/Projects/mint/mint/lib/intrinsic/Console.cpp"
	@gcc -c -Werror -I /Users/talin/Projects/mint/mint/build.osx/include -I /Users/talin/Projects/mint/mint/include -I /Users/talin/Projects/mint/mint/third_party/gtest-1.6.0/include -I /Users/talin/Projects/mint/mint/third_party/gtest-1.6.0 -I /Users/talin/Projects/mint/mint/third_party/re2 -o /Users/talin/Projects/mint/mint/build.osx/lib/intrinsic/Console.cpp.o /Users/talin/Projects/mint/mint/lib/intrinsic/Console.cpp

lib/support/DirectoryIterator.cpp.o : \
	lib/support/DirectoryIterator.cpp
	@echo "Compiling /Users/talin/Projects/mint/mint/lib/support/DirectoryIterator.cpp"
	@gcc -c -Werror -I /Users/talin/Projects/mint/mint/build.osx/include -I /Users/talin/Projects/mint/mint/include -I /Users/talin/Projects/mint/mint/third_party/gtest-1.6.0/include -I /Users/talin/Projects/mint/mint/third_party/gtest-1.6.0 -I /Users/talin/Projects/mint/mint/third_party/re2 -o /Users/talin/Projects/mint/mint/build.osx/lib/support/DirectoryIterator.cpp.o /Users/talin/Projects/mint/mint/lib/support/DirectoryIterator.cpp

lib/graph/Oper.cpp.o : \
	lib/graph/Oper.cpp
	@echo "Compiling /Users/talin/Projects/mint/mint/lib/graph/Oper.cpp"
	@gcc -c -Werror -I /Users/talin/Projects/mint/mint/build.osx/include -I /Users/talin/Projects/mint/mint/include -I /Users/talin/Projects/mint/mint/third_party/gtest-1.6.0/include -I /Users/talin/Projects/mint/mint/third_party/gtest-1.6.0 -I /Users/talin/Projects/mint/mint/third_party/re2 -o /Users/talin/Projects/mint/mint/build.osx/lib/graph/Oper.cpp.o /Users/talin/Projects/mint/mint/lib/graph/Oper.cpp

lib/intrinsic/FileModule.cpp.o : \
	lib/intrinsic/FileModule.cpp
	@echo "Compiling /Users/talin/Projects/mint/mint/lib/intrinsic/FileModule.cpp"
	@gcc -c -Werror -I /Users/talin/Projects/mint/mint/build.osx/include -I /Users/talin/Projects/mint/mint/include -I /Users/talin/Projects/mint/mint/third_party/gtest-1.6.0/include -I /Users/talin/Projects/mint/mint/third_party/gtest-1.6.0 -I /Users/talin/Projects/mint/mint/third_party/re2 -o /Users/talin/Projects/mint/mint/build.osx/lib/intrinsic/FileModule.cpp.o /Users/talin/Projects/mint/mint/lib/intrinsic/FileModule.cpp

lib/intrinsic/StringRegistry.cpp.o : \
	lib/intrinsic/StringRegistry.cpp
	@echo "Compiling /Users/talin/Projects/mint/mint/lib/intrinsic/StringRegistry.cpp"
	@gcc -c -Werror -I /Users/talin/Projects/mint/mint/build.osx/include -I /Users/talin/Projects/mint/mint/include -I /Users/talin/Projects/mint/mint/third_party/gtest-1.6.0/include -I /Users/talin/Projects/mint/mint/third_party/gtest-1.6.0 -I /Users/talin/Projects/mint/mint/third_party/re2 -o /Users/talin/Projects/mint/mint/build.osx/lib/intrinsic/StringRegistry.cpp.o /Users/talin/Projects/mint/mint/lib/intrinsic/StringRegistry.cpp

lib/support/Diagnostics.cpp.o : \
	lib/support/Diagnostics.cpp
	@echo "Compiling /Users/talin/Projects/mint/mint/lib/support/Diagnostics.cpp"
	@gcc -c -Werror -I /Users/talin/Projects/mint/mint/build.osx/include -I /Users/talin/Projects/mint/mint/include -I /Users/talin/Projects/mint/mint/third_party/gtest-1.6.0/include -I /Users/talin/Projects/mint/mint/third_party/gtest-1.6.0 -I /Users/talin/Projects/mint/mint/third_party/re2 -o /Users/talin/Projects/mint/mint/build.osx/lib/support/Diagnostics.cpp.o /Users/talin/Projects/mint/mint/lib/support/Diagnostics.cpp

lib/project/Configurator.cpp.o : \
	lib/project/Configurator.cpp
	@echo "Compiling /Users/talin/Projects/mint/mint/lib/project/Configurator.cpp"
	@gcc -c -Werror -I /Users/talin/Projects/mint/mint/build.osx/include -I /Users/talin/Projects/mint/mint/include -I /Users/talin/Projects/mint/mint/third_party/gtest-1.6.0/include -I /Users/talin/Projects/mint/mint/third_party/gtest-1.6.0 -I /Users/talin/Projects/mint/mint/third_party/re2 -o /Users/talin/Projects/mint/mint/build.osx/lib/project/Configurator.cpp.o /Users/talin/Projects/mint/mint/lib/project/Configurator.cpp

lib/project/Project.cpp.o : \
	lib/project/Project.cpp
	@echo "Compiling /Users/talin/Projects/mint/mint/lib/project/Project.cpp"
	@gcc -c -Werror -I /Users/talin/Projects/mint/mint/build.osx/include -I /Users/talin/Projects/mint/mint/include -I /Users/talin/Projects/mint/mint/third_party/gtest-1.6.0/include -I /Users/talin/Projects/mint/mint/third_party/gtest-1.6.0 -I /Users/talin/Projects/mint/mint/third_party/re2 -o /Users/talin/Projects/mint/mint/build.osx/lib/project/Project.cpp.o /Users/talin/Projects/mint/mint/lib/project/Project.cpp

test/unit/FundamentalsTest.cpp.o : \
	test/unit/FundamentalsTest.cpp \
	lib/mint.a \
	third_party/re2/re2.a \
	lib/gtest.a
	@echo "Compiling /Users/talin/Projects/mint/mint/test/unit/FundamentalsTest.cpp"
	@gcc -c -Werror -I /Users/talin/Projects/mint/mint/build.osx/include -I /Users/talin/Projects/mint/mint/include -I /Users/talin/Projects/mint/mint/third_party/gtest-1.6.0/include -I /Users/talin/Projects/mint/mint/third_party/gtest-1.6.0 -I /Users/talin/Projects/mint/mint/third_party/re2 -o /Users/talin/Projects/mint/mint/build.osx/test/unit/FundamentalsTest.cpp.o /Users/talin/Projects/mint/mint/test/unit/FundamentalsTest.cpp

lib/support/OStream.cpp.o : \
	lib/support/OStream.cpp
	@echo "Compiling /Users/talin/Projects/mint/mint/lib/support/OStream.cpp"
	@gcc -c -Werror -I /Users/talin/Projects/mint/mint/build.osx/include -I /Users/talin/Projects/mint/mint/include -I /Users/talin/Projects/mint/mint/third_party/gtest-1.6.0/include -I /Users/talin/Projects/mint/mint/third_party/gtest-1.6.0 -I /Users/talin/Projects/mint/mint/third_party/re2 -o /Users/talin/Projects/mint/mint/build.osx/lib/support/OStream.cpp.o /Users/talin/Projects/mint/mint/lib/support/OStream.cpp

lib/support/Wildcard.cpp.o : \
	lib/support/Wildcard.cpp
	@echo "Compiling /Users/talin/Projects/mint/mint/lib/support/Wildcard.cpp"
	@gcc -c -Werror -I /Users/talin/Projects/mint/mint/build.osx/include -I /Users/talin/Projects/mint/mint/include -I /Users/talin/Projects/mint/mint/third_party/gtest-1.6.0/include -I /Users/talin/Projects/mint/mint/third_party/gtest-1.6.0 -I /Users/talin/Projects/mint/mint/third_party/re2 -o /Users/talin/Projects/mint/mint/build.osx/lib/support/Wildcard.cpp.o /Users/talin/Projects/mint/mint/lib/support/Wildcard.cpp

test/unit/ParserTest.cpp.o : \
	test/unit/ParserTest.cpp \
	lib/mint.a \
	third_party/re2/re2.a \
	lib/gtest.a
	@echo "Compiling /Users/talin/Projects/mint/mint/test/unit/ParserTest.cpp"
	@gcc -c -Werror -I /Users/talin/Projects/mint/mint/build.osx/include -I /Users/talin/Projects/mint/mint/include -I /Users/talin/Projects/mint/mint/third_party/gtest-1.6.0/include -I /Users/talin/Projects/mint/mint/third_party/gtest-1.6.0 -I /Users/talin/Projects/mint/mint/third_party/re2 -o /Users/talin/Projects/mint/mint/build.osx/test/unit/ParserTest.cpp.o /Users/talin/Projects/mint/mint/test/unit/ParserTest.cpp

third_party/re2/re2.a :
	@rm -f re2.a
	@ar -r re2.a $^

lib/build/JobMgr.cpp.o : \
	lib/build/JobMgr.cpp
	@echo "Compiling /Users/talin/Projects/mint/mint/lib/build/JobMgr.cpp"
	@gcc -c -Werror -I /Users/talin/Projects/mint/mint/build.osx/include -I /Users/talin/Projects/mint/mint/include -I /Users/talin/Projects/mint/mint/third_party/gtest-1.6.0/include -I /Users/talin/Projects/mint/mint/third_party/gtest-1.6.0 -I /Users/talin/Projects/mint/mint/third_party/re2 -o /Users/talin/Projects/mint/mint/build.osx/lib/build/JobMgr.cpp.o /Users/talin/Projects/mint/mint/lib/build/JobMgr.cpp

test/unit/StringRefTest.cpp.o : \
	test/unit/StringRefTest.cpp \
	lib/mint.a \
	third_party/re2/re2.a \
	lib/gtest.a
	@echo "Compiling /Users/talin/Projects/mint/mint/test/unit/StringRefTest.cpp"
	@gcc -c -Werror -I /Users/talin/Projects/mint/mint/build.osx/include -I /Users/talin/Projects/mint/mint/include -I /Users/talin/Projects/mint/mint/third_party/gtest-1.6.0/include -I /Users/talin/Projects/mint/mint/third_party/gtest-1.6.0 -I /Users/talin/Projects/mint/mint/third_party/re2 -o /Users/talin/Projects/mint/mint/build.osx/test/unit/StringRefTest.cpp.o /Users/talin/Projects/mint/mint/test/unit/StringRefTest.cpp

lib/build/TargetFinder.cpp.o : \
	lib/build/TargetFinder.cpp
	@echo "Compiling /Users/talin/Projects/mint/mint/lib/build/TargetFinder.cpp"
	@gcc -c -Werror -I /Users/talin/Projects/mint/mint/build.osx/include -I /Users/talin/Projects/mint/mint/include -I /Users/talin/Projects/mint/mint/third_party/gtest-1.6.0/include -I /Users/talin/Projects/mint/mint/third_party/gtest-1.6.0 -I /Users/talin/Projects/mint/mint/third_party/re2 -o /Users/talin/Projects/mint/mint/build.osx/lib/build/TargetFinder.cpp.o /Users/talin/Projects/mint/mint/lib/build/TargetFinder.cpp

lib/intrinsic/ModuleType.cpp.o : \
	lib/intrinsic/ModuleType.cpp
	@echo "Compiling /Users/talin/Projects/mint/mint/lib/intrinsic/ModuleType.cpp"
	@gcc -c -Werror -I /Users/talin/Projects/mint/mint/build.osx/include -I /Users/talin/Projects/mint/mint/include -I /Users/talin/Projects/mint/mint/third_party/gtest-1.6.0/include -I /Users/talin/Projects/mint/mint/third_party/gtest-1.6.0 -I /Users/talin/Projects/mint/mint/third_party/re2 -o /Users/talin/Projects/mint/mint/build.osx/lib/intrinsic/ModuleType.cpp.o /Users/talin/Projects/mint/mint/lib/intrinsic/ModuleType.cpp

lib/graph/Node.cpp.o : \
	lib/graph/Node.cpp
	@echo "Compiling /Users/talin/Projects/mint/mint/lib/graph/Node.cpp"
	@gcc -c -Werror -I /Users/talin/Projects/mint/mint/build.osx/include -I /Users/talin/Projects/mint/mint/include -I /Users/talin/Projects/mint/mint/third_party/gtest-1.6.0/include -I /Users/talin/Projects/mint/mint/third_party/gtest-1.6.0 -I /Users/talin/Projects/mint/mint/third_party/re2 -o /Users/talin/Projects/mint/mint/build.osx/lib/graph/Node.cpp.o /Users/talin/Projects/mint/mint/lib/graph/Node.cpp

lib/project/MakefileGenerator.cpp.o : \
	lib/project/MakefileGenerator.cpp
	@echo "Compiling /Users/talin/Projects/mint/mint/lib/project/MakefileGenerator.cpp"
	@gcc -c -Werror -I /Users/talin/Projects/mint/mint/build.osx/include -I /Users/talin/Projects/mint/mint/include -I /Users/talin/Projects/mint/mint/third_party/gtest-1.6.0/include -I /Users/talin/Projects/mint/mint/third_party/gtest-1.6.0 -I /Users/talin/Projects/mint/mint/third_party/re2 -o /Users/talin/Projects/mint/mint/build.osx/lib/project/MakefileGenerator.cpp.o /Users/talin/Projects/mint/mint/lib/project/MakefileGenerator.cpp

lib/graph/TypeRegistry.cpp.o : \
	lib/graph/TypeRegistry.cpp
	@echo "Compiling /Users/talin/Projects/mint/mint/lib/graph/TypeRegistry.cpp"
	@gcc -c -Werror -I /Users/talin/Projects/mint/mint/build.osx/include -I /Users/talin/Projects/mint/mint/include -I /Users/talin/Projects/mint/mint/third_party/gtest-1.6.0/include -I /Users/talin/Projects/mint/mint/third_party/gtest-1.6.0 -I /Users/talin/Projects/mint/mint/third_party/re2 -o /Users/talin/Projects/mint/mint/build.osx/lib/graph/TypeRegistry.cpp.o /Users/talin/Projects/mint/mint/lib/graph/TypeRegistry.cpp

lib/support/OSError.cpp.o : \
	lib/support/OSError.cpp
	@echo "Compiling /Users/talin/Projects/mint/mint/lib/support/OSError.cpp"
	@gcc -c -Werror -I /Users/talin/Projects/mint/mint/build.osx/include -I /Users/talin/Projects/mint/mint/include -I /Users/talin/Projects/mint/mint/third_party/gtest-1.6.0/include -I /Users/talin/Projects/mint/mint/third_party/gtest-1.6.0 -I /Users/talin/Projects/mint/mint/third_party/re2 -o /Users/talin/Projects/mint/mint/build.osx/lib/support/OSError.cpp.o /Users/talin/Projects/mint/mint/lib/support/OSError.cpp

lib/build/Directory.cpp.o : \
	lib/build/Directory.cpp
	@echo "Compiling /Users/talin/Projects/mint/mint/lib/build/Directory.cpp"
	@gcc -c -Werror -I /Users/talin/Projects/mint/mint/build.osx/include -I /Users/talin/Projects/mint/mint/include -I /Users/talin/Projects/mint/mint/third_party/gtest-1.6.0/include -I /Users/talin/Projects/mint/mint/third_party/gtest-1.6.0 -I /Users/talin/Projects/mint/mint/third_party/re2 -o /Users/talin/Projects/mint/mint/build.osx/lib/build/Directory.cpp.o /Users/talin/Projects/mint/mint/lib/build/Directory.cpp

lib/graph/Object.cpp.o : \
	lib/graph/Object.cpp
	@echo "Compiling /Users/talin/Projects/mint/mint/lib/graph/Object.cpp"
	@gcc -c -Werror -I /Users/talin/Projects/mint/mint/build.osx/include -I /Users/talin/Projects/mint/mint/include -I /Users/talin/Projects/mint/mint/third_party/gtest-1.6.0/include -I /Users/talin/Projects/mint/mint/third_party/gtest-1.6.0 -I /Users/talin/Projects/mint/mint/third_party/re2 -o /Users/talin/Projects/mint/mint/build.osx/lib/graph/Object.cpp.o /Users/talin/Projects/mint/mint/lib/graph/Object.cpp

lib/graph/Function.cpp.o : \
	lib/graph/Function.cpp
	@echo "Compiling /Users/talin/Projects/mint/mint/lib/graph/Function.cpp"
	@gcc -c -Werror -I /Users/talin/Projects/mint/mint/build.osx/include -I /Users/talin/Projects/mint/mint/include -I /Users/talin/Projects/mint/mint/third_party/gtest-1.6.0/include -I /Users/talin/Projects/mint/mint/third_party/gtest-1.6.0 -I /Users/talin/Projects/mint/mint/third_party/re2 -o /Users/talin/Projects/mint/mint/build.osx/lib/graph/Function.cpp.o /Users/talin/Projects/mint/mint/lib/graph/Function.cpp

test/unit/StringDictTest.cpp.o : \
	test/unit/StringDictTest.cpp \
	lib/mint.a \
	third_party/re2/re2.a \
	lib/gtest.a
	@echo "Compiling /Users/talin/Projects/mint/mint/test/unit/StringDictTest.cpp"
	@gcc -c -Werror -I /Users/talin/Projects/mint/mint/build.osx/include -I /Users/talin/Projects/mint/mint/include -I /Users/talin/Projects/mint/mint/third_party/gtest-1.6.0/include -I /Users/talin/Projects/mint/mint/third_party/gtest-1.6.0 -I /Users/talin/Projects/mint/mint/third_party/re2 -o /Users/talin/Projects/mint/mint/build.osx/test/unit/StringDictTest.cpp.o /Users/talin/Projects/mint/mint/test/unit/StringDictTest.cpp

test/unit/LexerTest.cpp.o : \
	test/unit/LexerTest.cpp \
	lib/mint.a \
	third_party/re2/re2.a \
	lib/gtest.a
	@echo "Compiling /Users/talin/Projects/mint/mint/test/unit/LexerTest.cpp"
	@gcc -c -Werror -I /Users/talin/Projects/mint/mint/build.osx/include -I /Users/talin/Projects/mint/mint/include -I /Users/talin/Projects/mint/mint/third_party/gtest-1.6.0/include -I /Users/talin/Projects/mint/mint/third_party/gtest-1.6.0 -I /Users/talin/Projects/mint/mint/third_party/re2 -o /Users/talin/Projects/mint/mint/build.osx/test/unit/LexerTest.cpp.o /Users/talin/Projects/mint/mint/test/unit/LexerTest.cpp

lib/intrinsic/ListType.cpp.o : \
	lib/intrinsic/ListType.cpp
	@echo "Compiling /Users/talin/Projects/mint/mint/lib/intrinsic/ListType.cpp"
	@gcc -c -Werror -I /Users/talin/Projects/mint/mint/build.osx/include -I /Users/talin/Projects/mint/mint/include -I /Users/talin/Projects/mint/mint/third_party/gtest-1.6.0/include -I /Users/talin/Projects/mint/mint/third_party/gtest-1.6.0 -I /Users/talin/Projects/mint/mint/third_party/re2 -o /Users/talin/Projects/mint/mint/build.osx/lib/intrinsic/ListType.cpp.o /Users/talin/Projects/mint/mint/lib/intrinsic/ListType.cpp

lib/intrinsic/Subprocess.cpp.o : \
	lib/intrinsic/Subprocess.cpp
	@echo "Compiling /Users/talin/Projects/mint/mint/lib/intrinsic/Subprocess.cpp"
	@gcc -c -Werror -I /Users/talin/Projects/mint/mint/build.osx/include -I /Users/talin/Projects/mint/mint/include -I /Users/talin/Projects/mint/mint/third_party/gtest-1.6.0/include -I /Users/talin/Projects/mint/mint/third_party/gtest-1.6.0 -I /Users/talin/Projects/mint/mint/third_party/re2 -o /Users/talin/Projects/mint/mint/build.osx/lib/intrinsic/Subprocess.cpp.o /Users/talin/Projects/mint/mint/lib/intrinsic/Subprocess.cpp

lib/support/CommandLine.cpp.o : \
	lib/support/CommandLine.cpp
	@echo "Compiling /Users/talin/Projects/mint/mint/lib/support/CommandLine.cpp"
	@gcc -c -Werror -I /Users/talin/Projects/mint/mint/build.osx/include -I /Users/talin/Projects/mint/mint/include -I /Users/talin/Projects/mint/mint/third_party/gtest-1.6.0/include -I /Users/talin/Projects/mint/mint/third_party/gtest-1.6.0 -I /Users/talin/Projects/mint/mint/third_party/re2 -o /Users/talin/Projects/mint/mint/build.osx/lib/support/CommandLine.cpp.o /Users/talin/Projects/mint/mint/lib/support/CommandLine.cpp

lib/build/File.cpp.o : \
	lib/build/File.cpp
	@echo "Compiling /Users/talin/Projects/mint/mint/lib/build/File.cpp"
	@gcc -c -Werror -I /Users/talin/Projects/mint/mint/build.osx/include -I /Users/talin/Projects/mint/mint/include -I /Users/talin/Projects/mint/mint/third_party/gtest-1.6.0/include -I /Users/talin/Projects/mint/mint/third_party/gtest-1.6.0 -I /Users/talin/Projects/mint/mint/third_party/re2 -o /Users/talin/Projects/mint/mint/build.osx/lib/build/File.cpp.o /Users/talin/Projects/mint/mint/lib/build/File.cpp

tools/mint/mint.cpp.o : \
	tools/mint/mint.cpp \
	lib/mint.a \
	third_party/re2/re2.a
	@echo "Compiling /Users/talin/Projects/mint/mint/tools/mint/mint.cpp"
	@gcc -c -Werror -I /Users/talin/Projects/mint/mint/build.osx/include -I /Users/talin/Projects/mint/mint/include -I /Users/talin/Projects/mint/mint/third_party/gtest-1.6.0/include -I /Users/talin/Projects/mint/mint/third_party/gtest-1.6.0 -I /Users/talin/Projects/mint/mint/third_party/re2 -o /Users/talin/Projects/mint/mint/build.osx/tools/mint/mint.cpp.o /Users/talin/Projects/mint/mint/tools/mint/mint.cpp

lib/intrinsic/PathMethods.cpp.o : \
	lib/intrinsic/PathMethods.cpp
	@echo "Compiling /Users/talin/Projects/mint/mint/lib/intrinsic/PathMethods.cpp"
	@gcc -c -Werror -I /Users/talin/Projects/mint/mint/build.osx/include -I /Users/talin/Projects/mint/mint/include -I /Users/talin/Projects/mint/mint/third_party/gtest-1.6.0/include -I /Users/talin/Projects/mint/mint/third_party/gtest-1.6.0 -I /Users/talin/Projects/mint/mint/third_party/re2 -o /Users/talin/Projects/mint/mint/build.osx/lib/intrinsic/PathMethods.cpp.o /Users/talin/Projects/mint/mint/lib/intrinsic/PathMethods.cpp

lib/support/Path.cpp.o : \
	lib/support/Path.cpp
	@echo "Compiling /Users/talin/Projects/mint/mint/lib/support/Path.cpp"
	@gcc -c -Werror -I /Users/talin/Projects/mint/mint/build.osx/include -I /Users/talin/Projects/mint/mint/include -I /Users/talin/Projects/mint/mint/third_party/gtest-1.6.0/include -I /Users/talin/Projects/mint/mint/third_party/gtest-1.6.0 -I /Users/talin/Projects/mint/mint/third_party/re2 -o /Users/talin/Projects/mint/mint/build.osx/lib/support/Path.cpp.o /Users/talin/Projects/mint/mint/lib/support/Path.cpp

lib/build/TargetMgr.cpp.o : \
	lib/build/TargetMgr.cpp
	@echo "Compiling /Users/talin/Projects/mint/mint/lib/build/TargetMgr.cpp"
	@gcc -c -Werror -I /Users/talin/Projects/mint/mint/build.osx/include -I /Users/talin/Projects/mint/mint/include -I /Users/talin/Projects/mint/mint/third_party/gtest-1.6.0/include -I /Users/talin/Projects/mint/mint/third_party/gtest-1.6.0 -I /Users/talin/Projects/mint/mint/third_party/re2 -o /Users/talin/Projects/mint/mint/build.osx/lib/build/TargetMgr.cpp.o /Users/talin/Projects/mint/mint/lib/build/TargetMgr.cpp

test/unit/OStreamTest.cpp.o : \
	test/unit/OStreamTest.cpp \
	lib/mint.a \
	third_party/re2/re2.a \
	lib/gtest.a
	@echo "Compiling /Users/talin/Projects/mint/mint/test/unit/OStreamTest.cpp"
	@gcc -c -Werror -I /Users/talin/Projects/mint/mint/build.osx/include -I /Users/talin/Projects/mint/mint/include -I /Users/talin/Projects/mint/mint/third_party/gtest-1.6.0/include -I /Users/talin/Projects/mint/mint/third_party/gtest-1.6.0 -I /Users/talin/Projects/mint/mint/third_party/re2 -o /Users/talin/Projects/mint/mint/build.osx/test/unit/OStreamTest.cpp.o /Users/talin/Projects/mint/mint/test/unit/OStreamTest.cpp

lib/graph/Type.cpp.o : \
	lib/graph/Type.cpp
	@echo "Compiling /Users/talin/Projects/mint/mint/lib/graph/Type.cpp"
	@gcc -c -Werror -I /Users/talin/Projects/mint/mint/build.osx/include -I /Users/talin/Projects/mint/mint/include -I /Users/talin/Projects/mint/mint/third_party/gtest-1.6.0/include -I /Users/talin/Projects/mint/mint/third_party/gtest-1.6.0 -I /Users/talin/Projects/mint/mint/third_party/re2 -o /Users/talin/Projects/mint/mint/build.osx/lib/graph/Type.cpp.o /Users/talin/Projects/mint/mint/lib/graph/Type.cpp

lib/project/ModuleLoader.cpp.o : \
	lib/project/ModuleLoader.cpp
	@echo "Compiling /Users/talin/Projects/mint/mint/lib/project/ModuleLoader.cpp"
	@gcc -c -Werror -I /Users/talin/Projects/mint/mint/build.osx/include -I /Users/talin/Projects/mint/mint/include -I /Users/talin/Projects/mint/mint/third_party/gtest-1.6.0/include -I /Users/talin/Projects/mint/mint/third_party/gtest-1.6.0 -I /Users/talin/Projects/mint/mint/third_party/re2 -o /Users/talin/Projects/mint/mint/build.osx/lib/project/ModuleLoader.cpp.o /Users/talin/Projects/mint/mint/lib/project/ModuleLoader.cpp

third_party/gtest-1.6.0/src/gtest-all.cc.o : \
	third_party/gtest-1.6.0/src/gtest-all.cc
	@echo "Compiling /Users/talin/Projects/mint/mint/third_party/gtest-1.6.0/src/gtest-all.cc"
	@gcc -c -Werror -I /Users/talin/Projects/mint/mint/build.osx/include -I /Users/talin/Projects/mint/mint/include -I /Users/talin/Projects/mint/mint/third_party/gtest-1.6.0/include -I /Users/talin/Projects/mint/mint/third_party/gtest-1.6.0 -I /Users/talin/Projects/mint/mint/third_party/re2 -o /Users/talin/Projects/mint/mint/build.osx/third_party/gtest-1.6.0/src/gtest-all.cc.o /Users/talin/Projects/mint/mint/third_party/gtest-1.6.0/src/gtest-all.cc

lib/graph/GraphBuilder.cpp.o : \
	lib/graph/GraphBuilder.cpp
	@echo "Compiling /Users/talin/Projects/mint/mint/lib/graph/GraphBuilder.cpp"
	@gcc -c -Werror -I /Users/talin/Projects/mint/mint/build.osx/include -I /Users/talin/Projects/mint/mint/include -I /Users/talin/Projects/mint/mint/third_party/gtest-1.6.0/include -I /Users/talin/Projects/mint/mint/third_party/gtest-1.6.0 -I /Users/talin/Projects/mint/mint/third_party/re2 -o /Users/talin/Projects/mint/mint/build.osx/lib/graph/GraphBuilder.cpp.o /Users/talin/Projects/mint/mint/lib/graph/GraphBuilder.cpp

test/unit/_main.cpp.o : \
	test/unit/_main.cpp \
	lib/mint.a \
	third_party/re2/re2.a \
	lib/gtest.a
	@echo "Compiling /Users/talin/Projects/mint/mint/test/unit/_main.cpp"
	@gcc -c -Werror -I /Users/talin/Projects/mint/mint/build.osx/include -I /Users/talin/Projects/mint/mint/include -I /Users/talin/Projects/mint/mint/third_party/gtest-1.6.0/include -I /Users/talin/Projects/mint/mint/third_party/gtest-1.6.0 -I /Users/talin/Projects/mint/mint/third_party/re2 -o /Users/talin/Projects/mint/mint/build.osx/test/unit/_main.cpp.o /Users/talin/Projects/mint/mint/test/unit/_main.cpp

lib/support/Assert.cpp.o : \
	lib/support/Assert.cpp
	@echo "Compiling /Users/talin/Projects/mint/mint/lib/support/Assert.cpp"
	@gcc -c -Werror -I /Users/talin/Projects/mint/mint/build.osx/include -I /Users/talin/Projects/mint/mint/include -I /Users/talin/Projects/mint/mint/third_party/gtest-1.6.0/include -I /Users/talin/Projects/mint/mint/third_party/gtest-1.6.0 -I /Users/talin/Projects/mint/mint/third_party/re2 -o /Users/talin/Projects/mint/mint/build.osx/lib/support/Assert.cpp.o /Users/talin/Projects/mint/mint/lib/support/Assert.cpp

lib/intrinsic/Fundamentals.cpp.o : \
	lib/intrinsic/Fundamentals.cpp
	@echo "Compiling /Users/talin/Projects/mint/mint/lib/intrinsic/Fundamentals.cpp"
	@gcc -c -Werror -I /Users/talin/Projects/mint/mint/build.osx/include -I /Users/talin/Projects/mint/mint/include -I /Users/talin/Projects/mint/mint/third_party/gtest-1.6.0/include -I /Users/talin/Projects/mint/mint/third_party/gtest-1.6.0 -I /Users/talin/Projects/mint/mint/third_party/re2 -o /Users/talin/Projects/mint/mint/build.osx/lib/intrinsic/Fundamentals.cpp.o /Users/talin/Projects/mint/mint/lib/intrinsic/Fundamentals.cpp

lib/support/Hashing.cpp.o : \
	lib/support/Hashing.cpp
	@echo "Compiling /Users/talin/Projects/mint/mint/lib/support/Hashing.cpp"
	@gcc -c -Werror -I /Users/talin/Projects/mint/mint/build.osx/include -I /Users/talin/Projects/mint/mint/include -I /Users/talin/Projects/mint/mint/third_party/gtest-1.6.0/include -I /Users/talin/Projects/mint/mint/third_party/gtest-1.6.0 -I /Users/talin/Projects/mint/mint/third_party/re2 -o /Users/talin/Projects/mint/mint/build.osx/lib/support/Hashing.cpp.o /Users/talin/Projects/mint/mint/lib/support/Hashing.cpp

lib/parse/Parser.cpp.o : \
	lib/parse/Parser.cpp
	@echo "Compiling /Users/talin/Projects/mint/mint/lib/parse/Parser.cpp"
	@gcc -c -Werror -I /Users/talin/Projects/mint/mint/build.osx/include -I /Users/talin/Projects/mint/mint/include -I /Users/talin/Projects/mint/mint/third_party/gtest-1.6.0/include -I /Users/talin/Projects/mint/mint/third_party/gtest-1.6.0 -I /Users/talin/Projects/mint/mint/third_party/re2 -o /Users/talin/Projects/mint/mint/build.osx/lib/parse/Parser.cpp.o /Users/talin/Projects/mint/mint/lib/parse/Parser.cpp

mint : \
	lib/mint.a \
	third_party/re2/re2.a \
	tools/mint/mint.cpp.o
	@echo "Linking program mint"
	@gcc -Werror -lstdc++ -o mint $^

lib/lex/Lexer.cpp.o : \
	lib/lex/Lexer.cpp
	@echo "Compiling /Users/talin/Projects/mint/mint/lib/lex/Lexer.cpp"
	@gcc -c -Werror -I /Users/talin/Projects/mint/mint/build.osx/include -I /Users/talin/Projects/mint/mint/include -I /Users/talin/Projects/mint/mint/third_party/gtest-1.6.0/include -I /Users/talin/Projects/mint/mint/third_party/gtest-1.6.0 -I /Users/talin/Projects/mint/mint/third_party/re2 -o /Users/talin/Projects/mint/mint/build.osx/lib/lex/Lexer.cpp.o /Users/talin/Projects/mint/mint/lib/lex/Lexer.cpp

test/unit/unittest : \
	lib/mint.a \
	third_party/re2/re2.a \
	lib/gtest.a \
	test/unit/_main.cpp.o \
	test/unit/EvaluatorTest.cpp.o \
	test/unit/FundamentalsTest.cpp.o \
	test/unit/LexerTest.cpp.o \
	test/unit/OStreamTest.cpp.o \
	test/unit/ParserTest.cpp.o \
	test/unit/PathTest.cpp.o \
	test/unit/SmallVectorTest.cpp.o \
	test/unit/StringDictTest.cpp.o \
	test/unit/StringRefTest.cpp.o \
	test/unit/TypeRegistryTest.cpp.o \
	test/unit/WildcardMatcherTest.cpp.o
	@echo "Linking program test/unit/unittest"
	@gcc -Werror -lstdc++ -o test/unit/unittest $^

lib/intrinsic/RegEx.cpp.o : \
	lib/intrinsic/RegEx.cpp
	@echo "Compiling /Users/talin/Projects/mint/mint/lib/intrinsic/RegEx.cpp"
	@gcc -c -Werror -I /Users/talin/Projects/mint/mint/build.osx/include -I /Users/talin/Projects/mint/mint/include -I /Users/talin/Projects/mint/mint/third_party/gtest-1.6.0/include -I /Users/talin/Projects/mint/mint/third_party/gtest-1.6.0 -I /Users/talin/Projects/mint/mint/third_party/re2 -o /Users/talin/Projects/mint/mint/build.osx/lib/intrinsic/RegEx.cpp.o /Users/talin/Projects/mint/mint/lib/intrinsic/RegEx.cpp

lib/support/Process.cpp.o : \
	lib/support/Process.cpp
	@echo "Compiling /Users/talin/Projects/mint/mint/lib/support/Process.cpp"
	@gcc -c -Werror -I /Users/talin/Projects/mint/mint/build.osx/include -I /Users/talin/Projects/mint/mint/include -I /Users/talin/Projects/mint/mint/third_party/gtest-1.6.0/include -I /Users/talin/Projects/mint/mint/third_party/gtest-1.6.0 -I /Users/talin/Projects/mint/mint/third_party/re2 -o /Users/talin/Projects/mint/mint/build.osx/lib/support/Process.cpp.o /Users/talin/Projects/mint/mint/lib/support/Process.cpp

test/unit/TypeRegistryTest.cpp.o : \
	test/unit/TypeRegistryTest.cpp \
	lib/mint.a \
	third_party/re2/re2.a \
	lib/gtest.a
	@echo "Compiling /Users/talin/Projects/mint/mint/test/unit/TypeRegistryTest.cpp"
	@gcc -c -Werror -I /Users/talin/Projects/mint/mint/build.osx/include -I /Users/talin/Projects/mint/mint/include -I /Users/talin/Projects/mint/mint/third_party/gtest-1.6.0/include -I /Users/talin/Projects/mint/mint/third_party/gtest-1.6.0 -I /Users/talin/Projects/mint/mint/third_party/re2 -o /Users/talin/Projects/mint/mint/build.osx/test/unit/TypeRegistryTest.cpp.o /Users/talin/Projects/mint/mint/test/unit/TypeRegistryTest.cpp

test/unit/SmallVectorTest.cpp.o : \
	test/unit/SmallVectorTest.cpp \
	lib/mint.a \
	third_party/re2/re2.a \
	lib/gtest.a
	@echo "Compiling /Users/talin/Projects/mint/mint/test/unit/SmallVectorTest.cpp"
	@gcc -c -Werror -I /Users/talin/Projects/mint/mint/build.osx/include -I /Users/talin/Projects/mint/mint/include -I /Users/talin/Projects/mint/mint/third_party/gtest-1.6.0/include -I /Users/talin/Projects/mint/mint/third_party/gtest-1.6.0 -I /Users/talin/Projects/mint/mint/third_party/re2 -o /Users/talin/Projects/mint/mint/build.osx/test/unit/SmallVectorTest.cpp.o /Users/talin/Projects/mint/mint/test/unit/SmallVectorTest.cpp

lib/support/GC.cpp.o : \
	lib/support/GC.cpp
	@echo "Compiling /Users/talin/Projects/mint/mint/lib/support/GC.cpp"
	@gcc -c -Werror -I /Users/talin/Projects/mint/mint/build.osx/include -I /Users/talin/Projects/mint/mint/include -I /Users/talin/Projects/mint/mint/third_party/gtest-1.6.0/include -I /Users/talin/Projects/mint/mint/third_party/gtest-1.6.0 -I /Users/talin/Projects/mint/mint/third_party/re2 -o /Users/talin/Projects/mint/mint/build.osx/lib/support/GC.cpp.o /Users/talin/Projects/mint/mint/lib/support/GC.cpp

lib/collections/StringRef.cpp.o : \
	lib/collections/StringRef.cpp
	@echo "Compiling /Users/talin/Projects/mint/mint/lib/collections/StringRef.cpp"
	@gcc -c -Werror -I /Users/talin/Projects/mint/mint/build.osx/include -I /Users/talin/Projects/mint/mint/include -I /Users/talin/Projects/mint/mint/third_party/gtest-1.6.0/include -I /Users/talin/Projects/mint/mint/third_party/gtest-1.6.0 -I /Users/talin/Projects/mint/mint/third_party/re2 -o /Users/talin/Projects/mint/mint/build.osx/lib/collections/StringRef.cpp.o /Users/talin/Projects/mint/mint/lib/collections/StringRef.cpp

lib/gtest.a : \
	third_party/gtest-1.6.0/src/gtest-all.cc.o
	@rm -f lib/gtest.a
	@ar -r lib/gtest.a $^

lib/mint.a : \
	lib/build/Directory.cpp.o \
	lib/build/File.cpp.o \
	lib/build/JobMgr.cpp.o \
	lib/build/Target.cpp.o \
	lib/build/TargetFinder.cpp.o \
	lib/build/TargetMgr.cpp.o \
	lib/collections/StringRef.cpp.o \
	lib/eval/Evaluator.cpp.o \
	lib/graph/Function.cpp.o \
	lib/graph/GraphBuilder.cpp.o \
	lib/graph/GraphWriter.cpp.o \
	lib/graph/Module.cpp.o \
	lib/graph/Node.cpp.o \
	lib/graph/Object.cpp.o \
	lib/graph/Oper.cpp.o \
	lib/graph/String.cpp.o \
	lib/graph/Type.cpp.o \
	lib/graph/TypeRegistry.cpp.o \
	lib/intrinsic/Console.cpp.o \
	lib/intrinsic/FileModule.cpp.o \
	lib/intrinsic/FileSystem.cpp.o \
	lib/intrinsic/Fundamentals.cpp.o \
	lib/intrinsic/ListType.cpp.o \
	lib/intrinsic/ModuleType.cpp.o \
	lib/intrinsic/PathMethods.cpp.o \
	lib/intrinsic/RegEx.cpp.o \
	lib/intrinsic/StringRegistry.cpp.o \
	lib/intrinsic/Subprocess.cpp.o \
	lib/lex/Lexer.cpp.o \
	lib/parse/Parser.cpp.o \
	lib/project/BuildConfiguration.cpp.o \
	lib/project/Configurator.cpp.o \
	lib/project/MakefileGenerator.cpp.o \
	lib/project/ModuleLoader.cpp.o \
	lib/project/Project.cpp.o \
	lib/project/ProjectWriterXml.cpp.o \
	lib/support/Assert.cpp.o \
	lib/support/CommandLine.cpp.o \
	lib/support/Diagnostics.cpp.o \
	lib/support/DirectoryIterator.cpp.o \
	lib/support/GC.cpp.o \
	lib/support/Hashing.cpp.o \
	lib/support/OSError.cpp.o \
	lib/support/OStream.cpp.o \
	lib/support/Path.cpp.o \
	lib/support/Process.cpp.o \
	lib/support/Wildcard.cpp.o
	@rm -f lib/mint.a
	@ar -r lib/mint.a $^

lib/intrinsic/FileSystem.cpp.o : \
	lib/intrinsic/FileSystem.cpp
	@echo "Compiling /Users/talin/Projects/mint/mint/lib/intrinsic/FileSystem.cpp"
	@gcc -c -Werror -I /Users/talin/Projects/mint/mint/build.osx/include -I /Users/talin/Projects/mint/mint/include -I /Users/talin/Projects/mint/mint/third_party/gtest-1.6.0/include -I /Users/talin/Projects/mint/mint/third_party/gtest-1.6.0 -I /Users/talin/Projects/mint/mint/third_party/re2 -o /Users/talin/Projects/mint/mint/build.osx/lib/intrinsic/FileSystem.cpp.o /Users/talin/Projects/mint/mint/lib/intrinsic/FileSystem.cpp

lib/graph/GraphWriter.cpp.o : \
	lib/graph/GraphWriter.cpp
	@echo "Compiling /Users/talin/Projects/mint/mint/lib/graph/GraphWriter.cpp"
	@gcc -c -Werror -I /Users/talin/Projects/mint/mint/build.osx/include -I /Users/talin/Projects/mint/mint/include -I /Users/talin/Projects/mint/mint/third_party/gtest-1.6.0/include -I /Users/talin/Projects/mint/mint/third_party/gtest-1.6.0 -I /Users/talin/Projects/mint/mint/third_party/re2 -o /Users/talin/Projects/mint/mint/build.osx/lib/graph/GraphWriter.cpp.o /Users/talin/Projects/mint/mint/lib/graph/GraphWriter.cpp

lib/project/ProjectWriterXml.cpp.o : \
	lib/project/ProjectWriterXml.cpp
	@echo "Compiling /Users/talin/Projects/mint/mint/lib/project/ProjectWriterXml.cpp"
	@gcc -c -Werror -I /Users/talin/Projects/mint/mint/build.osx/include -I /Users/talin/Projects/mint/mint/include -I /Users/talin/Projects/mint/mint/third_party/gtest-1.6.0/include -I /Users/talin/Projects/mint/mint/third_party/gtest-1.6.0 -I /Users/talin/Projects/mint/mint/third_party/re2 -o /Users/talin/Projects/mint/mint/build.osx/lib/project/ProjectWriterXml.cpp.o /Users/talin/Projects/mint/mint/lib/project/ProjectWriterXml.cpp

test/unit/EvaluatorTest.cpp.o : \
	test/unit/EvaluatorTest.cpp \
	lib/mint.a \
	third_party/re2/re2.a \
	lib/gtest.a
	@echo "Compiling /Users/talin/Projects/mint/mint/test/unit/EvaluatorTest.cpp"
	@gcc -c -Werror -I /Users/talin/Projects/mint/mint/build.osx/include -I /Users/talin/Projects/mint/mint/include -I /Users/talin/Projects/mint/mint/third_party/gtest-1.6.0/include -I /Users/talin/Projects/mint/mint/third_party/gtest-1.6.0 -I /Users/talin/Projects/mint/mint/third_party/re2 -o /Users/talin/Projects/mint/mint/build.osx/test/unit/EvaluatorTest.cpp.o /Users/talin/Projects/mint/mint/test/unit/EvaluatorTest.cpp

test/unit/PathTest.cpp.o : \
	test/unit/PathTest.cpp \
	lib/mint.a \
	third_party/re2/re2.a \
	lib/gtest.a
	@echo "Compiling /Users/talin/Projects/mint/mint/test/unit/PathTest.cpp"
	@gcc -c -Werror -I /Users/talin/Projects/mint/mint/build.osx/include -I /Users/talin/Projects/mint/mint/include -I /Users/talin/Projects/mint/mint/third_party/gtest-1.6.0/include -I /Users/talin/Projects/mint/mint/third_party/gtest-1.6.0 -I /Users/talin/Projects/mint/mint/third_party/re2 -o /Users/talin/Projects/mint/mint/build.osx/test/unit/PathTest.cpp.o /Users/talin/Projects/mint/mint/test/unit/PathTest.cpp

lib/graph/String.cpp.o : \
	lib/graph/String.cpp
	@echo "Compiling /Users/talin/Projects/mint/mint/lib/graph/String.cpp"
	@gcc -c -Werror -I /Users/talin/Projects/mint/mint/build.osx/include -I /Users/talin/Projects/mint/mint/include -I /Users/talin/Projects/mint/mint/third_party/gtest-1.6.0/include -I /Users/talin/Projects/mint/mint/third_party/gtest-1.6.0 -I /Users/talin/Projects/mint/mint/third_party/re2 -o /Users/talin/Projects/mint/mint/build.osx/lib/graph/String.cpp.o /Users/talin/Projects/mint/mint/lib/graph/String.cpp

clean:
	@rm -rf  \
	lib/build/Target.cpp.o \
	test/unit/WildcardMatcherTest.cpp.o \
	lib/eval/Evaluator.cpp.o \
	lib/project/BuildConfiguration.cpp.o \
	lib/graph/Module.cpp.o \
	lib/intrinsic/Console.cpp.o \
	lib/support/DirectoryIterator.cpp.o \
	lib/graph/Oper.cpp.o \
	lib/intrinsic/FileModule.cpp.o \
	lib/intrinsic/StringRegistry.cpp.o \
	lib/support/Diagnostics.cpp.o \
	lib/project/Configurator.cpp.o \
	lib/project/Project.cpp.o \
	test/unit/FundamentalsTest.cpp.o \
	lib/support/OStream.cpp.o \
	lib/support/Wildcard.cpp.o \
	test/unit/ParserTest.cpp.o \
	third_party/re2/re2.a \
	lib/build/JobMgr.cpp.o \
	test/unit/StringRefTest.cpp.o \
	lib/build/TargetFinder.cpp.o \
	lib/intrinsic/ModuleType.cpp.o \
	lib/graph/Node.cpp.o \
	lib/project/MakefileGenerator.cpp.o \
	lib/graph/TypeRegistry.cpp.o \
	lib/support/OSError.cpp.o \
	lib/build/Directory.cpp.o \
	lib/graph/Object.cpp.o \
	lib/graph/Function.cpp.o \
	test/unit/StringDictTest.cpp.o \
	test/unit/LexerTest.cpp.o \
	lib/intrinsic/ListType.cpp.o \
	lib/intrinsic/Subprocess.cpp.o \
	lib/support/CommandLine.cpp.o \
	lib/build/File.cpp.o \
	tools/mint/mint.cpp.o \
	lib/intrinsic/PathMethods.cpp.o \
	lib/support/Path.cpp.o \
	lib/build/TargetMgr.cpp.o \
	test/unit/OStreamTest.cpp.o \
	lib/graph/Type.cpp.o \
	lib/project/ModuleLoader.cpp.o \
	third_party/gtest-1.6.0/src/gtest-all.cc.o \
	lib/graph/GraphBuilder.cpp.o \
	test/unit/_main.cpp.o \
	lib/support/Assert.cpp.o \
	lib/intrinsic/Fundamentals.cpp.o \
	lib/support/Hashing.cpp.o \
	lib/parse/Parser.cpp.o \
	mint \
	lib/lex/Lexer.cpp.o \
	test/unit/unittest \
	lib/intrinsic/RegEx.cpp.o \
	lib/support/Process.cpp.o \
	test/unit/TypeRegistryTest.cpp.o \
	test/unit/SmallVectorTest.cpp.o \
	lib/support/GC.cpp.o \
	lib/collections/StringRef.cpp.o \
	lib/gtest.a \
	lib/mint.a \
	lib/intrinsic/FileSystem.cpp.o \
	lib/graph/GraphWriter.cpp.o \
	lib/project/ProjectWriterXml.cpp.o \
	test/unit/EvaluatorTest.cpp.o \
	test/unit/PathTest.cpp.o \
	lib/graph/String.cpp.o

