# -----------------------------------------------------------------------------
# Definitions for the clang C/C++/Objective-C compiler
# -----------------------------------------------------------------------------

from compiler import compiler

clang = compiler {
  compile => [ "clang" ] ++
    args.cplus_flags ++
    args.include_dirs.map(x => ["-I", x]).chain() ++
    ["-o", args.outputs[0]] ++
    args.sources
}
