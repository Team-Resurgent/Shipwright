import glob, json, os
os.chdir(r'D:/Git/Shipwright/xbox/vendor/libzip')

# libzip 1.11.4, zlib-only read+write feature set for the RXDK/Xbox (picolibc) target.
# We compile the platform-clean core plus the POSIX file/random backends, and exclude:
#   * alternate compressors    : zip_algorithm_bzip2/xz/zstd.c  (HAVE_LIB* all off)
#   * every encryption backend : zip_crypto_*.c, zip_winzip_aes*.c, zip_source_winzip_aes_*.c
#   * Windows file/random src   : zip_source_file_win32*.c, zip_random_win32/uwp.c
# zlib (deflate/inflate/crc/adler) is NOT compiled here -- libzip only #include <zlib.h>
# and its zlib symbols are satisfied at final link by StormLib's vendored zlib 1.3.1
# (so exactly one zlib definition ends up in the app; see includePaths below).
EXCLUDE = {
    'zip_algorithm_bzip2.c', 'zip_algorithm_xz.c', 'zip_algorithm_zstd.c',
    'zip_crypto_commoncrypto.c', 'zip_crypto_gnutls.c', 'zip_crypto_mbedtls.c',
    'zip_crypto_openssl.c', 'zip_crypto_win.c',
    'zip_winzip_aes.c', 'zip_source_winzip_aes_decode.c', 'zip_source_winzip_aes_encode.c',
    'zip_source_file_win32.c', 'zip_source_file_win32_ansi.c', 'zip_source_file_win32_named.c',
    'zip_source_file_win32_utf16.c', 'zip_source_file_win32_utf8.c',
    'zip_random_win32.c', 'zip_random_uwp.c',
}
srcs = []
for f in glob.glob('src/*.c'):
    base = os.path.basename(f.replace('\\', '/'))
    if base in EXCLUDE:
        continue
    srcs.append('src/' + base)
srcs = sorted(set(srcs))
print('total sources:', len(srcs))

manifest = {
    "name": "libzip-xbox",
    "type": "library",
    "defaultConfiguration": "Release",
    "configurations": {}
}
for cfg, conf in [("Release", "release"), ("Debug", "debug")]:
    manifest["configurations"][cfg] = {
        "type": "library",
        "configuration": conf,
        "sources": srcs,
        "publicIncludePaths": ["src"],
        "includePaths": [
            "../../compat", "src",
            # zlib.h/zconf.h from StormLib's vendored zlib 1.3.1 (headers only; the zlib
            # objects come from stormlib-xbox.lib at the app link, never recompiled here).
            "../StormLib/src/zlib",
        ],
        "compileFlags": ["-include", "libzip_xbox.h", "-Wno-everything",
                         "-U_WIN32", "-U_WIN64", "-U__MINGW32__", "-U__MINGW64__"],
        "createIso": False,
        "outputDir": "out/" + cfg
    }
with open('rxdk.project.json', 'w') as fh:
    json.dump(manifest, fh, indent=2)
print('wrote', os.path.abspath('rxdk.project.json'))
