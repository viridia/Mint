/* ================================================================== *
 * System Configuration Definitions
 * ================================================================== */

#ifndef MINT_CONFIG_H
#define MINT_CONFIG_H

// C header files
#define HAVE_DIRENT_H 1
#define HAVE_ERRNO_H 1
#define HAVE_FCNTL_H 1
/* #undef HAVE_IO_H */
#define HAVE_LIMITS_H 1
/* #undef HAVE_MALLOC_H */
#define HAVE_MALLOC_MALLOC_H 1
#define HAVE_POLL_H 1
#define HAVE_STDBOOL_H 1
#define HAVE_STDDEF_H 1
#define HAVE_STDIO_H 1
/* #undef HAVE_STDINT_H */
#define HAVE_STDLIB_H 1
#define HAVE_STRING_H 1
#define HAVE_SIGNAL_H 1
#define HAVE_TIME_H 1
#define HAVE_UNISTD_H 1
#define HAVE_SYS_STAT_H 1
#define HAVE_SYS_TIME_H 1
#define HAVE_SYS_WAIT_H 1
#define HAVE_SYS_UNISTD_H 1

// C++ header files
#define HAVE_CPLUS_ALGORITHM 1
#define HAVE_CPLUS_ITERATOR 1
#define HAVE_CPLUS_MEMORY 1
#define HAVE_CPLUS_NEW 1
#define HAVE_CPLUS_QUEUE 1
#define HAVE_CPLUS_STRING 1
/* #undef HAVE_CPLUS_TYPE_TRAITS */
#define HAVE_CPLUS_UTILITY 1

// Whether the terminal supports ANSI color codes.
#define ANSI_COLORS 1

// Whether isatty() is available.
#define HAVE_ISATTY 1

// Whether stat() is available.
#define HAVE_STAT 1

// Whether access() is available.
#define HAVE_ACCESS 1

// Whether malloc_size() is available.
#define HAVE_MALLOC_SIZE 1

// Whether malloc_usable_size() is available.
/* #undef HAVE_MALLOC_USABLE_SIZE */

// Whether the time_t ssize_t is availble
#define HAVE_TYPE_SSIZE_T 1

// Whether the timespec struct is availble
#define HAVE_TYPE_TIMESPEC 1

// Whether the time_t type is availble
#define HAVE_TYPE_TIME_T 1

// Whether the 'struct dirent' type defined in <dirent.h> has the 'd_type' member.
#define DIRENT_HAS_D_TYPE 1

// Size of an integer.
#define SIZEOF_INT 4

// Size of size_t.
#define SIZEOF_SIZE_T 8

// The installation directory of the Mint standard prelude
#ifndef PRELUDE_PATH
#define PRELUDE_PATH "/usr/lib/mint/prelude"
#endif

// The Mint standard prelude (in the build directory)
#ifndef SRCDIR_PRELUDE_PATH
#define SRCDIR_PRELUDE_PATH "/Users/talin/Projects/mint/mint/prelude"
#endif

// Name of the host platform we're compiling for.
#ifndef HOST_PLATFORM
#define HOST_PLATFORM "OSX"
#endif

#endif // MINT_CONFIG_H
