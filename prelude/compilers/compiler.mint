# -----------------------------------------------------------------------------
# Base prototype for compilers
# -----------------------------------------------------------------------------

compiler = object {
  # Evaluates to a list of actions to perform for compilation.
  var actions : list[action] = undefined
}

linker = object {
  # Evaluates to a list of actions to perform for linking.
  var actions : list[action] = undefined
}
