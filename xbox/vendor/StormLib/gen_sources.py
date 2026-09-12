import glob, json, os
os.chdir(r'D:/Git/Shipwright/xbox/vendor/StormLib')

# Full StormLib source set except the wdk/ amalgamation files (which #include the other
# sources and would double-compile). Including the complete tree (core read+write, all
# decompressors under lzma/C, and the full libtomcrypt/libtommath) makes StormLib link
# with no dangling read-path references into write/verify/crypto.
srcs = []
for ext in ('*.cpp', '*.c'):
    for f in glob.glob('src/**/' + ext, recursive=True):
        p = f.replace('\\', '/')
        if '/wdk/' in p:
            continue
        # Multi-threaded LZMA (Win32 CRITICAL_SECTION/threads) — not needed for decode.
        base = os.path.basename(p)
        if base in ('LzFindMt.c', 'MtCoder.c', 'MtDec.c', 'Threads.c'):
            continue
        srcs.append(p)
srcs.append('bz_stubs.c')
srcs = sorted(set(srcs))
print('total sources:', len(srcs))

manifest = {
    "name": "stormlib-xbox",
    "type": "library",
    "defaultConfiguration": "Debug",
    "configurations": {}
}
for cfg, conf in [("Debug", "debug"), ("Release", "release")]:
    manifest["configurations"][cfg] = {
        "type": "library",
        "configuration": conf,
        "sources": srcs,
        "defines": ["BZ_NO_STDIO", "_7ZIP_ST"],
        "publicIncludePaths": ["src"],
        "includePaths": [
            "../../compat", "src",
            "src/libtomcrypt/src/headers", "src/libtommath",
            "src/zlib", "src/bzip2", "src/lzma/C",
        ],
        "compileFlags": ["-include", "stormlib_xbox.h", "-Wno-everything",
                         "-U_WIN32", "-U_WIN64", "-U__MINGW32__", "-U__MINGW64__"],
        "createIso": False,
        "outputDir": "out/" + cfg
    }
with open('rxdk.project.json', 'w') as fh:
    json.dump(manifest, fh, indent=2)
print('wrote', os.path.abspath('rxdk.project.json'))
