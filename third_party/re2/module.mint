# -----------------------------------------------------------------------------
# Mintfile for building RE2.
# -----------------------------------------------------------------------------

include_dirs = [ '.' ]

# -Wno-sign-compare
re2 = library {
  sources = [
    'util/arena.cc',
    'util/hash.cc',
    'util/rune.cc',
    'util/stringpiece.cc',
    'util/stringprintf.cc',
    'util/strutil.cc',
    'util/valgrind.cc',
    're2/bitstate.cc',
    're2/compile.cc',
    're2/dfa.cc',
    're2/filtered_re2.cc',
    're2/mimics_pcre.cc',
    're2/nfa.cc',
    're2/onepass.cc',
    're2/parse.cc',
    're2/perl_groups.cc',
    're2/prefilter.cc',
    're2/prefilter_tree.cc',
    're2/prog.cc',
    're2/re2.cc',
    're2/regexp.cc',
    're2/set.cc',
    're2/simplify.cc',
    're2/tostring.cc',
    're2/unicode_casefold.cc',
    're2/unicode_groups.cc',
  ]
  outputs = [ 're2.a' ]
}
