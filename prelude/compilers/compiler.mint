# -----------------------------------------------------------------------------
# Base prototype for compilers
# -----------------------------------------------------------------------------

compiler = object {
  # List of source files
  param sources      : list[string]
  
  # List of expected output files
  param outputs      : list[string]

  # Evaluates to a list of actions to perform for compilation.
  var actions : list[action] = undefined
}

linker = object {
  # List of source files
  param sources      : list[string]
  
  # List of expected output files
  param outputs      : list[string]

  # Evaluates to a list of actions to perform for linking.
  var actions : list[action] = undefined
}
