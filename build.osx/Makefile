# -----------------------------------------------------------------------------
# Makefile for main module
# -----------------------------------------------------------------------------

VPATH = ..

.PHONY: all clean

all: lib/mint.a lib/gtest.a mint test/unit/unittest

lib/graph/Module.cpp.o : \
	lib/graph/Module.cpp
	@echo "Compiling lib/graph/Module.cpp"
	@gcc -c -Werror -I include -I ../include -I ../third_party/gtest-1.6.0/include -I ../third_party/gtest-1.6.0 -I ../third_party/re2 -o lib/graph/Module.cpp.o ../lib/graph/Module.cpp

lib/build/JobMgr.cpp.o : \
	lib/build/JobMgr.cpp
	@echo "Compiling lib/build/JobMgr.cpp"
	@gcc -c -Werror -I include -I ../include -I ../third_party/gtest-1.6.0/include -I ../third_party/gtest-1.6.0 -I ../third_party/re2 -o lib/build/JobMgr.cpp.o ../lib/build/JobMgr.cpp

third_party/re2/re2.a :
	@rm -f re2.a
	@ar -r re2.a $^

test/unit/FundamentalsTest.cpp.o : \
	test/unit/FundamentalsTest.cpp \
	lib/mint.a \
	third_party/re2/re2.a \
	lib/gtest.a
	@echo "Compiling test/unit/FundamentalsTest.cpp"
	@gcc -c -Werror -I include -I ../include -I ../third_party/gtest-1.6.0/include -I ../third_party/gtest-1.6.0 -I ../third_party/re2 -o test/unit/FundamentalsTest.cpp.o ../test/unit/FundamentalsTest.cpp

third_party/gtest-1.6.0/src/gtest-all.cc.o : \
	third_party/gtest-1.6.0/src/gtest-all.cc
	@echo "Compiling third_party/gtest-1.6.0/src/gtest-all.cc"
	@gcc -c -Werror -I include -I ../include -I ../third_party/gtest-1.6.0/include -I ../third_party/gtest-1.6.0 -I ../third_party/re2 -o third_party/gtest-1.6.0/src/gtest-all.cc.o ../third_party/gtest-1.6.0/src/gtest-all.cc

test/unit/PathTest.cpp.o : \
	test/unit/PathTest.cpp \
	lib/mint.a \
	third_party/re2/re2.a \
	lib/gtest.a
	@echo "Compiling test/unit/PathTest.cpp"
	@gcc -c -Werror -I include -I ../include -I ../third_party/gtest-1.6.0/include -I ../third_party/gtest-1.6.0 -I ../third_party/re2 -o test/unit/PathTest.cpp.o ../test/unit/PathTest.cpp

lib/intrinsic/FileModule.cpp.o : \
	lib/intrinsic/FileModule.cpp
	@echo "Compiling lib/intrinsic/FileModule.cpp"
	@gcc -c -Werror -I include -I ../include -I ../third_party/gtest-1.6.0/include -I ../third_party/gtest-1.6.0 -I ../third_party/re2 -o lib/intrinsic/FileModule.cpp.o ../lib/intrinsic/FileModule.cpp

lib/eval/Evaluator.cpp.o : \
	lib/eval/Evaluator.cpp
	@echo "Compiling lib/eval/Evaluator.cpp"
	@gcc -c -Werror -I include -I ../include -I ../third_party/gtest-1.6.0/include -I ../third_party/gtest-1.6.0 -I ../third_party/re2 -o lib/eval/Evaluator.cpp.o ../lib/eval/Evaluator.cpp

lib/support/DirectoryIterator.cpp.o : \
	lib/support/DirectoryIterator.cpp
	@echo "Compiling lib/support/DirectoryIterator.cpp"
	@gcc -c -Werror -I include -I ../include -I ../third_party/gtest-1.6.0/include -I ../third_party/gtest-1.6.0 -I ../third_party/re2 -o lib/support/DirectoryIterator.cpp.o ../lib/support/DirectoryIterator.cpp

lib/project/ProjectWriterXml.cpp.o : \
	lib/project/ProjectWriterXml.cpp
	@echo "Compiling lib/project/ProjectWriterXml.cpp"
	@gcc -c -Werror -I include -I ../include -I ../third_party/gtest-1.6.0/include -I ../third_party/gtest-1.6.0 -I ../third_party/re2 -o lib/project/ProjectWriterXml.cpp.o ../lib/project/ProjectWriterXml.cpp

