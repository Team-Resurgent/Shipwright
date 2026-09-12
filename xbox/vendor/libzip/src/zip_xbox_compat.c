/*
 * RXDK/Xbox compatibility stubs for libzip.
 *
 * picolibc declares chmod() in <sys/stat.h> but provides no implementation for the
 * RXDK target (FATX has no POSIX permission bits). libzip only calls chmod() on the
 * archive *write* commit path (zip_source_file_stdio_named.c, when restoring the mode
 * of the temporary output file before the rename). On Xbox this is a no-op, so provide
 * a weak definition that satisfies the link. Declared weak so a real libc chmod wins if
 * one is ever linked.
 */
#include <sys/stat.h>

__attribute__((weak)) int chmod(const char *path, mode_t mode) {
    (void)path;
    (void)mode;
    return 0;
}
