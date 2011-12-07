# -----------------------------------------------------------------------------
# Compiler definition for GCC
# -----------------------------------------------------------------------------

from compiler import compiler

gcc = compiler {
  compile => [ "gcc" ] ++
    args.cplus_flags ++
    args.include_dirs.map(x => ["-I", x]).chain() ++
    ["-o", args.outputs[0]] ++
    args.sources
}