lib/intrinsic/Subprocess.cpp.o : \
	lib/intrinsic/Subprocess.cpp
	@echo "Compiling lib/intrinsic/Subprocess.cpp"
	@gcc -c -Werror -I include -I ../include -I ../third_party/gtest-1.6.0/include -I ../third_party/gtest-1.6.0 -I ../third_party/re2 -o lib/intrinsic/Subprocess.cpp.o ../lib/intrinsic/Subprocess.cpp

test/unit/TypeRegistryTest.cpp.o : \
	test/unit/TypeRegistryTest.cpp \
	lib/mint.a \
	third_party/re2/re2.a \
	lib/gtest.a
	@echo "Compiling test/unit/TypeRegistryTest.cpp"
	@gcc -c -Werror -I include -I ../include -I ../third_party/gtest-1.6.0/include -I ../third_party/gtest-1.6.0 -I ../third_party/re2 -o test/unit/TypeRegistryTest.cpp.o ../test/unit/TypeRegistryTest.cpp

test/unit/SmallVectorTest.cpp.o : \
	test/unit/SmallVectorTest.cpp \
	lib/mint.a \
	third_party/re2/re2.a \
	lib/gtest.a
	@echo "Compiling test/unit/SmallVectorTest.cpp"
	@gcc -c -Werror -I include -I ../include -I ../third_party/gtest-1.6.0/include -I ../third_party/gtest-1.6.0 -I ../third_party/re2 -o test/unit/SmallVectorTest.cpp.o ../test/unit/SmallVectorTest.cpp

lib/graph/String.cpp.o : \
	lib/graph/String.cpp
	@echo "Compiling lib/graph/String.cpp"
	@gcc -c -Werror -I include -I ../include -I ../third_party/gtest-1.6.0/include -I ../third_party/gtest-1.6.0 -I ../third_party/re2 -o lib/graph/String.cpp.o ../lib/graph/String.cpp

lib/support/OSError.cpp.o : \
	lib/support/OSError.cpp
	@echo "Compiling lib/support/OSError.cpp"
	@gcc -c -Werror -I include -I ../include -I ../third_party/gtest-1.6.0/include -I ../third_party/gtest-1.6.0 -I ../third_party/re2 -o lib/support/OSError.cpp.o ../lib/support/OSError.cpp

lib/build/Target.cpp.o : \
	lib/build/Target.cpp
	@echo "Compiling lib/build/Target.cpp"
	@gcc -c -Werror -I include -I ../include -I ../third_party/gtest-1.6.0/include -I ../third_party/gtest-1.6.0 -I ../third_party/re2 -o lib/build/Target.cpp.o ../lib/build/Target.cpp

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

lib/graph/GraphWriter.cpp.o : \
	lib/graph/GraphWriter.cpp
	@echo "Compiling lib/graph/GraphWriter.cpp"
	@gcc -c -Werror -I include -I ../include -I ../third_party/gtest-1.6.0/include -I ../third_party/gtest-1.6.0 -I ../third_party/re2 -o lib/graph/GraphWriter.cpp.o ../lib/graph/GraphWriter.cpp

lib/graph/Function.cpp.o : \
	lib/graph/Function.cpp
	@echo "Compiling lib/graph/Function.cpp"
	@gcc -c -Werror -I include -I ../include -I ../third_party/gtest-1.6.0/include -I ../third_party/gtest-1.6.0 -I ../third_party/re2 -o lib/graph/Function.cpp.o ../lib/graph/Function.cpp

lib/intrinsic/ListType.cpp.o : \
	lib/intrinsic/ListType.cpp
	@echo "Compiling lib/intrinsic/ListType.cpp"
	@gcc -c -Werror -I include -I ../include -I ../third_party/gtest-1.6.0/include -I ../third_party/gtest-1.6.0 -I ../third_party/re2 -o lib/intrinsic/ListType.cpp.o ../lib/intrinsic/ListType.cpp

lib/graph/Oper.cpp.o : \
	lib/graph/Oper.cpp
	@echo "Compiling lib/graph/Oper.cpp"
	@gcc -c -Werror -I include -I ../include -I ../third_party/gtest-1.6.0/include -I ../third_party/gtest-1.6.0 -I ../third_party/re2 -o lib/graph/Oper.cpp.o ../lib/graph/Oper.cpp

