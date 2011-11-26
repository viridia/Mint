# Mintfile to build Mint.

from prelude:configtests import check_include_file, check_include_file_cpp

#test1 = console.debug(path.ext("main.cpp"))
#test2 = console.info(path.change_ext("main.cpp", "o"))
#test3 = console.warn(path.add_ext("main.cpp", "o"))
#test4 = console.warn(path.basename("dir/main.cpp"))
#test5 = console.error(path.dirname("dir/main.cpp"))

option debug_symbols : bool {
  default = false
  help = "Build executable with debug symbols."
}

option opt_level : int {
  default = 0
  help = "Compiler optimization level."
}

HAVE_STDIO_H        = check_include_file { header = "stdio.h" }
HAVE_STDBOOL_H      = check_include_file { header = "stdbool.h" }
HAVE_STDDEF_H       = check_include_file { header = "stddef.h" }
HAVE_STDLIB_H       = check_include_file { header = "stdlib.h" }
HAVE_DIRENT_H       = check_include_file { header = "dirent.h" }
HAVE_ERRNO_H        = check_include_file { header = "errno.h" }
HAVE_FCNTL_H        = check_include_file { header = "fcntl.h" }
HAVE_LIMITS_H       = check_include_file { header = "limits.h" }
HAVE_SIGNAL_H       = check_include_file { header = "signal.h" }
HAVE_STRING_H       = check_include_file { header = "string.h" }
HAVE_UNISTD_H       = check_include_file { header = "unistd.h" }
HAVE_SYS_UNISTD_H   = check_include_file { header = "sys/unistd.h" }
HAVE_SYS_STAT_H     = check_include_file { header = "sys/stat.h" }
HAVE_CPP_ALGORITHM  = check_include_file_cpp { header = "algorithm" }
HAVE_CPP_ITERATOR   = check_include_file_cpp { header = "iterator" }
HAVE_CPP_NEW        = check_include_file_cpp { header = "new" }

#configure {
#  source = "include/mint/config.h.in"
#  output = "include/mint/config.h"
#}

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
