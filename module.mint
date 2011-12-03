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

option debug_symbols : bool {
  default = false
  help = "Build executable with debug symbols."
}

option opt_level : int {
  default = 0
  help = "Compiler optimization level."
}

# -----------------------------------------------------------------------------
# Configuration tests.
# -----------------------------------------------------------------------------

HAVE_STDIO_H          = check_include_file { header = "stdio.h" }
HAVE_STDBOOL_H        = check_include_file { header = "stdbool.h" }
HAVE_STDDEF_H         = check_include_file { header = "stddef.h" }
HAVE_STDLIB_H         = check_include_file { header = "stdlib.h" }
HAVE_DIRENT_H         = check_include_file { header = "dirent.h" }
HAVE_ERRNO_H          = check_include_file { header = "errno.h" }
HAVE_FCNTL_H          = check_include_file { header = "fcntl.h" }
HAVE_LIMITS_H         = check_include_file { header = "limits.h" }
HAVE_SIGNAL_H         = check_include_file { header = "signal.h" }
HAVE_STRING_H         = check_include_file { header = "string.h" }
HAVE_UNISTD_H         = check_include_file { header = "unistd.h" }
HAVE_SYS_UNISTD_H     = check_include_file { header = "sys/unistd.h" }
HAVE_SYS_STAT_H       = check_include_file { header = "sys/stat.h" }
HAVE_CPLUS_ALGORITHM  = check_include_file_cplus { header = "algorithm" }
HAVE_CPLUS_ITERATOR   = check_include_file_cplus { header = "iterator" }
HAVE_CPLUS_MEMORY     = check_include_file_cplus { header = "memory" }
HAVE_CPLUS_NEW        = check_include_file_cplus { header = "new" }

HAVE_ISATTY           = check_function_exists { function = "isatty" }
HAVE_STAT             = check_function_exists { function = "stat" }
HAVE_ACCESS           = check_function_exists { function = "access" }
HAVE_MALLOC_SIZE      = check_function_exists { function = "malloc_size" }
HAVE_MALLOC_USABLE_SIZE = check_function_exists { function = "malloc_usable_size" }

DIRENT_HAS_D_TYPE = check_struct_has_member {
  struct = "dirent"
  member = "d_type"
  header = "dirent.h"
}

# -----------------------------------------------------------------------------
# Actions to perform during configuration.
# -----------------------------------------------------------------------------

do c_header_template {
  source = "include/mint/config.h.in"
  output = "include/mint/config.h"
  env = self.module
}

# -----------------------------------------------------------------------------
# Build targets.
# -----------------------------------------------------------------------------

lib_mint = library {
  sources = glob("lib/*/*.cpp")
  outputs = [ "mint" ]
}

mint = executable {
  depends = [ lib_mint ]
  sources = [ "tools/mint/mint.cpp" ]
  outputs = [ "mint" ]
}

unittest = executable {
  depends = [ lib_mint ]
  sources = glob("test/unit/*.cpp")
  outputs = [ "unittest" ]
}