lib/intrinsic/Console.cpp.o : \
	lib/intrinsic/Console.cpp
	@echo "Compiling lib/intrinsic/Console.cpp"
	@gcc -c -Werror -I include -I ../include -I ../third_party/gtest-1.6.0/include -I ../third_party/gtest-1.6.0 -I ../third_party/re2 -o lib/intrinsic/Console.cpp.o ../lib/intrinsic/Console.cpp

test/unit/ParserTest.cpp.o : \
	test/unit/ParserTest.cpp \
	lib/mint.a \
	third_party/re2/re2.a \
	lib/gtest.a
	@echo "Compiling test/unit/ParserTest.cpp"
	@gcc -c -Werror -I include -I ../include -I ../third_party/gtest-1.6.0/include -I ../third_party/gtest-1.6.0 -I ../third_party/re2 -o test/unit/ParserTest.cpp.o ../test/unit/ParserTest.cpp

lib/gtest.a : \
	third_party/gtest-1.6.0/src/gtest-all.cc.o
	@rm -f lib/gtest.a
	@ar -r lib/gtest.a $^

lib/build/Directory.cpp.o : \
	lib/build/Directory.cpp
	@echo "Compiling lib/build/Directory.cpp"
	@gcc -c -Werror -I include -I ../include -I ../third_party/gtest-1.6.0/include -I ../third_party/gtest-1.6.0 -I ../third_party/re2 -o lib/build/Directory.cpp.o ../lib/build/Directory.cpp

lib/support/Path.cpp.o : \
	lib/support/Path.cpp
	@echo "Compiling lib/support/Path.cpp"
	@gcc -c -Werror -I include -I ../include -I ../third_party/gtest-1.6.0/include -I ../third_party/gtest-1.6.0 -I ../third_party/re2 -o lib/support/Path.cpp.o ../lib/support/Path.cpp

tools/mint/mint.cpp.o : \
	tools/mint/mint.cpp \
	lib/mint.a \
	third_party/re2/re2.a
	@echo "Compiling tools/mint/mint.cpp"
	@gcc -c -Werror -I include -I ../include -I ../third_party/gtest-1.6.0/include -I ../third_party/gtest-1.6.0 -I ../third_party/re2 -o tools/mint/mint.cpp.o ../tools/mint/mint.cpp

lib/intrinsic/ModuleType.cpp.o : \
	lib/intrinsic/ModuleType.cpp
	@echo "Compiling lib/intrinsic/ModuleType.cpp"
	@gcc -c -Werror -I include -I ../include -I ../third_party/gtest-1.6.0/include -I ../third_party/gtest-1.6.0 -I ../third_party/re2 -o lib/intrinsic/ModuleType.cpp.o ../lib/intrinsic/ModuleType.cpp

lib/project/Project.cpp.o : \
	lib/project/Project.cpp
	@echo "Compiling lib/project/Project.cpp"
	@gcc -c -Werror -I include -I ../include -I ../third_party/gtest-1.6.0/include -I ../third_party/gtest-1.6.0 -I ../third_party/re2 -o lib/project/Project.cpp.o ../lib/project/Project.cpp

lib/build/TargetFinder.cpp.o : \
	lib/build/TargetFinder.cpp
	@echo "Compiling lib/build/TargetFinder.cpp"
	@gcc -c -Werror -I include -I ../include -I ../third_party/gtest-1.6.0/include -I ../third_party/gtest-1.6.0 -I ../third_party/re2 -o lib/build/TargetFinder.cpp.o ../lib/build/TargetFinder.cpp

lib/support/OStream.cpp.o : \
	lib/support/OStream.cpp
	@echo "Compiling lib/support/OStream.cpp"
	@gcc -c -Werror -I include -I ../include -I ../third_party/gtest-1.6.0/include -I ../third_party/gtest-1.6.0 -I ../third_party/re2 -o lib/support/OStream.cpp.o ../lib/support/OStream.cpp

lib/support/Diagnostics.cpp.o : \
	lib/support/Diagnostics.cpp
	@echo "Compiling lib/support/Diagnostics.cpp"
	@gcc -c -Werror -I include -I ../include -I ../third_party/gtest-1.6.0/include -I ../third_party/gtest-1.6.0 -I ../third_party/re2 -o lib/support/Diagnostics.cpp.o ../lib/support/Diagnostics.cpp

