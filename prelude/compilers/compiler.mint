# -----------------------------------------------------------------------------
# Base prototype for compilers
# -----------------------------------------------------------------------------

compiler = object {
  # The build environent that defines all of the compiler flags and arguments.
  param args : object = undefined
  
  # Evaluates to a list of actions to perform for compilation.
  param compile : list[string] = undefined
  
  # Evaluates to a list of actions to perform for dependency scanning.
  param getdeps : list[string] = undefined
}
