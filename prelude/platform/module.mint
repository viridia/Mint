# -----------------------------------------------------------------------------
# Platform-specific definitions.
# -----------------------------------------------------------------------------

if "OSX" in fundamentals.platform {
  from osx import *
}

if "LINUX" in fundamentals.platform {
  from linux import *
}

if "MSWIN" in fundamentals.platform {
  from mswin import *
}