lib/lex/Lexer.cpp.o : \
	lib/lex/Lexer.cpp
	@echo "Compiling lib/lex/Lexer.cpp"
	@gcc -c -Werror -I include -I ../include -I ../third_party/gtest-1.6.0/include -I ../third_party/gtest-1.6.0 -I ../third_party/re2 -o lib/lex/Lexer.cpp.o ../lib/lex/Lexer.cpp

lib/graph/GraphBuilder.cpp.o : \
	lib/graph/GraphBuilder.cpp
	@echo "Compiling lib/graph/GraphBuilder.cpp"
	@gcc -c -Werror -I include -I ../include -I ../third_party/gtest-1.6.0/include -I ../third_party/gtest-1.6.0 -I ../third_party/re2 -o lib/graph/GraphBuilder.cpp.o ../lib/graph/GraphBuilder.cpp

test/unit/StringRefTest.cpp.o : \
	test/unit/StringRefTest.cpp \
	lib/mint.a \
	third_party/re2/re2.a \
	lib/gtest.a
	@echo "Compiling test/unit/StringRefTest.cpp"
	@gcc -c -Werror -I include -I ../include -I ../third_party/gtest-1.6.0/include -I ../third_party/gtest-1.6.0 -I ../third_party/re2 -o test/unit/StringRefTest.cpp.o ../test/unit/StringRefTest.cpp

lib/graph/Object.cpp.o : \
	lib/graph/Object.cpp
	@echo "Compiling lib/graph/Object.cpp"
	@gcc -c -Werror -I include -I ../include -I ../third_party/gtest-1.6.0/include -I ../third_party/gtest-1.6.0 -I ../third_party/re2 -o lib/graph/Object.cpp.o ../lib/graph/Object.cpp

mint : \
	lib/mint.a \
	third_party/re2/re2.a \
	tools/mint/mint.cpp.o
	@echo "Linking program mint"
	@gcc -Werror -lstdc++ -o mint $^

lib/graph/TypeRegistry.cpp.o : \
	lib/graph/TypeRegistry.cpp
	@echo "Compiling lib/graph/TypeRegistry.cpp"
	@gcc -c -Werror -I include -I ../include -I ../third_party/gtest-1.6.0/include -I ../third_party/gtest-1.6.0 -I ../third_party/re2 -o lib/graph/TypeRegistry.cpp.o ../lib/graph/TypeRegistry.cpp

lib/build/TargetMgr.cpp.o : \
	lib/build/TargetMgr.cpp
	@echo "Compiling lib/build/TargetMgr.cpp"
	@gcc -c -Werror -I include -I ../include -I ../third_party/gtest-1.6.0/include -I ../third_party/gtest-1.6.0 -I ../third_party/re2 -o lib/build/TargetMgr.cpp.o ../lib/build/TargetMgr.cpp

lib/project/ModuleLoader.cpp.o : \
	lib/project/ModuleLoader.cpp
	@echo "Compiling lib/project/ModuleLoader.cpp"
	@gcc -c -Werror -I include -I ../include -I ../third_party/gtest-1.6.0/include -I ../third_party/gtest-1.6.0 -I ../third_party/re2 -o lib/project/ModuleLoader.cpp.o ../lib/project/ModuleLoader.cpp

test/unit/_main.cpp.o : \
	test/unit/_main.cpp \
	lib/mint.a \
	third_party/re2/re2.a \
	lib/gtest.a
	@echo "Compiling test/unit/_main.cpp"
	@gcc -c -Werror -I include -I ../include -I ../third_party/gtest-1.6.0/include -I ../third_party/gtest-1.6.0 -I ../third_party/re2 -o test/unit/_main.cpp.o ../test/unit/_main.cpp

lib/support/Process.cpp.o : \
	lib/support/Process.cpp
	@echo "Compiling lib/support/Process.cpp"
	@gcc -c -Werror -I include -I ../include -I ../third_party/gtest-1.6.0/include -I ../third_party/gtest-1.6.0 -I ../third_party/re2 -o lib/support/Process.cpp.o ../lib/support/Process.cpp

