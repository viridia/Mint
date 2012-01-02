# -----------------------------------------------------------------------------
# OSX platform variables.
# -----------------------------------------------------------------------------

from compilers.gcc import gcc
from compilers.ar import ar

label = 'osx'
object_file_ext = 'o'
static_lib_ext = 'a'
executable_ext = ''
c_compiler_default = gcc.compiler
cplus_compiler_default = gcc.compiler
lib_compiler_default = ar
linker_default = gcc.linker
std_cplus_libs = [ 'stdc++' ]
