# -----------------------------------------------------------------------------
# Definitions for the clang C/C++/Objective-C compiler
# -----------------------------------------------------------------------------

from compiler import compiler

clang = compiler {
  # Inputs
  param flags        : list[string]
  param include_dirs : list[string]
  param library_dirs : list[string]
  param sources      : list[string]
  param outputs      : list[string]
  param source_dir   : string
  param warnings_as_errors : bool
  param all_warnings : bool

  # Outputs
  compile => [
    message.status("Compiling ${sources[0]}\n")
    command('clang',
      ['-c'] ++
      (all_warnings and [ '-Wall' ]) ++
      (warnings_as_errors and [ '-Werror' ]) ++
      flags ++
      path.join_all(source_dir, include_dirs).map(x => ['-I', x]).chain() ++
      ['-o', outputs[0]] ++
      path.join_all(source_dir, sources))
  ]
}