lib/intrinsic/RegEx.cpp.o : \
	lib/intrinsic/RegEx.cpp
	@echo "Compiling lib/intrinsic/RegEx.cpp"
	@gcc -c -Werror -I include -I ../include -I ../third_party/gtest-1.6.0/include -I ../third_party/gtest-1.6.0 -I ../third_party/re2 -o lib/intrinsic/RegEx.cpp.o ../lib/intrinsic/RegEx.cpp

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

test/unit/OStreamTest.cpp.o : \
	test/unit/OStreamTest.cpp \
	lib/mint.a \
	third_party/re2/re2.a \
	lib/gtest.a
	@echo "Compiling test/unit/OStreamTest.cpp"
	@gcc -c -Werror -I include -I ../include -I ../third_party/gtest-1.6.0/include -I ../third_party/gtest-1.6.0 -I ../third_party/re2 -o test/unit/OStreamTest.cpp.o ../test/unit/OStreamTest.cpp

lib/build/File.cpp.o : \
	lib/build/File.cpp
	@echo "Compiling lib/build/File.cpp"
	@gcc -c -Werror -I include -I ../include -I ../third_party/gtest-1.6.0/include -I ../third_party/gtest-1.6.0 -I ../third_party/re2 -o lib/build/File.cpp.o ../lib/build/File.cpp

test/unit/StringDictTest.cpp.o : \
	test/unit/StringDictTest.cpp \
	lib/mint.a \
	third_party/re2/re2.a \
	lib/gtest.a
	@echo "Compiling test/unit/StringDictTest.cpp"
	@gcc -c -Werror -I include -I ../include -I ../third_party/gtest-1.6.0/include -I ../third_party/gtest-1.6.0 -I ../third_party/re2 -o test/unit/StringDictTest.cpp.o ../test/unit/StringDictTest.cpp

lib/intrinsic/PathMethods.cpp.o : \
	lib/intrinsic/PathMethods.cpp
	@echo "Compiling lib/intrinsic/PathMethods.cpp"
	@gcc -c -Werror -I include -I ../include -I ../third_party/gtest-1.6.0/include -I ../third_party/gtest-1.6.0 -I ../third_party/re2 -o lib/intrinsic/PathMethods.cpp.o ../lib/intrinsic/PathMethods.cpp

lib/support/CommandLine.cpp.o : \
	lib/support/CommandLine.cpp
	@echo "Compiling lib/support/CommandLine.cpp"
	@gcc -c -Werror -I include -I ../include -I ../third_party/gtest-1.6.0/include -I ../third_party/gtest-1.6.0 -I ../third_party/re2 -o lib/support/CommandLine.cpp.o ../lib/support/CommandLine.cpp

lib/graph/Node.cpp.o : \
	lib/graph/Node.cpp
	@echo "Compiling lib/graph/Node.cpp"
	@gcc -c -Werror -I include -I ../include -I ../third_party/gtest-1.6.0/include -I ../third_party/gtest-1.6.0 -I ../third_party/re2 -o lib/graph/Node.cpp.o ../lib/graph/Node.cpp

lib/parse/Parser.cpp.o : \
	lib/parse/Parser.cpp
	@echo "Compiling lib/parse/Parser.cpp"
	@gcc -c -Werror -I include -I ../include -I ../third_party/gtest-1.6.0/include -I ../third_party/gtest-1.6.0 -I ../third_party/re2 -o lib/parse/Parser.cpp.o ../lib/parse/Parser.cpp

lib/support/GC.cpp.o : \
	lib/support/GC.cpp
	@echo "Compiling lib/support/GC.cpp"
	@gcc -c -Werror -I include -I ../include -I ../third_party/gtest-1.6.0/include -I ../third_party/gtest-1.6.0 -I ../third_party/re2 -o lib/support/GC.cpp.o ../lib/support/GC.cpp

lib/project/MakefileGenerator.cpp.o : \
	lib/project/MakefileGenerator.cpp
	@echo "Compiling lib/project/MakefileGenerator.cpp"
	@gcc -c -Werror -I include -I ../include -I ../third_party/gtest-1.6.0/include -I ../third_party/gtest-1.6.0 -I ../third_party/re2 -o lib/project/MakefileGenerator.cpp.o ../lib/project/MakefileGenerator.cpp

test/unit/LexerTest.cpp.o : \
	test/unit/LexerTest.cpp \
	lib/mint.a \
	third_party/re2/re2.a \
	lib/gtest.a
	@echo "Compiling test/unit/LexerTest.cpp"
	@gcc -c -Werror -I include -I ../include -I ../third_party/gtest-1.6.0/include -I ../third_party/gtest-1.6.0 -I ../third_party/re2 -o test/unit/LexerTest.cpp.o ../test/unit/LexerTest.cpp

