# -----------------------------------------------------------------------------
# Definitions for the 'ar' archive utility
# -----------------------------------------------------------------------------

ar = object {
  param sources  : list[string]
  param outputs  : list[string]
  var actions : list[action] => [
    # file.remove(outputs[0])
    command('rm', ['-f'] ++ outputs)
    command('ar', ['-r'] ++ outputs ++ sources)
  ]
}
