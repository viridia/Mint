# -----------------------------------------------------------------------------
# Standard Mint prelude
# -----------------------------------------------------------------------------

from platform import platform

from compilers.clang import clang, clang_link

# -----------------------------------------------------------------------------
# Optimization level enum
# -----------------------------------------------------------------------------

#enum opt_level_enum { O1, O2, O3, O4 }

# -----------------------------------------------------------------------------
# Header file scanner for C source files
# -----------------------------------------------------------------------------

#c_scanner = scanner {
#  actions = [
#    'gcc',
#    target.cflags,
##    target.include_dirs.map(x => ['-I', x])
#    sources
#  ]
#}

# -----------------------------------------------------------------------------
# Base for targets that produce files.
# -----------------------------------------------------------------------------

builder = target {
  # Default source directory is from the invoking module
  param source_dir : string => self.module.source_dir

  # Default output directory is from the invoking module
  param output_dir : string => self.module.output_dir

  # Default action is no actions.
  cached param actions : list[action] = []
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
  outputs => sources
}

# -----------------------------------------------------------------------------
# Builder for C source files
# -----------------------------------------------------------------------------

c_builder = builder {
  param c_flags      : list[string]
  param include_dirs : list[string]
  param library_dirs : list[string]
  param debug_symbols : bool
  param all_warnings : bool
  param warnings_as_errors : bool

  param flags : list[string] => self.c_flags or self.module['c_flags']
  param compiler : object => clang.compose([self, self.module])
  outputs => sources.map(src => path.add_ext(src, platform.object_file_ext))
  actions => compiler.compile
}

# -----------------------------------------------------------------------------
# Builder for C++ source files
# -----------------------------------------------------------------------------

cplus_builder = builder {
  param cplus_flags  : list[string]
  param include_dirs : list[string]
  param library_dirs : list[string]
  param debug_symbols : bool
  param all_warnings : bool
  param warnings_as_errors : bool

  param flags : list[string] => self.cplus_flags or self.module['cplus_flags']
  param compiler : object => clang.compose([ self, self.module ])
  outputs => sources.map(src => path.add_ext(src, platform.object_file_ext))
  actions => compiler.compile
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
  actions => compiler.compile
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
  actions => compiler.compile
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
  implicit_depends => sources.map(
      src => builder_map[path.ext(src)].compose([
        { 'sources' = [ src ],
          'implicit_depends' = []  # Prevent recursion
        },
        self,
        self.module ]))
}

# -----------------------------------------------------------------------------
# Creates an executable from C++ or C sources.
# -----------------------------------------------------------------------------

#executable = delegating_builder {
#  param flags : list[string] => self.ld_flags or self.module['ld_flags']
#  param linker : object => platform.linker_default.compose([
#    { 'sources' = implicit_depends.map(builder => builder.outputs).merge()
#    }
#    self,
#    self.module
#  ])
#  outputs => [ path.change_ext(name, platform.executable_ext) ]
#  actions => linker.build
#}

executable = delegating_builder {
  param flags : list[string] => self.ld_flags or self.module['ld_flags']
  param linker : object => clang_link.compose([
    { 'sources' = (implicit_depends ++ depends).map(tg => path.join_all(tg.output_dir, tg.outputs)).merge()
    }
    self,
    self.module
  ])
  outputs => [ path.change_ext(name, platform.executable_ext) ]
  actions => linker.build
}

# -----------------------------------------------------------------------------
# Creates a library from C++ or C sources.
# -----------------------------------------------------------------------------

library = delegating_builder {
  param archiver : object => platform.lib_compiler_default.compose([
    { 'sources' = (implicit_depends ++ depends).map(tg => tg.outputs).merge()
    }
    self,
    self.module
  ])
  outputs => [ path.change_ext(name, platform.static_lib_ext) ]
  actions => archiver.build
}