test/unit/EvaluatorTest.cpp.o : \
	test/unit/EvaluatorTest.cpp \
	lib/mint.a \
	third_party/re2/re2.a \
	lib/gtest.a
	@echo "Compiling test/unit/EvaluatorTest.cpp"
	@gcc -c -Werror -I include -I ../include -I ../third_party/gtest-1.6.0/include -I ../third_party/gtest-1.6.0 -I ../third_party/re2 -o test/unit/EvaluatorTest.cpp.o ../test/unit/EvaluatorTest.cpp

lib/intrinsic/FileSystem.cpp.o : \
	lib/intrinsic/FileSystem.cpp
	@echo "Compiling lib/intrinsic/FileSystem.cpp"
	@gcc -c -Werror -I include -I ../include -I ../third_party/gtest-1.6.0/include -I ../third_party/gtest-1.6.0 -I ../third_party/re2 -o lib/intrinsic/FileSystem.cpp.o ../lib/intrinsic/FileSystem.cpp

lib/project/BuildConfiguration.cpp.o : \
	lib/project/BuildConfiguration.cpp
	@echo "Compiling lib/project/BuildConfiguration.cpp"
	@gcc -c -Werror -I include -I ../include -I ../third_party/gtest-1.6.0/include -I ../third_party/gtest-1.6.0 -I ../third_party/re2 -o lib/project/BuildConfiguration.cpp.o ../lib/project/BuildConfiguration.cpp

lib/collections/StringRef.cpp.o : \
	lib/collections/StringRef.cpp
	@echo "Compiling lib/collections/StringRef.cpp"
	@gcc -c -Werror -I include -I ../include -I ../third_party/gtest-1.6.0/include -I ../third_party/gtest-1.6.0 -I ../third_party/re2 -o lib/collections/StringRef.cpp.o ../lib/collections/StringRef.cpp

test/unit/WildcardMatcherTest.cpp.o : \
	test/unit/WildcardMatcherTest.cpp \
	lib/mint.a \
	third_party/re2/re2.a \
	lib/gtest.a
	@echo "Compiling test/unit/WildcardMatcherTest.cpp"
	@gcc -c -Werror -I include -I ../include -I ../third_party/gtest-1.6.0/include -I ../third_party/gtest-1.6.0 -I ../third_party/re2 -o test/unit/WildcardMatcherTest.cpp.o ../test/unit/WildcardMatcherTest.cpp

lib/intrinsic/StringRegistry.cpp.o : \
	lib/intrinsic/StringRegistry.cpp
	@echo "Compiling lib/intrinsic/StringRegistry.cpp"
	@gcc -c -Werror -I include -I ../include -I ../third_party/gtest-1.6.0/include -I ../third_party/gtest-1.6.0 -I ../third_party/re2 -o lib/intrinsic/StringRegistry.cpp.o ../lib/intrinsic/StringRegistry.cpp

lib/project/Configurator.cpp.o : \
	lib/project/Configurator.cpp
	@echo "Compiling lib/project/Configurator.cpp"
	@gcc -c -Werror -I include -I ../include -I ../third_party/gtest-1.6.0/include -I ../third_party/gtest-1.6.0 -I ../third_party/re2 -o lib/project/Configurator.cpp.o ../lib/project/Configurator.cpp

lib/support/Wildcard.cpp.o : \
	lib/support/Wildcard.cpp
	@echo "Compiling lib/support/Wildcard.cpp"
	@gcc -c -Werror -I include -I ../include -I ../third_party/gtest-1.6.0/include -I ../third_party/gtest-1.6.0 -I ../third_party/re2 -o lib/support/Wildcard.cpp.o ../lib/support/Wildcard.cpp

lib/support/Assert.cpp.o : \
	lib/support/Assert.cpp
	@echo "Compiling lib/support/Assert.cpp"
	@gcc -c -Werror -I include -I ../include -I ../third_party/gtest-1.6.0/include -I ../third_party/gtest-1.6.0 -I ../third_party/re2 -o lib/support/Assert.cpp.o ../lib/support/Assert.cpp

