/* bzip2 requires the application to provide bz_internal_error() when built with
 * BZ_NO_STDIO (we strip bzip2's stdio/FILE* API for the Xbox build). Also provide
 * BZ2_bz__AssertH__fail, normally defined in bzlib.c's stdio path. StormLib only uses
 * bzip2's in-memory decompress for OTR reads; a fatal here indicates archive corruption. */
void bz_internal_error(int errcode) {
    (void)errcode;
    for (;;) {
    }
}
void BZ2_bz__AssertH__fail(int errcode) {
    (void)errcode;
    for (;;) {
    }
}
