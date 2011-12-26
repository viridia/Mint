# -----------------------------------------------------------------------------
# Makefile for third_party/re2
# -----------------------------------------------------------------------------

VPATH = ../../../third_party/re2

.PHONY: all clean

all: re2.a

re2.a : \
	util/arena.cc.o \
	util/hash.cc.o \
	util/rune.cc.o \
	util/stringpiece.cc.o \
	util/stringprintf.cc.o \
	util/strutil.cc.o \
	util/valgrind.cc.o \
	re2/bitstate.cc.o \
	re2/compile.cc.o \
	re2/dfa.cc.o \
	re2/filtered_re2.cc.o \
	re2/mimics_pcre.cc.o \
	re2/nfa.cc.o \
	re2/onepass.cc.o \
	re2/parse.cc.o \
	re2/perl_groups.cc.o \
	re2/prefilter.cc.o \
	re2/prefilter_tree.cc.o \
	re2/prog.cc.o \
	re2/re2.cc.o \
	re2/regexp.cc.o \
	re2/set.cc.o \
	re2/simplify.cc.o \
	re2/tostring.cc.o \
	re2/unicode_casefold.cc.o \
	re2/unicode_groups.cc.o
	@rm -f re2.a
	@ar -r re2.a $^

re2/bitstate.cc.o : \
	re2/bitstate.cc
	@echo "Compiling re2/bitstate.cc"
	@gcc -c -I ../../../third_party/re2 -o re2/bitstate.cc.o ../../../third_party/re2/re2/bitstate.cc

re2/compile.cc.o : \
	re2/compile.cc
	@echo "Compiling re2/compile.cc"
	@gcc -c -I ../../../third_party/re2 -o re2/compile.cc.o ../../../third_party/re2/re2/compile.cc

re2/dfa.cc.o : \
	re2/dfa.cc
	@echo "Compiling re2/dfa.cc"
	@gcc -c -I ../../../third_party/re2 -o re2/dfa.cc.o ../../../third_party/re2/re2/dfa.cc

re2/filtered_re2.cc.o : \
	re2/filtered_re2.cc
	@echo "Compiling re2/filtered_re2.cc"
	@gcc -c -I ../../../third_party/re2 -o re2/filtered_re2.cc.o ../../../third_party/re2/re2/filtered_re2.cc

re2/mimics_pcre.cc.o : \
	re2/mimics_pcre.cc
	@echo "Compiling re2/mimics_pcre.cc"
	@gcc -c -I ../../../third_party/re2 -o re2/mimics_pcre.cc.o ../../../third_party/re2/re2/mimics_pcre.cc

re2/nfa.cc.o : \
	re2/nfa.cc
	@echo "Compiling re2/nfa.cc"
	@gcc -c -I ../../../third_party/re2 -o re2/nfa.cc.o ../../../third_party/re2/re2/nfa.cc

re2/onepass.cc.o : \
	re2/onepass.cc
	@echo "Compiling re2/onepass.cc"
	@gcc -c -I ../../../third_party/re2 -o re2/onepass.cc.o ../../../third_party/re2/re2/onepass.cc

re2/parse.cc.o : \
	re2/parse.cc
	@echo "Compiling re2/parse.cc"
	@gcc -c -I ../../../third_party/re2 -o re2/parse.cc.o ../../../third_party/re2/re2/parse.cc

re2/perl_groups.cc.o : \
	re2/perl_groups.cc
	@echo "Compiling re2/perl_groups.cc"
	@gcc -c -I ../../../third_party/re2 -o re2/perl_groups.cc.o ../../../third_party/re2/re2/perl_groups.cc

re2/prefilter.cc.o : \
	re2/prefilter.cc
	@echo "Compiling re2/prefilter.cc"
	@gcc -c -I ../../../third_party/re2 -o re2/prefilter.cc.o ../../../third_party/re2/re2/prefilter.cc

re2/prefilter_tree.cc.o : \
	re2/prefilter_tree.cc
	@echo "Compiling re2/prefilter_tree.cc"
	@gcc -c -I ../../../third_party/re2 -o re2/prefilter_tree.cc.o ../../../third_party/re2/re2/prefilter_tree.cc

re2/prog.cc.o : \
	re2/prog.cc
	@echo "Compiling re2/prog.cc"
	@gcc -c -I ../../../third_party/re2 -o re2/prog.cc.o ../../../third_party/re2/re2/prog.cc

