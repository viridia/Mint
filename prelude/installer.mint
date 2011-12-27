# -----------------------------------------------------------------------------
# Installer definitions.
# -----------------------------------------------------------------------------

from packaging import package

intall_prefix = option {
  param value : string = ""
  help = 'base directory for installation.'
}

# -----------------------------------------------------------------------------
# This simple installer merely copies the package contents to the appropriate
# places within the filesystem.
# -----------------------------------------------------------------------------

installer = target {
  exclude_from_all = true

  # Base directory for installation  
  param prefix : string => install_prefix

  # List of packages to install
  param packages : list[package]
}

# -----------------------------------------------------------------------------
# A target which creates a compressed tar archive file.
# -----------------------------------------------------------------------------

dist_archive = target {
  exclude_from_all = true

  # The name and path of the archive file, relative to the current output directory
  param output : string

  # The name of the archive file
  param location : string => install_prefix

  # List of packages to install
  param packages : list[package]
}
