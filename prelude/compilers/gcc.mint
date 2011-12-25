# -----------------------------------------------------------------------------
# Compiler definition for GCC
# -----------------------------------------------------------------------------

from compiler import compiler, linker

gcc = {
  'compiler' = compiler {
    # Inputs
    param flags        : list[string]
    param include_dirs : list[string]
    param library_dirs : list[string]
    param source_dir   : string
    param warnings_as_errors : bool
    param all_warnings : bool
  
    # Outputs
    actions => [
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
  },
  
  'linker' = linker {
    # Inputs
    param flags        : list[string]
    param lib_dirs     : list[string]
    param libs         : list[string]
    param warnings_as_errors : bool
    param all_warnings : bool
  
    # Outputs
    actions => [
      message.status("Linking program ${outputs[0]}\n")
      command('gcc',
        (all_warnings and [ '-Wall' ]) ++
        (warnings_as_errors and [ '-Werror' ]) ++
        flags ++
        libs.map(x => '-l' ++ x) ++
        lib_dirs.map(x => ['-L', x]).merge() ++
        ['-o', outputs[0]] ++
        sources)
    ]
  }
}
