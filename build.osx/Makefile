# -----------------------------------------------------------------------------
# Makefile for main module
# -----------------------------------------------------------------------------

VPATH = ..

.PHONY: all clean

all: lib/gtest.a lib/mint.a mint test/unit/unittest

lib/gtest.a : \
	third_party/gtest-1.6.0/src/gtest-all.cc.o
	@mkdir -p lib
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
	lib/project/OptionFinder.cpp.o \
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
	@mkdir -p lib
	@rm -f lib/mint.a
	@ar -r lib/mint.a $^

mint : \
	lib/mint.a \
	third_party/re2/re2.a \
	tools/mint/mint.cpp.o
	@echo "Linking program mint"
	@gcc -Werror -lstdc++ -lpthread -o mint $^

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
	@mkdir -p test/unit
	@echo "Linking program test/unit/unittest"
	@gcc -Werror -lstdc++ -lpthread -o test/unit/unittest $^

.PHONY: check

check : \
	test/unit/unittest
	@test/unit/unittest

.PHONY: install

install : \
	mint \
	../prelude/builders.mint \
	../prelude/compilers/ar.mint \
	../prelude/compilers/clang.mint \
	../prelude/compilers/compiler.mint \
	../prelude/compilers/gcc.mint \
	../prelude/compilers/ld.mint \
	../prelude/configtests/module.mint \
	../prelude/installer.mint \
	../prelude/module.mint \
	../prelude/packaging.mint \
	../prelude/platform/linux.mint \
	../prelude/platform/module.mint \
	../prelude/platform/osx.mint \
	../prelude/templates/module.mint

.PHONY: tarball

tarball : \
	dist/mint-osx-0.1.0a1/bin/mint \
	dist/mint-osx-0.1.0a1/lib/mint/prelude/builders.mint \
	dist/mint-osx-0.1.0a1/lib/mint/prelude/compilers/ar.mint \
	dist/mint-osx-0.1.0a1/lib/mint/prelude/compilers/clang.mint \
	dist/mint-osx-0.1.0a1/lib/mint/prelude/compilers/compiler.mint \
	dist/mint-osx-0.1.0a1/lib/mint/prelude/compilers/gcc.mint \
	dist/mint-osx-0.1.0a1/lib/mint/prelude/compilers/ld.mint \
	dist/mint-osx-0.1.0a1/lib/mint/prelude/configtests/module.mint \
	dist/mint-osx-0.1.0a1/lib/mint/prelude/installer.mint \
	dist/mint-osx-0.1.0a1/lib/mint/prelude/module.mint \
	dist/mint-osx-0.1.0a1/lib/mint/prelude/packaging.mint \
	dist/mint-osx-0.1.0a1/lib/mint/prelude/platform/linux.mint \
	dist/mint-osx-0.1.0a1/lib/mint/prelude/platform/module.mint \
	dist/mint-osx-0.1.0a1/lib/mint/prelude/platform/osx.mint \
	dist/mint-osx-0.1.0a1/lib/mint/prelude/templates/module.mint
	@echo "Building archive file mint-osx-0.1.0a1.tgz"

dist/mint-osx-0.1.0a1/bin/mint : \
	mint
	@mkdir -p dist/mint-osx-0.1.0a1/bin
	@cp /Users/talin/Projects/mint/mint/build.osx/mint /Users/talin/Projects/mint/mint/build.osx/dist/mint-osx-0.1.0a1/bin/mint

