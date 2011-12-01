/* ================================================================== *
 * System Configuration Definitions
 * ================================================================== */

#ifndef MINT_CONFIG_H
#define MINT_CONFIG_H

// Set to 1 if <stdio.h> is available.
#ifndef HAVE_STDIO_H
#define HAVE_STDIO_H 1
#endif

// Set to 1 if <unistd.h> is available.
#ifndef HAVE_UNISTD_H
#define HAVE_UNISTD_H 1
#endif

// Set to 1 if <sys/unistd.h> is available.
#ifndef HAVE_SYS_UNISTD_H
#define HAVE_SYS_UNISTD_H 1
#endif

// Set to 1 if <sys/stat.h> is available.
#ifndef HAVE_SYS_STAT_H
#define HAVE_SYS_STAT_H 1
#endif

// Set to 1 if <errno.h> is available.
#ifndef HAVE_ERRNO_H
#define HAVE_ERRNO_H 1
#endif

// Set to 1 if <sstddef.h> is available.
#ifndef HAVE_STDDEF_H
#define HAVE_STDDEF_H 1
#endif

// Set to 1 if <stdlib.h> is available.
#ifndef HAVE_STDLIB_H
#define HAVE_STDLIB_H 1
#endif

// Set to 1 if <stdbool.h> is available.
#ifndef HAVE_STDBOOL_H
#define HAVE_STDBOOL_H 1
#endif

// Set to 1 if <string.h> is available.
#ifndef HAVE_STRING_H
#define HAVE_STRING_H 1
#endif

// Set to 1 if <limits.h> is available.
#ifndef HAVE_LIMITS_H
#define HAVE_LIMITS_H 1
#endif

// Set to 1 if <fcntl.h> is available.
#ifndef HAVE_FCNTL_H
#define HAVE_FCNTL_H 1
#endif

// Set to 1 if <signal.h> is available.
#ifndef HAVE_SIGNAL_H
#define HAVE_SIGNAL_H 1
#endif

// Set to 1 if <dirent.h> is available.
#ifndef HAVE_DIRENT_H
#define HAVE_DIRENT_H 1
#endif

// Set to 1 if <malloc/malloc.h> is available.
#ifndef HAVE_MALLOC_MALLOC_H
#define HAVE_MALLOC_MALLOC_H 0
#endif

// Set to 1 if <algorithm> is available
#ifndef HAVE_ALGORITHM
#define HAVE_ALGORITHM 1
#endif

// Set to 1 if <iterator> is available
#ifndef HAVE_ITERATOR
#define HAVE_ITERATOR 1
#endif

// Set to 1 if <memory> is available
#ifndef HAVE_MEMORY
#define HAVE_MEMORY 1
#endif

// Set to 1 if <new> is available
#ifndef HAVE_NEW
#define HAVE_NEW 1
#endif

// Set to 1 if <string> is available.
#ifndef HAVE_STRING
#define HAVE_STRING 1
#endif

// Set to 1 if the terminal supports ANSI color codes.
#ifndef ANSI_COLORS
#define ANSI_COLORS 1
#endif

// Set to 1 if isatty() is available.
#ifndef HAVE_ISATTY
#define HAVE_ISATTY 1
#endif

// Set to 1 if stat() is available.
#ifndef HAVE_STAT
#define HAVE_STAT 1
#endif

// Set to 1 if access() is available.
#ifndef HAVE_ACCESS
#define HAVE_ACCESS 1
#endif

// Set to 1 if malloc_size() is available.
#ifndef HAVE_MALLOC_SIZE
#define HAVE_MALLOC_SIZE 0
#endif

// Set to 1 if malloc_usable_size() is available.
#ifndef HAVE_MALLOC_USABLE_SIZE
#define HAVE_MALLOC_USABLE_SIZE 0
#endif

// A string containing the character used to separate directory names in a path
#ifndef NATIVE_DIRECTORY_SEPARATOR
#define NATIVE_DIRECTORY_SEPARATOR "/"
#endif

// A string containing the alternate character used to separate directory names in a path
#ifndef NATIVE_ALT_DIRECTORY_SEPARATOR
#define NATIVE_ALT_DIRECTORY_SEPARATOR "\\"
#endif

// The installation directory of the Mint standard prelude
#ifndef PRELUDE_PATH
#define PRELUDE_PATH "/usr/lib/mint/prelude"
#endif

// Whether the 'struct dirent' type defined in <dirent.h> has the 'd_type' member.
#ifndef DIRENT_HAS_D_TYPE
#define DIRENT_HAS_D_TYPE 1
#endif

#endif // MINT_CONFIG_H
