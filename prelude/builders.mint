# -----------------------------------------------------------------------------
# Builders
# -----------------------------------------------------------------------------

from platform import platform

# -----------------------------------------------------------------------------
# Base for targets that produce files.
# -----------------------------------------------------------------------------

builder = target {
  # Default source directory is from the invoking module
  param source_dir : string => self.module.source_dir

  # 'gendeps' is the target which generates the file containing the list of
  # automatic dependencies for this target
  param gendeps : list[target] = []

  # Absolute paths to output files 
  var abs_outputs : list[string] => outputs.map(x => path.join(output_dir, x))

  # Method to calculate the output path from the source path.
  def build_output_path(source:string) -> string :
      if (source.starts_with(source_dir))
          output_dir ++ source.substr(source_dir.size, source.size)
      else path.join(output_dir, source)
}

# -----------------------------------------------------------------------------
# Builder that does nothing - has no output
# -----------------------------------------------------------------------------

null_builder = builder {
  outputs = []
  exclude_from_all = true
  source_only = true
}

# -----------------------------------------------------------------------------
# Builder that does nothing - it's output is just it's input
# -----------------------------------------------------------------------------

identity_builder = builder {
  # Make the outputs absolute
  outputs => sources.map(src => path.join(source_dir, src))
  exclude_from_all = true
  source_only = true
}

# -----------------------------------------------------------------------------
# Dependency generator for C source files
# -----------------------------------------------------------------------------

c_gendeps = builder {
  param c_flags      : list[string]
  param include_dirs : list[string]
  param compiler     : object = platform.c_compiler_default
  #param compiler : object => clang.gendeps.compose(self, self.module)
  outputs => [ "c_sources.deps" ]
  actions => compiler.gendeps
}

# -----------------------------------------------------------------------------
# Dependency generator for C++ source files
# -----------------------------------------------------------------------------

cplus_gendeps = builder {
  param cplus_flags  : list[string]
  param include_dirs : list[string]
  param compiler     : object = platform.c_compiler_default
  #param compiler : object => clang.gendeps.compose(self, self.module)
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
  var compiler_instance : object => compiler.compose(
    { 'include_dirs' = include_dirs.map(x => path.join(source_dir, x)),
      'outputs' = abs_outputs
    },
    self,
    self.module)

  # Outputs
  outputs => sources.map(src => build_output_path(path.add_ext(src, platform.object_file_ext)))
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
  var compiler_instance : object => compiler.compose(
    { 'include_dirs' = include_dirs.map(x => path.join(source_dir, x)),
      'outputs' = abs_outputs
    },
    self,
    self.module)
  
  # Outputs
  outputs => sources.map(src => build_output_path(path.add_ext(src, platform.object_file_ext)))
  actions => compiler_instance.actions
  
  # We want one deps file per source directory, so use folding.
#  gendeps => sources.map(src => cplus_gendeps.for_output(
#      path.join(path.parent(src), "deps.txt"),
#      [ { sources = [ src ], }, self, self.module ],
}

# -----------------------------------------------------------------------------
# Builder for Objective-C source files
# -----------------------------------------------------------------------------

objective_c_builder = c_builder {
  # TODO: Fill in
}

# -----------------------------------------------------------------------------
# Builder for Objective-C++ source files
# -----------------------------------------------------------------------------

objective_cplus_builder = cplus_builder {
  # TODO: Fill in
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
          path.join(source_dir, src), { 'depends' = [] }, self, self.module))
  # List of output files from all delegated builders.
  # Note that in makefile generation this gets replaced with a simple '$<'.
  var implicit_sources : list[string] => (
      implicit_depends ++ depends).map(bld => bld.abs_outputs).merge()
}

# -----------------------------------------------------------------------------
# Builder that simply copies files from source to the output directory.
# This works either with explicitly listed source files, or outputs from
# other targets which have been listed as dependencies.
# -----------------------------------------------------------------------------

filecopy_builder = builder {
  exclude_from_all = true

  var abs_sources : list[string] => sources ++ depends.map(tgt => tgt.outputs).merge()

  outputs => abs_sources.map(src => build_output_path(src))

  # Using cp rather than internal file copy makes it easier to export to makefile.
  actions => abs_sources.map(src => command("cp", [src, build_output_path(src)]))
}