dist/mint-osx-0.1.0a1/lib/mint/prelude/builders.mint dist/mint-osx-0.1.0a1/lib/mint/prelude/compilers/ar.mint dist/mint-osx-0.1.0a1/lib/mint/prelude/compilers/clang.mint dist/mint-osx-0.1.0a1/lib/mint/prelude/compilers/compiler.mint dist/mint-osx-0.1.0a1/lib/mint/prelude/compilers/gcc.mint dist/mint-osx-0.1.0a1/lib/mint/prelude/compilers/ld.mint dist/mint-osx-0.1.0a1/lib/mint/prelude/configtests/module.mint dist/mint-osx-0.1.0a1/lib/mint/prelude/installer.mint dist/mint-osx-0.1.0a1/lib/mint/prelude/module.mint dist/mint-osx-0.1.0a1/lib/mint/prelude/packaging.mint dist/mint-osx-0.1.0a1/lib/mint/prelude/platform/linux.mint dist/mint-osx-0.1.0a1/lib/mint/prelude/platform/module.mint dist/mint-osx-0.1.0a1/lib/mint/prelude/platform/osx.mint dist/mint-osx-0.1.0a1/lib/mint/prelude/templates/module.mint : \
	../prelude/builders.mint \
	../prelude/compilers/ar.mint \
	../prelude/compilers/clang.mint \
	../prelude/compilers/compiler.mint \
	../prelude/compilers/gcc.mint \
	../prelude/compilers/ld.mint \
	../prelude/configtests/module.mint \
	../prelude/installer.mint \
	../prelude/module.mint \
	../prelude/packaging.mint \
	../prelude/platform/linux.mint \
	../prelude/platform/module.mint \
	../prelude/platform/osx.mint \
	../prelude/templates/module.mint
	@mkdir -p dist/mint-osx-0.1.0a1/lib/mint/prelude/platform
	@mkdir -p dist/mint-osx-0.1.0a1/lib/mint/prelude
	@mkdir -p dist/mint-osx-0.1.0a1/lib/mint/prelude/templates
	@mkdir -p dist/mint-osx-0.1.0a1/lib/mint/prelude/compilers
	@mkdir -p dist/mint-osx-0.1.0a1/lib/mint/prelude/configtests
	@cp /Users/talin/Projects/mint/mint/prelude/builders.mint /Users/talin/Projects/mint/mint/build.osx/dist/mint-osx-0.1.0a1/lib/mint/prelude/builders.mint
	@cp /Users/talin/Projects/mint/mint/prelude/compilers/ar.mint /Users/talin/Projects/mint/mint/build.osx/dist/mint-osx-0.1.0a1/lib/mint/prelude/compilers/ar.mint
	@cp /Users/talin/Projects/mint/mint/prelude/compilers/clang.mint /Users/talin/Projects/mint/mint/build.osx/dist/mint-osx-0.1.0a1/lib/mint/prelude/compilers/clang.mint
	@cp /Users/talin/Projects/mint/mint/prelude/compilers/compiler.mint /Users/talin/Projects/mint/mint/build.osx/dist/mint-osx-0.1.0a1/lib/mint/prelude/compilers/compiler.mint
	@cp /Users/talin/Projects/mint/mint/prelude/compilers/gcc.mint /Users/talin/Projects/mint/mint/build.osx/dist/mint-osx-0.1.0a1/lib/mint/prelude/compilers/gcc.mint
	@cp /Users/talin/Projects/mint/mint/prelude/compilers/ld.mint /Users/talin/Projects/mint/mint/build.osx/dist/mint-osx-0.1.0a1/lib/mint/prelude/compilers/ld.mint
	@cp /Users/talin/Projects/mint/mint/prelude/configtests/module.mint /Users/talin/Projects/mint/mint/build.osx/dist/mint-osx-0.1.0a1/lib/mint/prelude/configtests/module.mint
	@cp /Users/talin/Projects/mint/mint/prelude/installer.mint /Users/talin/Projects/mint/mint/build.osx/dist/mint-osx-0.1.0a1/lib/mint/prelude/installer.mint
	@cp /Users/talin/Projects/mint/mint/prelude/module.mint /Users/talin/Projects/mint/mint/build.osx/dist/mint-osx-0.1.0a1/lib/mint/prelude/module.mint
	@cp /Users/talin/Projects/mint/mint/prelude/packaging.mint /Users/talin/Projects/mint/mint/build.osx/dist/mint-osx-0.1.0a1/lib/mint/prelude/packaging.mint
	@cp /Users/talin/Projects/mint/mint/prelude/platform/linux.mint /Users/talin/Projects/mint/mint/build.osx/dist/mint-osx-0.1.0a1/lib/mint/prelude/platform/linux.mint
	@cp /Users/talin/Projects/mint/mint/prelude/platform/module.mint /Users/talin/Projects/mint/mint/build.osx/dist/mint-osx-0.1.0a1/lib/mint/prelude/platform/module.mint
	@cp /Users/talin/Projects/mint/mint/prelude/platform/osx.mint /Users/talin/Projects/mint/mint/build.osx/dist/mint-osx-0.1.0a1/lib/mint/prelude/platform/osx.mint
	@cp /Users/talin/Projects/mint/mint/prelude/templates/module.mint /Users/talin/Projects/mint/mint/build.osx/dist/mint-osx-0.1.0a1/lib/mint/prelude/templates/module.mint

lib/build/Directory.cpp.o : \
	../lib/build/Directory.cpp
	@mkdir -p lib/build
	@echo "Compiling lib/build/Directory.cpp"
	@gcc -c -Werror -I include -I ../include -I ../third_party/gtest-1.6.0/include -I ../third_party/gtest-1.6.0 -I ../third_party/re2 -o lib/build/Directory.cpp.o ../lib/build/Directory.cpp

lib/build/File.cpp.o : \
	../lib/build/File.cpp
	@mkdir -p lib/build
	@echo "Compiling lib/build/File.cpp"
	@gcc -c -Werror -I include -I ../include -I ../third_party/gtest-1.6.0/include -I ../third_party/gtest-1.6.0 -I ../third_party/re2 -o lib/build/File.cpp.o ../lib/build/File.cpp

