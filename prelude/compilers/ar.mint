# -----------------------------------------------------------------------------
# Definitions for the 'ar' archive utility
# -----------------------------------------------------------------------------

from compiler import translator

ar = translator {
  param sources  : list[string]
  param outputs  : list[string]
  var actions : list[action] => [
    # file.remove(outputs[0])
    command('rm', ['-f'] ++ makerel(outputs))
    command('ar', ['-r'] ++ makerel(outputs) ++ makerel(sources))
  ]
}
