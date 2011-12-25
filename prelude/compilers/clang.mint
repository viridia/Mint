# -----------------------------------------------------------------------------
# Definitions for the clang C/C++/Objective-C compiler
# -----------------------------------------------------------------------------

from compiler import compiler, linker

clang = {
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
      command('clang',
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
  
    # Outputs
    actions => [
      message.status("Linking program ${outputs[0]}\n")
      command('clang',
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