lib/build/JobMgr.cpp.o : \
	../lib/build/JobMgr.cpp
	@mkdir -p lib/build
	@echo "Compiling lib/build/JobMgr.cpp"
	@gcc -c -Werror -I include -I ../include -I ../third_party/gtest-1.6.0/include -I ../third_party/gtest-1.6.0 -I ../third_party/re2 -o lib/build/JobMgr.cpp.o ../lib/build/JobMgr.cpp

lib/build/Target.cpp.o : \
	../lib/build/Target.cpp
	@mkdir -p lib/build
	@echo "Compiling lib/build/Target.cpp"
	@gcc -c -Werror -I include -I ../include -I ../third_party/gtest-1.6.0/include -I ../third_party/gtest-1.6.0 -I ../third_party/re2 -o lib/build/Target.cpp.o ../lib/build/Target.cpp

lib/build/TargetFinder.cpp.o : \
	../lib/build/TargetFinder.cpp
	@mkdir -p lib/build
	@echo "Compiling lib/build/TargetFinder.cpp"
	@gcc -c -Werror -I include -I ../include -I ../third_party/gtest-1.6.0/include -I ../third_party/gtest-1.6.0 -I ../third_party/re2 -o lib/build/TargetFinder.cpp.o ../lib/build/TargetFinder.cpp

lib/build/TargetMgr.cpp.o : \
	../lib/build/TargetMgr.cpp
	@mkdir -p lib/build
	@echo "Compiling lib/build/TargetMgr.cpp"
	@gcc -c -Werror -I include -I ../include -I ../third_party/gtest-1.6.0/include -I ../third_party/gtest-1.6.0 -I ../third_party/re2 -o lib/build/TargetMgr.cpp.o ../lib/build/TargetMgr.cpp

lib/collections/StringRef.cpp.o : \
	../lib/collections/StringRef.cpp
	@mkdir -p lib/collections
	@echo "Compiling lib/collections/StringRef.cpp"
	@gcc -c -Werror -I include -I ../include -I ../third_party/gtest-1.6.0/include -I ../third_party/gtest-1.6.0 -I ../third_party/re2 -o lib/collections/StringRef.cpp.o ../lib/collections/StringRef.cpp

lib/eval/Evaluator.cpp.o : \
	../lib/eval/Evaluator.cpp
	@mkdir -p lib/eval
	@echo "Compiling lib/eval/Evaluator.cpp"
	@gcc -c -Werror -I include -I ../include -I ../third_party/gtest-1.6.0/include -I ../third_party/gtest-1.6.0 -I ../third_party/re2 -o lib/eval/Evaluator.cpp.o ../lib/eval/Evaluator.cpp

lib/graph/Function.cpp.o : \
	../lib/graph/Function.cpp
	@mkdir -p lib/graph
	@echo "Compiling lib/graph/Function.cpp"
	@gcc -c -Werror -I include -I ../include -I ../third_party/gtest-1.6.0/include -I ../third_party/gtest-1.6.0 -I ../third_party/re2 -o lib/graph/Function.cpp.o ../lib/graph/Function.cpp

lib/graph/GraphWriter.cpp.o : \
	../lib/graph/GraphWriter.cpp
	@mkdir -p lib/graph
	@echo "Compiling lib/graph/GraphWriter.cpp"
	@gcc -c -Werror -I include -I ../include -I ../third_party/gtest-1.6.0/include -I ../third_party/gtest-1.6.0 -I ../third_party/re2 -o lib/graph/GraphWriter.cpp.o ../lib/graph/GraphWriter.cpp

lib/graph/Module.cpp.o : \
	../lib/graph/Module.cpp
	@mkdir -p lib/graph
	@echo "Compiling lib/graph/Module.cpp"
	@gcc -c -Werror -I include -I ../include -I ../third_party/gtest-1.6.0/include -I ../third_party/gtest-1.6.0 -I ../third_party/re2 -o lib/graph/Module.cpp.o ../lib/graph/Module.cpp

lib/graph/Node.cpp.o : \
	../lib/graph/Node.cpp
	@mkdir -p lib/graph
	@echo "Compiling lib/graph/Node.cpp"
	@gcc -c -Werror -I include -I ../include -I ../third_party/gtest-1.6.0/include -I ../third_party/gtest-1.6.0 -I ../third_party/re2 -o lib/graph/Node.cpp.o ../lib/graph/Node.cpp

lib/graph/Object.cpp.o : \
	../lib/graph/Object.cpp
	@mkdir -p lib/graph
	@echo "Compiling lib/graph/Object.cpp"
	@gcc -c -Werror -I include -I ../include -I ../third_party/gtest-1.6.0/include -I ../third_party/gtest-1.6.0 -I ../third_party/re2 -o lib/graph/Object.cpp.o ../lib/graph/Object.cpp

