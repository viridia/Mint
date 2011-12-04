# Standard Mint prelude

# -----------------------------------------------------------------------------
# Optimization level enum
# -----------------------------------------------------------------------------

#enum opt_level_enum { O1, O2, O3, O4 }

# -----------------------------------------------------------------------------
# Header file scanner for C source files
# -----------------------------------------------------------------------------

#c_scanner = scanner {
#  actions = [
#    "gcc",
#    target.cflags,
##    target.include_dirs.map(x => ["-I", x])
#    sources
#  ]
#}

# -----------------------------------------------------------------------------
# Base for targets that produce files.
# -----------------------------------------------------------------------------

builder = target {
  export param actions : list[any] = []
}

# -----------------------------------------------------------------------------
# Builder that does nothing - has no output
# -----------------------------------------------------------------------------

null_builder = builder {
}

# -----------------------------------------------------------------------------
# Builder that does nothing - it's output is just it's input
# -----------------------------------------------------------------------------

identity_builder = builder {
  outputs => sources
}

# -----------------------------------------------------------------------------
# Builder for C source files
# -----------------------------------------------------------------------------

c_builder = builder {
  param env : any => self.module
  param c_flags : list[string] => env['cflags'] or []
  param include_dirs : list[string] => env['include_dirs'] or []
  param library_dirs : list[string] => env['library_dirs'] or []
  outputs => sources.map(src => path.add_ext(src, "o"))
  actions => [
    "gcc"
    c_flags ++
    include_dirs.map(x => ["-I", x]) ++
    sources ++
    ["-o", outputs[0]]
  ]
}

# -----------------------------------------------------------------------------
# Builder for C++ source files
# -----------------------------------------------------------------------------

cplus_builder = builder {
  param env : any => self.module
  param cplus_flags : list[string] => env['cplus_flags'] or []
  param include_dirs : list[string] => env['include_dirs'] or []
  param library_dirs : list[string] => env['library_dirs'] or []
  outputs => sources.map(src => path.add_ext(src, "o"))
  actions => [
    "gcc"
    cplus_flags ++
    include_dirs.map(x => ["-I", x]) ++
    sources ++
    ["-o", outputs[0]]
  ]
}

# -----------------------------------------------------------------------------
# Builder for Objectve-C source files
# -----------------------------------------------------------------------------

objective_c_builder = builder {
  #output_types = ["o"]
  outputs => sources.map(src => path.add_ext(src, "o"))
  actions => [
    "gcc",
#    target.cflags,
#    target.include_dirs.map(x => ["-I", x])
    sources
  ]
}

# -----------------------------------------------------------------------------
# Builder for Objectve-C++ source files
# -----------------------------------------------------------------------------

objective_cplus_builder = builder {
  #output_types = ["o"]
  outputs => sources.map(src => path.add_ext(src, "o"))
  actions => [
    "gcc",
#    target.cflags,
#    target.include_dirs.map(x => ["-I", x])
    sources
  ]
}

# -----------------------------------------------------------------------------
# Target that knows how to invoke builders to produce object files.
# -----------------------------------------------------------------------------

delegating_builder = builder {
  param cflags : list[string] = []
  param cxxflags : list[string] = []
  param include_dirs : list[string] = []
  param library_dirs : list[string] = []
  param warnings_as_errors = false
  param builder_map : dict[string, builder] = {
    "c"   = c_builder,
    "cpp" = cplus_builder,
    "cxx" = cplus_builder,
    "cc"  = cplus_builder,
    "m"   = objective_c_builder,
    "mm"  = objective_cplus_builder,
    "h"   = null_builder,
    "hpp" = null_builder,
    "hxx" = null_builder,
    "lib" = identity_builder,
    "a"   = identity_builder,
    "o"   = identity_builder,
  }
  export param implicit_depends : list[builder] => sources.map(
      src => builder_map[path.ext(src)] {
        sources = [ src ]
        env = self.module
      })
}

# -----------------------------------------------------------------------------
# Creates an executable from C++ or C sources.
# -----------------------------------------------------------------------------

executable = delegating_builder {
}

# -----------------------------------------------------------------------------
# Creates a library from C++ or C sources.
# -----------------------------------------------------------------------------

library = delegating_builder {
}

