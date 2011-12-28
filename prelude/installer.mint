# -----------------------------------------------------------------------------
# Installer definitions.
# -----------------------------------------------------------------------------

from packaging import package
from builders import builder, filecopy_builder

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
    cached param debug : string => package.label
    actions = []
  }

  # Base directory for installation  
  param prefix_dir : string = ""

  # List of packages to install
  param packages : list[package]

#  cached var installers : list[target] => packages.map(
#      pkg => install_package.compose({ 'package' = pkg }, self, self.module))
      
  implicit_depends => packages.map(p => p.contents.map(c => c.contents).merge()).merge()
#  actions => [
#    installers.map(inst => inst.actions)
#  ]
}

# -----------------------------------------------------------------------------
# A target which creates a compressed tar archive file.
# -----------------------------------------------------------------------------

tarball_builder = builder {
  exclude_from_all = true

  # The name and path of the archive file, relative to the current output directory
  param output : string

  # The name of the directory where we are going to assemble the distribution files
  # TODO: We ought to have some way to clear gunk out of the staging dir.
  # Maybe give a list of files that should be there, and delete any that shouldn't.
  param staging_dir : string = "dist"

  # List of packages to include in the tarball
  param packages : list[package]
  
  cached var archive_name : string => "${packages[0].label}.tgz"

  # Use filecopy builders to handle the copying of the files to the staging dir.
  implicit_depends => packages.map(
      let odir = output_dir, sdir = source_dir :
        pkg => pkg.contents.map(
            el => filecopy_builder {
              depends = el.contents
              source_dir = sdir
              output_dir = path.join(odir, staging_dir, pkg.label, el.location)
            })).merge()

  # TODO: Once the deps are done copying, use tar or whatever.
  actions => [
    message.status("Building archive file ${archive_name}\n")
  ]
}