lib/graph/Oper.cpp.o : \
	../lib/graph/Oper.cpp
	@mkdir -p lib/graph
	@echo "Compiling lib/graph/Oper.cpp"
	@gcc -c -Werror -I include -I ../include -I ../third_party/gtest-1.6.0/include -I ../third_party/gtest-1.6.0 -I ../third_party/re2 -o lib/graph/Oper.cpp.o ../lib/graph/Oper.cpp

lib/graph/String.cpp.o : \
	../lib/graph/String.cpp
	@mkdir -p lib/graph
	@echo "Compiling lib/graph/String.cpp"
	@gcc -c -Werror -I include -I ../include -I ../third_party/gtest-1.6.0/include -I ../third_party/gtest-1.6.0 -I ../third_party/re2 -o lib/graph/String.cpp.o ../lib/graph/String.cpp

lib/graph/Type.cpp.o : \
	../lib/graph/Type.cpp
	@mkdir -p lib/graph
	@echo "Compiling lib/graph/Type.cpp"
	@gcc -c -Werror -I include -I ../include -I ../third_party/gtest-1.6.0/include -I ../third_party/gtest-1.6.0 -I ../third_party/re2 -o lib/graph/Type.cpp.o ../lib/graph/Type.cpp

lib/graph/TypeRegistry.cpp.o : \
	../lib/graph/TypeRegistry.cpp
	@mkdir -p lib/graph
	@echo "Compiling lib/graph/TypeRegistry.cpp"
	@gcc -c -Werror -I include -I ../include -I ../third_party/gtest-1.6.0/include -I ../third_party/gtest-1.6.0 -I ../third_party/re2 -o lib/graph/TypeRegistry.cpp.o ../lib/graph/TypeRegistry.cpp

lib/intrinsic/Console.cpp.o : \
	../lib/intrinsic/Console.cpp
	@mkdir -p lib/intrinsic
	@echo "Compiling lib/intrinsic/Console.cpp"
	@gcc -c -Werror -I include -I ../include -I ../third_party/gtest-1.6.0/include -I ../third_party/gtest-1.6.0 -I ../third_party/re2 -o lib/intrinsic/Console.cpp.o ../lib/intrinsic/Console.cpp

lib/intrinsic/FileModule.cpp.o : \
	../lib/intrinsic/FileModule.cpp
	@mkdir -p lib/intrinsic
	@echo "Compiling lib/intrinsic/FileModule.cpp"
	@gcc -c -Werror -I include -I ../include -I ../third_party/gtest-1.6.0/include -I ../third_party/gtest-1.6.0 -I ../third_party/re2 -o lib/intrinsic/FileModule.cpp.o ../lib/intrinsic/FileModule.cpp

lib/intrinsic/FileSystem.cpp.o : \
	../lib/intrinsic/FileSystem.cpp
	@mkdir -p lib/intrinsic
	@echo "Compiling lib/intrinsic/FileSystem.cpp"
	@gcc -c -Werror -I include -I ../include -I ../third_party/gtest-1.6.0/include -I ../third_party/gtest-1.6.0 -I ../third_party/re2 -o lib/intrinsic/FileSystem.cpp.o ../lib/intrinsic/FileSystem.cpp

lib/intrinsic/Fundamentals.cpp.o : \
	../lib/intrinsic/Fundamentals.cpp
	@mkdir -p lib/intrinsic
	@echo "Compiling lib/intrinsic/Fundamentals.cpp"
	@gcc -c -Werror -I include -I ../include -I ../third_party/gtest-1.6.0/include -I ../third_party/gtest-1.6.0 -I ../third_party/re2 -o lib/intrinsic/Fundamentals.cpp.o ../lib/intrinsic/Fundamentals.cpp

lib/intrinsic/ListType.cpp.o : \
	../lib/intrinsic/ListType.cpp
	@mkdir -p lib/intrinsic
	@echo "Compiling lib/intrinsic/ListType.cpp"
	@gcc -c -Werror -I include -I ../include -I ../third_party/gtest-1.6.0/include -I ../third_party/gtest-1.6.0 -I ../third_party/re2 -o lib/intrinsic/ListType.cpp.o ../lib/intrinsic/ListType.cpp

lib/intrinsic/ModuleType.cpp.o : \
	../lib/intrinsic/ModuleType.cpp
	@mkdir -p lib/intrinsic
	@echo "Compiling lib/intrinsic/ModuleType.cpp"
	@gcc -c -Werror -I include -I ../include -I ../third_party/gtest-1.6.0/include -I ../third_party/gtest-1.6.0 -I ../third_party/re2 -o lib/intrinsic/ModuleType.cpp.o ../lib/intrinsic/ModuleType.cpp

lib/intrinsic/PathMethods.cpp.o : \
	../lib/intrinsic/PathMethods.cpp
	@mkdir -p lib/intrinsic
	@echo "Compiling lib/intrinsic/PathMethods.cpp"
	@gcc -c -Werror -I include -I ../include -I ../third_party/gtest-1.6.0/include -I ../third_party/gtest-1.6.0 -I ../third_party/re2 -o lib/intrinsic/PathMethods.cpp.o ../lib/intrinsic/PathMethods.cpp

