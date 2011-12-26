# -----------------------------------------------------------------------------
# Standard Mint prelude
# -----------------------------------------------------------------------------

from platform import platform

from compilers.clang import clang

# -----------------------------------------------------------------------------
# Optimization level enum
# -----------------------------------------------------------------------------

#enum opt_level_enum { O1, O2, O3, O4 }

# -----------------------------------------------------------------------------
# Base for targets that produce files.
# -----------------------------------------------------------------------------

builder = target {
  # Default source directory is from the invoking module
  param source_dir : string => self.module.source_dir

  # Default output directory is from the invoking module
  param output_dir : string => self.module.output_dir

  # 'gendeps' is the target which generates the file containing the list of
  # automatic dependencies for this target
  param gendeps : list[target] = []

  # Method to calculate the output path from the source path.
  def output_path(source:string, ext:string) -> string :
    path.add_ext(
        if (source.starts_with(source_dir))
            output_dir ++ source.substr(source_dir.size, source.size)
        else path.join(output_path, source),
        ext)
}

# -----------------------------------------------------------------------------
# Builder that does nothing - has no output
# -----------------------------------------------------------------------------

null_builder = builder {
  outputs = []
}

# -----------------------------------------------------------------------------
# Builder that does nothing - it's output is just it's input
# -----------------------------------------------------------------------------

identity_builder = builder {
  # Make the outputs absolute
  outputs => sources.map(src => path.join(source_dir, src))
}

# -----------------------------------------------------------------------------
# Dependency generator for C source files
# -----------------------------------------------------------------------------

c_gendeps = builder {
  param c_flags      : list[string]
  param include_dirs : list[string]
  param compiler : object => clang.gendeps.compose([self, self.module])
  outputs => [ "c_sources.deps" ]
  actions => compiler.gendeps
}

# -----------------------------------------------------------------------------
# Dependency generator for C++ source files
# -----------------------------------------------------------------------------

cplus_gendeps = builder {
  param cplus_flags      : list[string]
  param include_dirs : list[string]
  param compiler : object => clang.gendeps.compose([self, self.module])
  outputs => [ "cplus_sources.deps" ]
  actions => compiler.gendeps
}

# -----------------------------------------------------------------------------
# Builder for C source files
# -----------------------------------------------------------------------------

c_builder = builder {
  # Inputs
  param c_flags      : list[string]
  param include_dirs : list[string]
  param library_dirs : list[string]
  param debug_symbols: bool
  param all_warnings : bool
  param warnings_as_errors : bool
  param compiler     : object = platform.c_compiler_default

  # Variables
  var flags : list[string] => self.c_flags or self.module['c_flags']
  var compiler_instance : object => compiler.compose([self, self.module])

  # Outputs
  outputs => sources.map(src => output_path(src, platform.object_file_ext))
  actions => compiler_instance.actions
}

# -----------------------------------------------------------------------------
# Builder for C++ source files
# -----------------------------------------------------------------------------

cplus_builder = builder {
  # Inputs
  param cplus_flags  : list[string]
  param include_dirs : list[string]
  param library_dirs : list[string]
  param debug_symbols: bool
  param all_warnings : bool
  param warnings_as_errors : bool
  param compiler     : object => platform.cplus_compiler_default

  # Variables
  var flags : list[string] => self.cplus_flags or self.module['cplus_flags']
  var compiler_instance : object => compiler.compose([
    { 'include_dirs' = include_dirs.map(x => path.join(source_dir, x)) },
    self,
    self.module
    ])
  
  # Outputs
  outputs => sources.map(src => output_path(src, platform.object_file_ext))
  actions => compiler_instance.actions
  
  # We want one deps file per source directory, so use folding.
#  gendeps => sources.map(src => cplus_gendeps.for_output(
#      path.join(path.parent(src), "deps.txt"),
#      [ { sources = [ src ], }, self, self.module ],
}

# -----------------------------------------------------------------------------
# Builder for Objective-C source files
# -----------------------------------------------------------------------------

objective_c_builder = builder {
  param c_flags      : list[string]
  param include_dirs : list[string]
  param library_dirs : list[string]
  param debug_symbols : bool
  param all_warnings : bool
  param warnings_as_errors : bool

  param flags : list[string] => self.c_flags or self.module['c_flags']
  param compiler : object => platform.compiler_default.compose([self, self.module])
  outputs => sources.map(src => path.add_ext(src, platform.object_file_ext))
  actions => compiler.actions
}

# -----------------------------------------------------------------------------
# Builder for Objective-C++ source files
# -----------------------------------------------------------------------------

objective_cplus_builder = builder {
  param cplus_flags  : list[string]
  param include_dirs : list[string]
  param library_dirs : list[string]
  param debug_symbols : bool
  param all_warnings : bool
  param warnings_as_errors : bool

  param flags : list[string] => self.cplus_flags or self.module['cplus_flags']
  param compiler : object => platform.compiler_default.compose([ self, self.module ])
  outputs => sources.map(src => path.add_ext(src, platform.object_file_ext))
  actions => compiler.actions
}

# -----------------------------------------------------------------------------
# Target that knows how to invoke builders to produce object files.
# -----------------------------------------------------------------------------

delegating_builder = builder {
  param c_flags      : list[string]
  param cplus_flags  : list[string]
  param ld_flags     : list[string]
  param include_dirs : list[string]
  param lib_dirs     : list[string]
  param libs         : list[string]
  param definitions  : dict[string, string]
  param debug_symbols : bool
  param enable_exceptions : bool = true
  param warnings_as_errors : bool
  param no_standard_includes : bool
  param builder_map : dict[string, builder] = {
    'c'   = c_builder,
    'cpp' = cplus_builder,
    'cxx' = cplus_builder,
    'cc'  = cplus_builder,
    'm'   = objective_c_builder,
    'mm'  = objective_cplus_builder,
    'h'   = null_builder,
    'hpp' = null_builder,
    'hxx' = null_builder,
    'lib' = identity_builder,
    'a'   = identity_builder,
    'o'   = identity_builder,
    'obj' = identity_builder,
  }
  # Create a builder for each source based on the file extension.
  implicit_depends => sources.map(
      src => builder_map[path.ext(src)].for_source(
          path.join(source_dir, src), [ { 'depends' = [] }, self, self.module ]))
  # List of output files from all delegated builders.
  # Note that in makefile generation this gets replaced with a simple '$<'.
  var implicit_sources : list[string] => (
      implicit_depends ++ depends).map(bld => bld.outputs).merge()
}

# -----------------------------------------------------------------------------
# Creates an executable from C++ or C sources.
# -----------------------------------------------------------------------------

executable = delegating_builder {
  param flags : list[string] => self.ld_flags or self.module['ld_flags']
  param linker : object => platform.linker_default.compose([
    { 'sources' = implicit_sources }
    self,
    self.module
  ])
  outputs => [ path.change_ext(name, platform.executable_ext) ]
  actions => linker.actions
}

# -----------------------------------------------------------------------------
# Creates a library from C++ or C sources.
# -----------------------------------------------------------------------------

library = delegating_builder {
  param archiver : object => platform.lib_compiler_default.compose([
    { 'sources' = implicit_sources }
    self,
    self.module
  ])
  outputs => [ path.change_ext(name, platform.static_lib_ext) ]
  actions => archiver.actions
}
