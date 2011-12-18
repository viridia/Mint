# -----------------------------------------------------------------------------
# Platform-specific definitions.
# -----------------------------------------------------------------------------

if (fundamentals.platform["OSX"])
  import platform.osx as platform
else if (fundamentals.platform["LINUX"])
  import platform.linux as platform
else if (fundamentals.platform["MSWIN"])
  import platform.mswin as platform
else
  dummy = console.error("Unknown platform type")