lib/intrinsic/RegEx.cpp.o : \
	../lib/intrinsic/RegEx.cpp
	@mkdir -p lib/intrinsic
	@echo "Compiling lib/intrinsic/RegEx.cpp"
	@gcc -c -Werror -I include -I ../include -I ../third_party/gtest-1.6.0/include -I ../third_party/gtest-1.6.0 -I ../third_party/re2 -o lib/intrinsic/RegEx.cpp.o ../lib/intrinsic/RegEx.cpp

lib/intrinsic/StringRegistry.cpp.o : \
	../lib/intrinsic/StringRegistry.cpp
	@mkdir -p lib/intrinsic
	@echo "Compiling lib/intrinsic/StringRegistry.cpp"
	@gcc -c -Werror -I include -I ../include -I ../third_party/gtest-1.6.0/include -I ../third_party/gtest-1.6.0 -I ../third_party/re2 -o lib/intrinsic/StringRegistry.cpp.o ../lib/intrinsic/StringRegistry.cpp

lib/intrinsic/Subprocess.cpp.o : \
	../lib/intrinsic/Subprocess.cpp
	@mkdir -p lib/intrinsic
	@echo "Compiling lib/intrinsic/Subprocess.cpp"
	@gcc -c -Werror -I include -I ../include -I ../third_party/gtest-1.6.0/include -I ../third_party/gtest-1.6.0 -I ../third_party/re2 -o lib/intrinsic/Subprocess.cpp.o ../lib/intrinsic/Subprocess.cpp

lib/lex/Lexer.cpp.o : \
	../lib/lex/Lexer.cpp
	@mkdir -p lib/lex
	@echo "Compiling lib/lex/Lexer.cpp"
	@gcc -c -Werror -I include -I ../include -I ../third_party/gtest-1.6.0/include -I ../third_party/gtest-1.6.0 -I ../third_party/re2 -o lib/lex/Lexer.cpp.o ../lib/lex/Lexer.cpp

lib/parse/Parser.cpp.o : \
	../lib/parse/Parser.cpp
	@mkdir -p lib/parse
	@echo "Compiling lib/parse/Parser.cpp"
	@gcc -c -Werror -I include -I ../include -I ../third_party/gtest-1.6.0/include -I ../third_party/gtest-1.6.0 -I ../third_party/re2 -o lib/parse/Parser.cpp.o ../lib/parse/Parser.cpp

lib/project/BuildConfiguration.cpp.o : \
	../lib/project/BuildConfiguration.cpp
	@mkdir -p lib/project
	@echo "Compiling lib/project/BuildConfiguration.cpp"
	@gcc -c -Werror -I include -I ../include -I ../third_party/gtest-1.6.0/include -I ../third_party/gtest-1.6.0 -I ../third_party/re2 -o lib/project/BuildConfiguration.cpp.o ../lib/project/BuildConfiguration.cpp

lib/project/Configurator.cpp.o : \
	../lib/project/Configurator.cpp
	@mkdir -p lib/project
	@echo "Compiling lib/project/Configurator.cpp"
	@gcc -c -Werror -I include -I ../include -I ../third_party/gtest-1.6.0/include -I ../third_party/gtest-1.6.0 -I ../third_party/re2 -o lib/project/Configurator.cpp.o ../lib/project/Configurator.cpp

lib/project/MakefileGenerator.cpp.o : \
	../lib/project/MakefileGenerator.cpp
	@mkdir -p lib/project
	@echo "Compiling lib/project/MakefileGenerator.cpp"
	@gcc -c -Werror -I include -I ../include -I ../third_party/gtest-1.6.0/include -I ../third_party/gtest-1.6.0 -I ../third_party/re2 -o lib/project/MakefileGenerator.cpp.o ../lib/project/MakefileGenerator.cpp

lib/project/ModuleLoader.cpp.o : \
	../lib/project/ModuleLoader.cpp
	@mkdir -p lib/project
	@echo "Compiling lib/project/ModuleLoader.cpp"
	@gcc -c -Werror -I include -I ../include -I ../third_party/gtest-1.6.0/include -I ../third_party/gtest-1.6.0 -I ../third_party/re2 -o lib/project/ModuleLoader.cpp.o ../lib/project/ModuleLoader.cpp

lib/project/OptionFinder.cpp.o : \
	../lib/project/OptionFinder.cpp
	@mkdir -p lib/project
	@echo "Compiling lib/project/OptionFinder.cpp"
	@gcc -c -Werror -I include -I ../include -I ../third_party/gtest-1.6.0/include -I ../third_party/gtest-1.6.0 -I ../third_party/re2 -o lib/project/OptionFinder.cpp.o ../lib/project/OptionFinder.cpp

