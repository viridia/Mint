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
  output_types = ["o"]
  outputs = path.add_extension(sources, "o")
  actions = [
    "gcc",
    target.cflags,
#    target.include_dirs.map(x => ["-I", x])
    sources
  ]
}

# -----------------------------------------------------------------------------
# Builder for C++ source files
# -----------------------------------------------------------------------------

cpp_builder = builder {
  # source_patterns = ["Makefile", "CMakeLists.txt", "*.mint"]
  output_types = ["o"]
  outputs = path.add_extension(sources, "o")
  actions = [
    "gcc",
    target.cxxflags,
#    target.include_dirs.map(x => ["-I", x])
    sources
  ]
}

# -----------------------------------------------------------------------------
# Base for targets that produce files.
# -----------------------------------------------------------------------------

file_target = target {
  lazy param actions : list[any] = []
}

# -----------------------------------------------------------------------------
# Target that knows how to invoke builders to produce object files.
# -----------------------------------------------------------------------------

object_file_target = file_target {
  param cflags : list[string] = []
  param cxxflags : list[string] = []
  param include_dirs : list[string] = []
  param library_dirs : list[string] = []
  param builder_map : dict[string, builder] = {
    "c"   = c_builder,
    "cpp" = cpp_builder,
    "cxx" = cpp_builder,
    "cc"  = cpp_builder,
    "h"   = null_builder,
    "hpp" = null_builder,
    "hxx" = null_builder,
    "lib" = identity_builder,
    "a"   = identity_builder,
    "o"   = identity_builder,
  }
  actions = sources.map(
      src => builder_map[path.ext(src)] {
        target = self
        sources = [ src ]
      })
}

# -----------------------------------------------------------------------------
# Creates an executable from C++ or C sources.
# -----------------------------------------------------------------------------

executable = object_file_target {
}

# -----------------------------------------------------------------------------
# Creates a library from C++ or C sources.
# -----------------------------------------------------------------------------

library = object_file_target {
}

# TODO - these should be explicitly included, and there should be a means
# to create your own tests.

check_include_file = object {
  param header : string = undefined
  param paths : list[string] = [] # Make this the standard paths
  param message : string = "Checking for ${0}..."
#  lazy param test : bool = [ any(file_exists(path, header) for path in paths) ]
#  lazy param test : bool = [ any(path => path.is_file(path.join(path, header)), paths) ]
# lazy param source_file = tempfile { extension = "c", content = "#include \"${header}\" "}
# lazy param test = cc { sources = [ source_file.path ] }.test
}

check_include_file_cpp = object {
  param header : string = undefined
  param paths : list[string] = []
  param message : string = "Checking for ${0}..."
#  lazy param test = [ any(file_exists(path, header) for path in paths) ]
}

find_library = object {
  param library : string = undefined
  param paths : list[string] = []
  param message : string = "Checking for ${0}..."
#  lazy param test = [ any(file_exists(path, header) for path in paths) ]
}

cpp = tool {
  param sources : list[string] = []
  param include_dirs : list[string] = []
  param library_dirs : list[string] = []
  param flags : list[string] = []
#  lazy param include_flags : list[string] = map(include_dirs, dir => ["-I", dir])
#  lazy param compile : list[function] = [
#    shell("g++", flags ++ include_flags)
#  ]
#  lazy param gendeps : list[function] = [
#    shell("g++", flags ++ include_flags)
#  ]
#  lazy param test : list[function] = [
#    shell("g++", flags ++ include_flags)
#  ]
}
