# -----------------------------------------------------------------------------
# Installer definitions.
# -----------------------------------------------------------------------------

from packaging import package

# -----------------------------------------------------------------------------
# This simple installer merely copies the package contents to the appropriate
# -----------------------------------------------------------------------------

# -----------------------------------------------------------------------------
# This simple installer merely copies the package contents to the appropriate
# places within the filesystem.
# -----------------------------------------------------------------------------

installer = target {
  exclude_from_all = true

  # Helper for installing a single package
  param install_package = target {
    exclude_from_all = true
    param package : package
  }

  # Base directory for installation  
  param prefix_dir : string = ""

  # List of packages to install
  param packages : list[package]

  var installers : list[target] => packages.map(
      pkg => install_package.compose({ package = pkg }, self))
      
  implicit_depends => packages.map(p => p.contents.map(c => c.contents).merge()).merge()
  actions => [
    installers.map(inst => inst.actions)
  ]
}

# -----------------------------------------------------------------------------
# A target which creates a compressed tar archive file.
# -----------------------------------------------------------------------------

dist_archive = target {
  exclude_from_all = true

  # The name and path of the archive file, relative to the current output directory
  param output : string

  # The name of the directory where we are going to assemble the distribution files
  param staging_dir : string = "dist"

  # The name of the archive file
  param location : string => install_prefix

  # List of packages to install
  param packages : list[package]
  
  actions => [
    packages.map(pkg => 0)
  ]
  # Actions should be:
  # -- create the staging dir: ${staging_dir}
  # -- for each package create the package dir ${package.name}_${package.version}
  #      for each component in the package:
  #        calculate the component base dir as join(base, component.location)
  #        for each target in the component
  #          for each output file in the target:
  #            copy to the component dir
}
