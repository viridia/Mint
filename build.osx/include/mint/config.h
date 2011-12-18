/* ================================================================== *
 * System Configuration Definitions
 * ================================================================== */

#ifndef MINT_CONFIG_H
#define MINT_CONFIG_H

// C header files
/* #undef HAVE_STDIO_H */
/* #undef HAVE_UNISTD_H */
/* #undef HAVE_SYS_UNISTD_H */
/* #undef HAVE_SYS_STAT_H */
/* #undef HAVE_ERRNO_H */
/* #undef HAVE_STDDEF_H */
/* #undef HAVE_STDLIB_H */
/* #undef HAVE_STDBOOL_H */
/* #undef HAVE_STRING_H */
/* #undef HAVE_LIMITS_H */
/* #undef HAVE_FCNTL_H */
/* #undef HAVE_SIGNAL_H */
/* #undef HAVE_DIRENT_H */
/* #undef HAVE_TIME_H */
/* #undef HAVE_MALLOC_MALLOC_H */

// C++ header files
/* #undef HAVE_CPLUS_ALGORITHM */
/* #undef HAVE_CPLUS_ITERATOR */
/* #undef HAVE_CPLUS_MEMORY */
/* #undef HAVE_CPLUS_NEW */

// Whether the terminal supports ANSI color codes.
/* #undef ANSI_COLORS */

// Whether isatty() is available.
/* #undef HAVE_ISATTY */

// Whether stat() is available.
/* #undef HAVE_STAT */

// Whether access() is available.
/* #undef HAVE_ACCESS */

// Whether malloc_size() is available.
/* #undef HAVE_MALLOC_SIZE */

// Whether malloc_usable_size() is available.
/* #undef HAVE_MALLOC_USABLE_SIZE */

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
#define DIRENT_HAS_D_TYPE 1

#endif // MINT_CONFIG_H