lib/intrinsic/Fundamentals.cpp.o : \
	lib/intrinsic/Fundamentals.cpp
	@echo "Compiling lib/intrinsic/Fundamentals.cpp"
	@gcc -c -Werror -I include -I ../include -I ../third_party/gtest-1.6.0/include -I ../third_party/gtest-1.6.0 -I ../third_party/re2 -o lib/intrinsic/Fundamentals.cpp.o ../lib/intrinsic/Fundamentals.cpp

lib/support/Hashing.cpp.o : \
	lib/support/Hashing.cpp
	@echo "Compiling lib/support/Hashing.cpp"
	@gcc -c -Werror -I include -I ../include -I ../third_party/gtest-1.6.0/include -I ../third_party/gtest-1.6.0 -I ../third_party/re2 -o lib/support/Hashing.cpp.o ../lib/support/Hashing.cpp

lib/graph/Type.cpp.o : \
	lib/graph/Type.cpp
	@echo "Compiling lib/graph/Type.cpp"
	@gcc -c -Werror -I include -I ../include -I ../third_party/gtest-1.6.0/include -I ../third_party/gtest-1.6.0 -I ../third_party/re2 -o lib/graph/Type.cpp.o ../lib/graph/Type.cpp

clean:
	@rm -rf  \
	lib/graph/Module.cpp.o \
	lib/build/JobMgr.cpp.o \
	third_party/re2/re2.a \
	test/unit/FundamentalsTest.cpp.o \
	third_party/gtest-1.6.0/src/gtest-all.cc.o \
	test/unit/PathTest.cpp.o \
	lib/intrinsic/FileModule.cpp.o \
	lib/eval/Evaluator.cpp.o \
	lib/support/DirectoryIterator.cpp.o \
	lib/project/ProjectWriterXml.cpp.o \
	lib/intrinsic/Subprocess.cpp.o \
	test/unit/TypeRegistryTest.cpp.o \
	test/unit/SmallVectorTest.cpp.o \
	lib/graph/String.cpp.o \
	lib/support/OSError.cpp.o \
	lib/build/Target.cpp.o \
	lib/mint.a \
	lib/graph/GraphWriter.cpp.o \
	lib/graph/Function.cpp.o \
	lib/intrinsic/ListType.cpp.o \
	lib/graph/Oper.cpp.o \
	lib/intrinsic/Console.cpp.o \
	test/unit/ParserTest.cpp.o \
	lib/gtest.a \
	lib/build/Directory.cpp.o \
	lib/support/Path.cpp.o \
	tools/mint/mint.cpp.o \
	lib/intrinsic/ModuleType.cpp.o \
	lib/project/Project.cpp.o \
	lib/build/TargetFinder.cpp.o \
	lib/support/OStream.cpp.o \
	lib/support/Diagnostics.cpp.o \
	lib/lex/Lexer.cpp.o \
	lib/graph/GraphBuilder.cpp.o \
	test/unit/StringRefTest.cpp.o \
	lib/graph/Object.cpp.o \
	mint \
	lib/graph/TypeRegistry.cpp.o \
	lib/build/TargetMgr.cpp.o \
	lib/project/ModuleLoader.cpp.o \
	test/unit/_main.cpp.o \
	lib/support/Process.cpp.o \
	lib/intrinsic/RegEx.cpp.o \
	test/unit/unittest \
	test/unit/OStreamTest.cpp.o \
	lib/build/File.cpp.o \
	test/unit/StringDictTest.cpp.o \
	lib/intrinsic/PathMethods.cpp.o \
	lib/support/CommandLine.cpp.o \
	lib/graph/Node.cpp.o \
	lib/parse/Parser.cpp.o \
	lib/support/GC.cpp.o \
	lib/project/MakefileGenerator.cpp.o \
	test/unit/LexerTest.cpp.o \
	test/unit/EvaluatorTest.cpp.o \
	lib/intrinsic/FileSystem.cpp.o \
	lib/project/BuildConfiguration.cpp.o \
	lib/collections/StringRef.cpp.o \
	test/unit/WildcardMatcherTest.cpp.o \
	lib/intrinsic/StringRegistry.cpp.o \
	lib/project/Configurator.cpp.o \
	lib/support/Wildcard.cpp.o \
	lib/support/Assert.cpp.o \
	lib/intrinsic/Fundamentals.cpp.o \
	lib/support/Hashing.cpp.o \
	lib/graph/Type.cpp.o

