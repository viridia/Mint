# -----------------------------------------------------------------------------
# Installer definitions.
# -----------------------------------------------------------------------------

intall_prefix = option {
  param value : string = ""
  help = 'base directory for installation.'
}

installer = target {
  # Base directory for installation  
  param prefix : string = "/usr/local"

  # List of programs to install
  param programs : list[target]
}
