# -----------------------------------------------------------------------------
# OSX platform variables.
# -----------------------------------------------------------------------------

from compilers.gcc import gcc
from compilers.ar import ar
from compilers.ld import ld

object_file_ext = 'o'
static_lib_ext = 'a'
executable_ext = ''
c_compiler_default = gcc.compiler
cplus_compiler_default = gcc.compiler
lib_compiler_default = ar
linker_default = gcc.linker
