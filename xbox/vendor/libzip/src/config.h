#ifndef HAD_CONFIG_H
#define HAD_CONFIG_H
#ifndef _HAD_ZIPCONF_H
#include "zipconf.h"
#endif
/*
 * Hand-written config.h for the RXDK / Xbox (picolibc + zig/clang) build of libzip
 * 1.11.4. Normally produced by CMake; here we pin the feature set for the OG-Xbox
 * target: zlib compression only (deflate/store), and NO bzip2 / xz(lzma) / zstd and
 * NO encryption (WinZip-AES / OpenSSL / GnuTLS / mbedTLS / CommonCrypto / Windows
 * bcrypt). Only the HAVE_* that picolibc actually satisfies are enabled; the rest
 * stay undefined so libzip falls back to its portable (fseek/ftell + long) paths.
 */
/* BEGIN DEFINES */
/* #undef ENABLE_FDOPEN */          /* zip_fdopen() -> returns "operation not supported" */
/* #undef HAVE___PROGNAME */
/* #undef HAVE__CLOSE */
/* #undef HAVE__DUP */
/* #undef HAVE__FDOPEN */
/* #undef HAVE__FILENO */
/* #undef HAVE__FSEEKI64 */
/* #undef HAVE__FSTAT64 */
/* #undef HAVE__SETMODE */
/* #undef HAVE__SNPRINTF */
/* #undef HAVE__SNPRINTF_S */
/* #undef HAVE__SNWPRINTF_S */
/* #undef HAVE__STAT64 */
/* #undef HAVE__STRDUP */
/* #undef HAVE__STRICMP */
/* #undef HAVE__STRTOI64 */
/* #undef HAVE__STRTOUI64 */
/* #undef HAVE__UNLINK */
/* #undef HAVE_ARC4RANDOM */
/* #undef HAVE_CLONEFILE */
/* #undef HAVE_COMMONCRYPTO */
/* #undef HAVE_CRYPTO */
/* #undef HAVE_FICLONERANGE */
#define HAVE_FILENO
/* #undef HAVE_FCHMOD */
#define HAVE_FSEEKO                 /* picolibc declares fseeko()/ftello() (off_t is 32-bit) */
#define HAVE_FTELLO
/* #undef HAVE_GETPROGNAME */
/* #undef HAVE_GETSECURITYINFO */
/* #undef HAVE_GNUTLS */
/* #undef HAVE_LIBBZ2 */
/* #undef HAVE_LIBLZMA */
/* #undef HAVE_LIBZSTD */
#define HAVE_LOCALTIME_R
/* #undef HAVE_LOCALTIME_S */
/* #undef HAVE_MEMCPY_S */
/* #undef HAVE_MBEDTLS */
/* #undef HAVE_MKSTEMP */
/* #undef HAVE_OPENSSL */
/* #undef HAVE_SETMODE */
#define HAVE_SNPRINTF
/* #undef HAVE_SNPRINTF_S */
#define HAVE_STRCASECMP
#define HAVE_STRDUP
/* #undef HAVE_STRERROR_S */
/* #undef HAVE_STRERRORLEN_S */
/* #undef HAVE_STRICMP */
/* #undef HAVE_STRNCPY_S */
#define HAVE_STRTOLL
#define HAVE_STRTOULL
/* #undef HAVE_STRUCT_TM_TM_ZONE */
#define HAVE_STDBOOL_H
#define HAVE_STRINGS_H
#define HAVE_UNISTD_H
/* #undef HAVE_WINDOWS_CRYPTO */
#define SIZEOF_OFF_T 4
#define SIZEOF_SIZE_T 4
/* #undef HAVE_DIRENT_H */
/* #undef HAVE_FTS_H */
/* #undef HAVE_NDIR_H */
/* #undef HAVE_SYS_DIR_H */
/* #undef HAVE_SYS_NDIR_H */
/* #undef WORDS_BIGENDIAN */        /* Xbox is little-endian x86 */
/* #undef HAVE_SHARED */
/* END DEFINES */
#define PACKAGE "libzip"
#define VERSION "1.11.4"

#endif /* HAD_CONFIG_H */
