# -----------------------------------------------------------------------------
# Base prototype for compilers
# -----------------------------------------------------------------------------

compiler = object {
  # Evaluates to a list of actions to perform for compilation.
  param compile : list[string] = undefined
  
  # Evaluates to a list of actions to perform for dependency scanning.
  param gendeps : list[string] = undefined
}
