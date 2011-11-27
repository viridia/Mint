# Standard Mint prelude

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
  export lazy param actions : list[any] = []
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
  outputs = sources
}

# -----------------------------------------------------------------------------
# Builder for C source files
# -----------------------------------------------------------------------------

c_builder = builder {
  #output_types = ["o"]
  outputs = sources.map(src => path.add_ext(src, "o"))
  actions = [
    "gcc",
#    target.cflags,
#    target.include_dirs.map(x => ["-I", x])
    sources
  ]
}

# -----------------------------------------------------------------------------
# Builder for C++ source files
# -----------------------------------------------------------------------------

cpp_builder = builder {
  # source_patterns = ["Makefile", "CMakeLists.txt", "*.mint"]
  #output_types = ["o"]
  outputs = sources.map(src => path.add_ext(src, "o"))
  actions = [
    "gcc",
#    target.cxxflags,
#    target.include_dirs.map(x => ["-I", x])
    sources
  ]
}

# -----------------------------------------------------------------------------
# Builder for Objectve-C source files
# -----------------------------------------------------------------------------

objective_c_builder = builder {
  #output_types = ["o"]
  outputs = sources.map(src => path.add_ext(src, "o"))
  actions = [
    "gcc",
#    target.cflags,
#    target.include_dirs.map(x => ["-I", x])
    sources
  ]
}

# -----------------------------------------------------------------------------
# Builder for Objectve-C++ source files
# -----------------------------------------------------------------------------

objective_cpp_builder = builder {
  #output_types = ["o"]
  outputs = sources.map(src => path.add_ext(src, "o"))
  actions = [
    "gcc",
#    target.cflags,
#    target.include_dirs.map(x => ["-I", x])
    sources
  ]
}

# -----------------------------------------------------------------------------
# Target that knows how to invoke builders to produce object files.
# -----------------------------------------------------------------------------

object_builder = builder {
  param cflags : list[string] = []
  param cxxflags : list[string] = []
  param include_dirs : list[string] = []
  param library_dirs : list[string] = []
  param warnings_as_errors = false
  param builder_map : dict[string, builder] = {
    "c"   = c_builder,
    "cpp" = cpp_builder,
    "cxx" = cpp_builder,
    "cc"  = cpp_builder,
    "m"   = objective_c_builder,
    "mm"  = objective_cpp_builder,
    "h"   = null_builder,
    "hpp" = null_builder,
    "hxx" = null_builder,
    "lib" = identity_builder,
    "a"   = identity_builder,
    "o"   = identity_builder,
  }
  export lazy param implicit_depends : list[builder] = sources.map(
      src => builder_map[path.ext(src)] {
        sources = [ src ]
      })
  actions = implicit_depends.map(b => b.actions)
}

# -----------------------------------------------------------------------------
# Creates an executable from C++ or C sources.
# -----------------------------------------------------------------------------

executable = object_builder {
}

# -----------------------------------------------------------------------------
# Creates a library from C++ or C sources.
# -----------------------------------------------------------------------------

library = object_builder {
}

