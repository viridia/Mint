# -----------------------------------------------------------------------------
# Mintfile to build Mint.
# -----------------------------------------------------------------------------

from prelude:configtests import
    check_include_file,
    check_include_file_cplus,
    check_function_exists,
    check_struct_has_member
    
from prelude:templates import c_header_template

# -----------------------------------------------------------------------------
# Configuration options
# -----------------------------------------------------------------------------

host_platform = option {
  param value : string = ""
  help = 'platform we are building for.'
}

ansi_colors = option {
  param value : bool = false
  help = 'Build with support for ANSI colors.'
}

debug_symbols = option {
  param value : bool = false
  help = 'Build executable with debug symbols.'
}

opt_level = option {
  param value : int = 0
  help = 'Compiler optimization level.'
}

# -----------------------------------------------------------------------------
# Configuration tests.
# -----------------------------------------------------------------------------

HAVE_DIRENT_H         = check_include_file { header = 'dirent.h' }
HAVE_ERRNO_H          = check_include_file { header = 'errno.h' }
HAVE_FCNTL_H          = check_include_file { header = 'fcntl.h' }
HAVE_LIMITS_H         = check_include_file { header = 'limits.h' }
HAVE_MALLOC_H         = check_include_file { header = 'malloc.h' }
HAVE_MALLOC_MALLOC_H  = check_include_file { header = 'malloc/malloc.h' }
HAVE_POLL_H           = check_include_file { header = 'poll.h' }
HAVE_SIGNAL_H         = check_include_file { header = 'signal.h' }
HAVE_STDBOOL_H        = check_include_file { header = 'stdbool.h' }
HAVE_STDDEF_H         = check_include_file { header = 'stddef.h' }
HAVE_STDIO_H          = check_include_file { header = 'stdio.h' }
HAVE_STDLIB_H         = check_include_file { header = 'stdlib.h' }
HAVE_STRING_H         = check_include_file { header = 'string.h' }
HAVE_TIME_H           = check_include_file { header = 'time.h' }
HAVE_UNISTD_H         = check_include_file { header = 'unistd.h' }
HAVE_SYS_STAT_H       = check_include_file { header = 'sys/stat.h' }
HAVE_SYS_TIME_H       = check_include_file { header = 'sys/time.h' }
HAVE_SYS_WAIT_H       = check_include_file { header = 'sys/wait.h' }
HAVE_SYS_UNISTD_H     = check_include_file { header = 'sys/unistd.h' }
HAVE_CPLUS_ALGORITHM  = check_include_file_cplus { header = 'algorithm' }
HAVE_CPLUS_ITERATOR   = check_include_file_cplus { header = 'iterator' }
HAVE_CPLUS_MEMORY     = check_include_file_cplus { header = 'memory' }
HAVE_CPLUS_NEW        = check_include_file_cplus { header = 'new' }
HAVE_CPLUS_QUEUE      = check_include_file_cplus { header = 'queue' }
HAVE_CPLUS_STRING     = check_include_file_cplus { header = 'string' }
HAVE_CPLUS_UTILITY    = check_include_file_cplus { header = 'utility' }

HAVE_ISATTY           = check_function_exists { function = 'isatty' }
HAVE_STAT             = check_function_exists { function = 'stat' }
HAVE_ACCESS           = check_function_exists { function = 'access' }
HAVE_MALLOC_SIZE      = check_function_exists { function = 'malloc_size' }
HAVE_MALLOC_USABLE_SIZE = check_function_exists { function = 'malloc_usable_size' }

DIRENT_HAS_D_TYPE = check_struct_has_member {
  struct = 'dirent'
  member = 'd_type'
  header = 'dirent.h'
}

SRC_PRELUDE_PATH = path.join(source_dir, "prelude")

ANSI_COLORS = ansi_colors

# -----------------------------------------------------------------------------
# Actions to perform during configuration.
# -----------------------------------------------------------------------------

do c_header_template {
  source = 'include/mint/config.h.in'
  output = 'include/mint/config.h'
}

# -----------------------------------------------------------------------------
# Common definitions.
# -----------------------------------------------------------------------------

include_dirs = [
  path.join(output_dir, 'include'),
  'include',
  'third_party/gtest-1.6.0/include',
  'third_party/gtest-1.6.0',
  'third_party/re2'
]

warnings_as_errors = true

# -----------------------------------------------------------------------------
# Build targets.
# -----------------------------------------------------------------------------

from third_party.re2 import re2

lib_mint = library {
  name = 'mint'
  sources = glob('lib/*/*.cpp')
  outputs = [ 'lib/mint.a' ]
}

gtest = library {
  sources = [ 'third_party/gtest-1.6.0/src/gtest-all.cc' ]
  outputs = [ 'lib/gtest.a' ]
}

mint = executable {
  depends = [ lib_mint, re2 ]
  sources = [ 'tools/mint/mint.cpp' ]
  libs    = [ 'stdc++' ]
}

unittest = executable {
  depends = [ lib_mint, re2, gtest ]
#  depends = [ lib_mint, %('third_party/re2#re2') ]
  sources = glob('test/unit/*.cpp')
  outputs = [ 'test/unit/unittest' ]
  libs    = [ 'stdc++' ]
}

check = target {
  depends = [ unittest ]
  actions = [
    command("test/unit/unittest", [])
  ]
}
