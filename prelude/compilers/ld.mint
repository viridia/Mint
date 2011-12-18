# -----------------------------------------------------------------------------
# Definitions for the 'ld' linker
# -----------------------------------------------------------------------------

ld = object {
  param sources : list[string]
  param outputs : list[string]
  param build : list[action] => [
    message.status("Linking program ${outputs[0]}\n")
  ]
}