lib/project/Project.cpp.o : \
	../lib/project/Project.cpp
	@mkdir -p lib/project
	@echo "Compiling lib/project/Project.cpp"
	@gcc -c -Werror -I include -I ../include -I ../third_party/gtest-1.6.0/include -I ../third_party/gtest-1.6.0 -I ../third_party/re2 -o lib/project/Project.cpp.o ../lib/project/Project.cpp

lib/project/ProjectWriterXml.cpp.o : \
	../lib/project/ProjectWriterXml.cpp
	@mkdir -p lib/project
	@echo "Compiling lib/project/ProjectWriterXml.cpp"
	@gcc -c -Werror -I include -I ../include -I ../third_party/gtest-1.6.0/include -I ../third_party/gtest-1.6.0 -I ../third_party/re2 -o lib/project/ProjectWriterXml.cpp.o ../lib/project/ProjectWriterXml.cpp

lib/support/Assert.cpp.o : \
	../lib/support/Assert.cpp
	@mkdir -p lib/support
	@echo "Compiling lib/support/Assert.cpp"
	@gcc -c -Werror -I include -I ../include -I ../third_party/gtest-1.6.0/include -I ../third_party/gtest-1.6.0 -I ../third_party/re2 -o lib/support/Assert.cpp.o ../lib/support/Assert.cpp

lib/support/CommandLine.cpp.o : \
	../lib/support/CommandLine.cpp
	@mkdir -p lib/support
	@echo "Compiling lib/support/CommandLine.cpp"
	@gcc -c -Werror -I include -I ../include -I ../third_party/gtest-1.6.0/include -I ../third_party/gtest-1.6.0 -I ../third_party/re2 -o lib/support/CommandLine.cpp.o ../lib/support/CommandLine.cpp

lib/support/Diagnostics.cpp.o : \
	../lib/support/Diagnostics.cpp
	@mkdir -p lib/support
	@echo "Compiling lib/support/Diagnostics.cpp"
	@gcc -c -Werror -I include -I ../include -I ../third_party/gtest-1.6.0/include -I ../third_party/gtest-1.6.0 -I ../third_party/re2 -o lib/support/Diagnostics.cpp.o ../lib/support/Diagnostics.cpp

lib/support/DirectoryIterator.cpp.o : \
	../lib/support/DirectoryIterator.cpp
	@mkdir -p lib/support
	@echo "Compiling lib/support/DirectoryIterator.cpp"
	@gcc -c -Werror -I include -I ../include -I ../third_party/gtest-1.6.0/include -I ../third_party/gtest-1.6.0 -I ../third_party/re2 -o lib/support/DirectoryIterator.cpp.o ../lib/support/DirectoryIterator.cpp

lib/support/GC.cpp.o : \
	../lib/support/GC.cpp
	@mkdir -p lib/support
	@echo "Compiling lib/support/GC.cpp"
	@gcc -c -Werror -I include -I ../include -I ../third_party/gtest-1.6.0/include -I ../third_party/gtest-1.6.0 -I ../third_party/re2 -o lib/support/GC.cpp.o ../lib/support/GC.cpp

lib/support/Hashing.cpp.o : \
	../lib/support/Hashing.cpp
	@mkdir -p lib/support
	@echo "Compiling lib/support/Hashing.cpp"
	@gcc -c -Werror -I include -I ../include -I ../third_party/gtest-1.6.0/include -I ../third_party/gtest-1.6.0 -I ../third_party/re2 -o lib/support/Hashing.cpp.o ../lib/support/Hashing.cpp

lib/support/OSError.cpp.o : \
	../lib/support/OSError.cpp
	@mkdir -p lib/support
	@echo "Compiling lib/support/OSError.cpp"
	@gcc -c -Werror -I include -I ../include -I ../third_party/gtest-1.6.0/include -I ../third_party/gtest-1.6.0 -I ../third_party/re2 -o lib/support/OSError.cpp.o ../lib/support/OSError.cpp

lib/support/OStream.cpp.o : \
	../lib/support/OStream.cpp
	@mkdir -p lib/support
	@echo "Compiling lib/support/OStream.cpp"
	@gcc -c -Werror -I include -I ../include -I ../third_party/gtest-1.6.0/include -I ../third_party/gtest-1.6.0 -I ../third_party/re2 -o lib/support/OStream.cpp.o ../lib/support/OStream.cpp

lib/support/Path.cpp.o : \
	../lib/support/Path.cpp
	@mkdir -p lib/support
	@echo "Compiling lib/support/Path.cpp"
	@gcc -c -Werror -I include -I ../include -I ../third_party/gtest-1.6.0/include -I ../third_party/gtest-1.6.0 -I ../third_party/re2 -o lib/support/Path.cpp.o ../lib/support/Path.cpp