re2/re2.cc.o : \
	re2/re2.cc
	@echo "Compiling re2/re2.cc"
	@gcc -c -I ../../../third_party/re2 -o re2/re2.cc.o ../../../third_party/re2/re2/re2.cc

re2/regexp.cc.o : \
	re2/regexp.cc
	@echo "Compiling re2/regexp.cc"
	@gcc -c -I ../../../third_party/re2 -o re2/regexp.cc.o ../../../third_party/re2/re2/regexp.cc

re2/set.cc.o : \
	re2/set.cc
	@echo "Compiling re2/set.cc"
	@gcc -c -I ../../../third_party/re2 -o re2/set.cc.o ../../../third_party/re2/re2/set.cc

re2/simplify.cc.o : \
	re2/simplify.cc
	@echo "Compiling re2/simplify.cc"
	@gcc -c -I ../../../third_party/re2 -o re2/simplify.cc.o ../../../third_party/re2/re2/simplify.cc

re2/tostring.cc.o : \
	re2/tostring.cc
	@echo "Compiling re2/tostring.cc"
	@gcc -c -I ../../../third_party/re2 -o re2/tostring.cc.o ../../../third_party/re2/re2/tostring.cc

re2/unicode_casefold.cc.o : \
	re2/unicode_casefold.cc
	@echo "Compiling re2/unicode_casefold.cc"
	@gcc -c -I ../../../third_party/re2 -o re2/unicode_casefold.cc.o ../../../third_party/re2/re2/unicode_casefold.cc

re2/unicode_groups.cc.o : \
	re2/unicode_groups.cc
	@echo "Compiling re2/unicode_groups.cc"
	@gcc -c -I ../../../third_party/re2 -o re2/unicode_groups.cc.o ../../../third_party/re2/re2/unicode_groups.cc

util/arena.cc.o : \
	util/arena.cc
	@echo "Compiling util/arena.cc"
	@gcc -c -I ../../../third_party/re2 -o util/arena.cc.o ../../../third_party/re2/util/arena.cc

util/hash.cc.o : \
	util/hash.cc
	@echo "Compiling util/hash.cc"
	@gcc -c -I ../../../third_party/re2 -o util/hash.cc.o ../../../third_party/re2/util/hash.cc

util/rune.cc.o : \
	util/rune.cc
	@echo "Compiling util/rune.cc"
	@gcc -c -I ../../../third_party/re2 -o util/rune.cc.o ../../../third_party/re2/util/rune.cc

util/stringpiece.cc.o : \
	util/stringpiece.cc
	@echo "Compiling util/stringpiece.cc"
	@gcc -c -I ../../../third_party/re2 -o util/stringpiece.cc.o ../../../third_party/re2/util/stringpiece.cc

util/stringprintf.cc.o : \
	util/stringprintf.cc
	@echo "Compiling util/stringprintf.cc"
	@gcc -c -I ../../../third_party/re2 -o util/stringprintf.cc.o ../../../third_party/re2/util/stringprintf.cc

util/strutil.cc.o : \
	util/strutil.cc
	@echo "Compiling util/strutil.cc"
	@gcc -c -I ../../../third_party/re2 -o util/strutil.cc.o ../../../third_party/re2/util/strutil.cc

util/valgrind.cc.o : \
	util/valgrind.cc
	@echo "Compiling util/valgrind.cc"
	@gcc -c -I ../../../third_party/re2 -o util/valgrind.cc.o ../../../third_party/re2/util/valgrind.cc

clean:
	@rm -rf \
	re2.a \
	re2/bitstate.cc.o \
	re2/compile.cc.o \
	re2/dfa.cc.o \
	re2/filtered_re2.cc.o \
	re2/mimics_pcre.cc.o \
	re2/nfa.cc.o \
	re2/onepass.cc.o \
	re2/parse.cc.o \
	re2/perl_groups.cc.o \
	re2/prefilter.cc.o \
	re2/prefilter_tree.cc.o \
	re2/prog.cc.o \
	re2/re2.cc.o \
	re2/regexp.cc.o \
	re2/set.cc.o \
	re2/simplify.cc.o \
	re2/tostring.cc.o \
	re2/unicode_casefold.cc.o \
	re2/unicode_groups.cc.o \
	util/arena.cc.o \
	util/hash.cc.o \
	util/rune.cc.o \
	util/stringpiece.cc.o \
	util/stringprintf.cc.o \
	util/strutil.cc.o \
	util/valgrind.cc.o

