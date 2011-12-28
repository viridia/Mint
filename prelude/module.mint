# -----------------------------------------------------------------------------
# Standard Mint prelude
# -----------------------------------------------------------------------------

from platform import platform
from builders import delegating_builder

# -----------------------------------------------------------------------------
# Optimization level enum
# -----------------------------------------------------------------------------

#enum opt_level_enum { O1, O2, O3, O4 }

# -----------------------------------------------------------------------------
# Creates an executable from C++ or C sources.
# -----------------------------------------------------------------------------

executable = delegating_builder {
  param flags : list[string] => self.ld_flags or self.module['ld_flags']
  param linker : object => platform.linker_default.compose(
    { 'sources' = implicit_sources,
      'outputs' = abs_outputs,
      'libs' = platform.std_cplus_libs ++ self.libs
    },
    self,
    self.module)
  outputs => [ build_output_path(path.change_ext(name, platform.executable_ext)) ]
  actions => linker.actions
}

# -----------------------------------------------------------------------------
# Creates a library from C++ or C sources.
# -----------------------------------------------------------------------------

library = delegating_builder {
  param archiver : object => platform.lib_compiler_default.compose(
    { 'sources' = implicit_sources,
      'outputs' = abs_outputs,
      'libs' = platform.std_cplus_libs ++ self.libs
    },
    self,
    self.module)
  outputs => [ build_output_path(path.change_ext(name, platform.static_lib_ext)) ]
  actions => archiver.actions
}

# -----------------------------------------------------------------------------
# A fileset is a target which simply represents a set of unmodified files.
# -----------------------------------------------------------------------------

fileset = target {
  param source_dir : string => self.module.source_dir
  outputs => sources.map(src => path.join(source_dir, src))
  exclude_from_all = true
  source_only = true
}

# -----------------------------------------------------------------------------
# Base for tests.
# -----------------------------------------------------------------------------

test = target {
  exclude_from_all = true
  outputs = []
}
