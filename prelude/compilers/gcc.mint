# -----------------------------------------------------------------------------
# Compiler definition for GCC
# -----------------------------------------------------------------------------

from compiler import compiler, linker

gcc = {
  'compiler' = compiler {
    # Inputs
    param flags        : list[string]
    param include_dirs : list[string]
    param source_dir   : string
    param warnings_as_errors : bool
    param all_warnings : bool

    # Calculate a short version of the source path
    var source_path : string => path.make_relative(source_dir, sources[0])
  
    # Outputs
    actions => [
      message.status("Compiling ${source_path}\n")
      command('gcc',
        ['-c'] ++
        (all_warnings and [ '-Wall' ]) ++
        (warnings_as_errors and [ '-Werror' ]) ++
        flags ++
        makerel(include_dirs).map(x => ['-I', x]).merge() ++
        ['-o', makerel(outputs)[0]] ++
        makerel(sources))
    ]
  },
  
  'linker' = linker {
    # Inputs
    param flags        : list[string]
    param lib_dirs     : list[string]
    param libs         : list[string]
    param warnings_as_errors : bool
    param all_warnings : bool
  
    # Calculate a short version of the output path
    var output_file : string => makerel(outputs)[0]
  
    # Outputs
    actions => [
      message.status("Linking program ${output_file}\n")
      command('gcc',
        (all_warnings and [ '-Wall' ]) ++
        (warnings_as_errors and [ '-Werror' ]) ++
        flags ++
        libs.map(x => '-l' ++ x) ++
        makerel(lib_dirs).map(x => ['-L', x]).merge() ++
        ['-o', output_file] ++
        makerel(sources))
    ]
  }
}