lib/support/Process.cpp.o : \
	../lib/support/Process.cpp
	@mkdir -p lib/support
	@echo "Compiling lib/support/Process.cpp"
	@gcc -c -Werror -I include -I ../include -I ../third_party/gtest-1.6.0/include -I ../third_party/gtest-1.6.0 -I ../third_party/re2 -o lib/support/Process.cpp.o ../lib/support/Process.cpp

lib/support/Wildcard.cpp.o : \
	../lib/support/Wildcard.cpp
	@mkdir -p lib/support
	@echo "Compiling lib/support/Wildcard.cpp"
	@gcc -c -Werror -I include -I ../include -I ../third_party/gtest-1.6.0/include -I ../third_party/gtest-1.6.0 -I ../third_party/re2 -o lib/support/Wildcard.cpp.o ../lib/support/Wildcard.cpp

test/unit/EvaluatorTest.cpp.o : \
	../test/unit/EvaluatorTest.cpp
	@mkdir -p test/unit
	@echo "Compiling test/unit/EvaluatorTest.cpp"
	@gcc -c -Werror -I include -I ../include -I ../third_party/gtest-1.6.0/include -I ../third_party/gtest-1.6.0 -I ../third_party/re2 -o test/unit/EvaluatorTest.cpp.o ../test/unit/EvaluatorTest.cpp

test/unit/FundamentalsTest.cpp.o : \
	../test/unit/FundamentalsTest.cpp
	@mkdir -p test/unit
	@echo "Compiling test/unit/FundamentalsTest.cpp"
	@gcc -c -Werror -I include -I ../include -I ../third_party/gtest-1.6.0/include -I ../third_party/gtest-1.6.0 -I ../third_party/re2 -o test/unit/FundamentalsTest.cpp.o ../test/unit/FundamentalsTest.cpp

test/unit/LexerTest.cpp.o : \
	../test/unit/LexerTest.cpp
	@mkdir -p test/unit
	@echo "Compiling test/unit/LexerTest.cpp"
	@gcc -c -Werror -I include -I ../include -I ../third_party/gtest-1.6.0/include -I ../third_party/gtest-1.6.0 -I ../third_party/re2 -o test/unit/LexerTest.cpp.o ../test/unit/LexerTest.cpp

test/unit/OStreamTest.cpp.o : \
	../test/unit/OStreamTest.cpp
	@mkdir -p test/unit
	@echo "Compiling test/unit/OStreamTest.cpp"
	@gcc -c -Werror -I include -I ../include -I ../third_party/gtest-1.6.0/include -I ../third_party/gtest-1.6.0 -I ../third_party/re2 -o test/unit/OStreamTest.cpp.o ../test/unit/OStreamTest.cpp

test/unit/ParserTest.cpp.o : \
	../test/unit/ParserTest.cpp
	@mkdir -p test/unit
	@echo "Compiling test/unit/ParserTest.cpp"
	@gcc -c -Werror -I include -I ../include -I ../third_party/gtest-1.6.0/include -I ../third_party/gtest-1.6.0 -I ../third_party/re2 -o test/unit/ParserTest.cpp.o ../test/unit/ParserTest.cpp

test/unit/PathTest.cpp.o : \
	../test/unit/PathTest.cpp
	@mkdir -p test/unit
	@echo "Compiling test/unit/PathTest.cpp"
	@gcc -c -Werror -I include -I ../include -I ../third_party/gtest-1.6.0/include -I ../third_party/gtest-1.6.0 -I ../third_party/re2 -o test/unit/PathTest.cpp.o ../test/unit/PathTest.cpp

test/unit/SmallVectorTest.cpp.o : \
	../test/unit/SmallVectorTest.cpp
	@mkdir -p test/unit
	@echo "Compiling test/unit/SmallVectorTest.cpp"
	@gcc -c -Werror -I include -I ../include -I ../third_party/gtest-1.6.0/include -I ../third_party/gtest-1.6.0 -I ../third_party/re2 -o test/unit/SmallVectorTest.cpp.o ../test/unit/SmallVectorTest.cpp

test/unit/StringDictTest.cpp.o : \
	../test/unit/StringDictTest.cpp
	@mkdir -p test/unit
	@echo "Compiling test/unit/StringDictTest.cpp"
	@gcc -c -Werror -I include -I ../include -I ../third_party/gtest-1.6.0/include -I ../third_party/gtest-1.6.0 -I ../third_party/re2 -o test/unit/StringDictTest.cpp.o ../test/unit/StringDictTest.cpp

test/unit/StringRefTest.cpp.o : \
	../test/unit/StringRefTest.cpp
	@mkdir -p test/unit
	@echo "Compiling test/unit/StringRefTest.cpp"
	@gcc -c -Werror -I include -I ../include -I ../third_party/gtest-1.6.0/include -I ../third_party/gtest-1.6.0 -I ../third_party/re2 -o test/unit/StringRefTest.cpp.o ../test/unit/StringRefTest.cpp

