# -----------------------------------------------------------------------------
# Compiler definition for GCC
# -----------------------------------------------------------------------------

from compiler import compiler, linker, gendeps

gcc = {
  'compiler' = compiler {
    # Inputs
    param flags        : list[string]
    param include_dirs : list[string]
    param source_dir   : string
    param warnings_as_errors : bool
    param all_warnings : bool
    param program : string = 'gcc'
    param source_language : string = undefined
    param preprocess_only : bool = false

    # Calculate a short version of the source path
    var source_path : string => path.make_relative(source_dir, sources[0])
    
    # Table of source language names
    var source_languages = {
      'c' = '-xc',
      'c++' = '-xc++',
      'cplus' = '-xc++',
    }
  
    # Actions
    actions => [
      message.status("Compiling ${source_path}\n")
      command(program,
        make_arglist(
          if (preprocess_only) '-E' else '-c',
          all_warnings and '-Wall',
          warnings_as_errors and '-Werror',
          source_languages[source_language]) ++
        flags ++
        makerel(include_dirs).map(x => ['-I', x]).merge() ++
        ['-o', makerel(outputs)[0]] ++
        makerel(sources))
    ]

    # Method used when compiling for a configuration test
    def check_compile(input:string) -> object :
      shell(program,
        make_arglist(
            preprocess_only and '-E',
            all_warnings and '-Wall',
            warnings_as_errors and '-Werror',
            source_languages[source_language]) ++
          flags ++
          makerel(include_dirs or []).map(x => ['-I', x]).merge() ++
          ['-o', outputs[0], '-', '2>', '/dev/null', '1>', '/dev/null'],
        input)
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
  
    # Actions
    actions => [
      message.status("Linking program ${output_file}\n")
      command(program,
        (all_warnings and [ '-Wall' ]) ++
        (warnings_as_errors and [ '-Werror' ]) ++
        flags ++
        libs.map(x => '-l' ++ x) ++
        makerel(lib_dirs).map(x => ['-L', x]).merge() ++
        ['-o', output_file] ++
        makerel(sources))
    ]
  },
  
  'gendeps' = gendeps {
    # Inputs
    param flags        : list[string]
    param include_dirs : list[string]
    param source_dir   : string

    # Calculate a short version of the source path
    var source_path : string => path.make_relative(source_dir, sources[0])
  
    # Actions
    actions => [
      message.status("Generating dependencies for ${source_dir}\n")
      command(program,
        ['-c'] ++
        flags ++
        makerel(include_dirs).map(x => ['-I', x]).merge() ++
        ['-o', makerel(outputs)[0]] ++
        makerel(sources))
    ]
  },
}
