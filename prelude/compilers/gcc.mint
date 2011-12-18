# -----------------------------------------------------------------------------
# Compiler definition for GCC
# -----------------------------------------------------------------------------

from compiler import compiler

gcc = compiler {
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
    command('gcc',
      ['-c'] ++
      (all_warnings and [ '-Wall' ]) ++
      (warnings_as_errors and [ '-Werror' ]) ++
      flags ++
      path.join_all(source_dir, include_dirs).map(x => ['-I', x]).merge() ++
      ['-o', outputs[0]] ++
      path.join_all(source_dir, sources))
  ]
}