test/unit/TypeRegistryTest.cpp.o : \
	../test/unit/TypeRegistryTest.cpp
	@mkdir -p test/unit
	@echo "Compiling test/unit/TypeRegistryTest.cpp"
	@gcc -c -Werror -I include -I ../include -I ../third_party/gtest-1.6.0/include -I ../third_party/gtest-1.6.0 -I ../third_party/re2 -o test/unit/TypeRegistryTest.cpp.o ../test/unit/TypeRegistryTest.cpp

test/unit/WildcardMatcherTest.cpp.o : \
	../test/unit/WildcardMatcherTest.cpp
	@mkdir -p test/unit
	@echo "Compiling test/unit/WildcardMatcherTest.cpp"
	@gcc -c -Werror -I include -I ../include -I ../third_party/gtest-1.6.0/include -I ../third_party/gtest-1.6.0 -I ../third_party/re2 -o test/unit/WildcardMatcherTest.cpp.o ../test/unit/WildcardMatcherTest.cpp

test/unit/_main.cpp.o : \
	../test/unit/_main.cpp
	@mkdir -p test/unit
	@echo "Compiling test/unit/_main.cpp"
	@gcc -c -Werror -I include -I ../include -I ../third_party/gtest-1.6.0/include -I ../third_party/gtest-1.6.0 -I ../third_party/re2 -o test/unit/_main.cpp.o ../test/unit/_main.cpp

third_party/gtest-1.6.0/src/gtest-all.cc.o : \
	../third_party/gtest-1.6.0/src/gtest-all.cc
	@mkdir -p third_party/gtest-1.6.0/src
	@echo "Compiling third_party/gtest-1.6.0/src/gtest-all.cc"
	@gcc -c -Werror -I include -I ../include -I ../third_party/gtest-1.6.0/include -I ../third_party/gtest-1.6.0 -I ../third_party/re2 -o third_party/gtest-1.6.0/src/gtest-all.cc.o ../third_party/gtest-1.6.0/src/gtest-all.cc

tools/mint/mint.cpp.o : \
	../tools/mint/mint.cpp
	@mkdir -p tools/mint
	@echo "Compiling tools/mint/mint.cpp"
	@gcc -c -Werror -I include -I ../include -I ../third_party/gtest-1.6.0/include -I ../third_party/gtest-1.6.0 -I ../third_party/re2 -o tools/mint/mint.cpp.o ../tools/mint/mint.cpp

third_party/re2/re2.a :
	$(MAKE) -C /Users/talin/Projects/mint/mint/build.osx/third_party/re2 re2.a

clean:
	${MAKE} -C /Users/talin/Projects/mint/mint/build.osx/third_party/re2 clean
	@rm -rf \
	lib/gtest.a \
	lib/mint.a \
	mint \
	test/unit/unittest \
	dist/mint-osx-0.1.0a1/bin/mint \
	dist/mint-osx-0.1.0a1/lib/mint/prelude/builders.mint \
	dist/mint-osx-0.1.0a1/lib/mint/prelude/compilers/ar.mint \
	dist/mint-osx-0.1.0a1/lib/mint/prelude/compilers/clang.mint \
	dist/mint-osx-0.1.0a1/lib/mint/prelude/compilers/compiler.mint \
	dist/mint-osx-0.1.0a1/lib/mint/prelude/compilers/gcc.mint \
	dist/mint-osx-0.1.0a1/lib/mint/prelude/compilers/ld.mint \
	dist/mint-osx-0.1.0a1/lib/mint/prelude/configtests/module.mint \
	dist/mint-osx-0.1.0a1/lib/mint/prelude/installer.mint \
	dist/mint-osx-0.1.0a1/lib/mint/prelude/module.mint \
	dist/mint-osx-0.1.0a1/lib/mint/prelude/packaging.mint \
	dist/mint-osx-0.1.0a1/lib/mint/prelude/platform/linux.mint \
	dist/mint-osx-0.1.0a1/lib/mint/prelude/platform/module.mint \
	dist/mint-osx-0.1.0a1/lib/mint/prelude/platform/osx.mint \
	dist/mint-osx-0.1.0a1/lib/mint/prelude/templates/module.mint \
	lib/build/Directory.cpp.o \
	lib/build/File.cpp.o \
	lib/build/JobMgr.cpp.o \
	lib/build/Target.cpp.o \
	lib/build/TargetFinder.cpp.o \
	lib/build/TargetMgr.cpp.o \
	lib/collections/StringRef.cpp.o \
	lib/eval/Evaluator.cpp.o \
	lib/graph/Function.cpp.o \
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
	lib/project/OptionFinder.cpp.o \
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
	lib/support/Wildcard.cpp.o \
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
	test/unit/WildcardMatcherTest.cpp.o \
	test/unit/_main.cpp.o \
	third_party/gtest-1.6.0/src/gtest-all.cc.o \
	tools/mint/mint.cpp.o

