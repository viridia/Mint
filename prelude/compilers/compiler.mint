# -----------------------------------------------------------------------------
# Base prototype for compilers
# -----------------------------------------------------------------------------

translator = object {
  # List of source files
  param sources      : list[string]
  
  # List of expected output files
  param outputs      : list[string]

  # What the working directory will be when the program is run.
  param output_dir   : string
    
  # Evaluates to a list of actions to perform for compilation.
  var actions : list[action] = undefined
  
  # Make all of the paths in 'files' relative to the output directory.
  # All input paths must be absolute.
  def makerel(files:list[string]) -> list[string] :
      files.map(x => path.make_relative(output_dir, x))
}

compiler = translator {
}

linker = translator {
}
