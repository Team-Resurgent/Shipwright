#!/usr/bin/env python3
"""Generate xbox/soh-src/rxdk.project.json — the OoT decomp (soh/src) as a static library.

Built as type:library so all 803 C files compile without needing SoH's C++ glue (OTRGlobals
etc.) to link yet — a fast way to shake out the shared-header gates at scale. The soh/soh
mods + final executable link come after. Defines mirror soh/CMakeLists.txt (Clang path),
minus ENABLE_OPENGL (we're D3D8). Language macros come via the lus_lang.h force-include.
"""
import glob, json, os

ROOT = r'D:/Git/Shipwright'
os.chdir(ROOT)

# The decomp is C. (A handful of .cpp exist under soh/src; include them too.)
all_c = [p.replace('\\', '/') for p in
         (glob.glob('soh/src/**/*.c', recursive=True) + glob.glob('soh/src/**/*.cpp', recursive=True))]

# Mirror soh/CMakeLists.txt source filtering: these subtrees are N64-hardware / host
# facilities that libultraship replaces (raw AI/PI/VI io, libultra os, libc, rmon), plus
# a set of gu/ math files that have replacements. Excluding them is required to build.
EXCLUDE_RE = ('soh/src/dmadata/', 'soh/src/elf_message/', 'soh/src/libultra/io/',
              'soh/src/libultra/libc/', 'soh/src/libultra/os/', 'soh/src/libultra/rmon/')
REMOVE_FILES = {'soh/src/libultra/gu/cosf.c', 'soh/src/libultra/gu/lookat.c',
                'soh/src/libultra/gu/lookathil.c', 'soh/src/libultra/gu/perspective.c',
                'soh/src/libultra/gu/position.c', 'soh/src/libultra/gu/sinf.c',
                'soh/src/libultra/gu/sqrtf.c', 'soh/src/libultra/gu/us2dex.c',
                # main.c defines main()/Main() + the OS globals; it is compiled into the
                # soh-app EXECUTABLE (not the archive) so the XapiTitleStartup CRT, which is
                # linked after the projectReference group, can resolve `main` as an always-
                # linked object rather than an on-demand archive member.
                'soh/src/code/main.c'}
srcs = [p for p in all_c
        if not any(d in p for d in EXCLUDE_RE) and p not in REMOVE_FILES]
srcs.append('soh/src/libultra/libc/sprintf.c')  # explicitly added back by SoH
srcs = sorted(set(srcs))
# Reference from xbox/soh-src/ project root -> ../../soh/...
sources = ['../../' + p for p in srcs]
print('soh/src sources:', len(sources))

# Include paths relative to the xbox/soh-src/ project root.
inc = [
    '../../soh/include', '../../soh/src', '../../soh', '../../soh/assets', '../../soh/soh',
    '../../libultraship/include', '../../libultraship/src',
    '../compat', '../vendor', '../vendor/imgui', '../vendor/StormLib/src',
]
defines = [
    'LUS_XBOX=1', 'F3DEX_GBI_2', 'CONTROLLERBUTTONS_T=uint32_t', 'INCLUDE_MPQ_SUPPORT',
    # picolibc gates POSIX/MISC extensions (strdup, strtok_r, etc.) behind feature
    # visibility; the decomp relies on them, so opt into the GNU surface like soh-soh.
    '_GNU_SOURCE',
    'NDEBUG', '_CONSOLE', '_CRT_SECURE_NO_WARNINGS', 'UNICODE', '_UNICODE',
    'SPDLOG_NO_THREAD_ID', 'SPDLOG_NO_TLS', 'STBI_NO_THREAD_LOCALS',
    'SPDLOG_ACTIVE_LEVEL=0', 'LOG_LEVEL_GAME_PRINTS=6',
]
manifest = {
    "name": "soh-src",
    "type": "library",
    "defaultConfiguration": "Release",
    "configurations": {}
}
for cfg, conf in [("Release", "release"), ("Debug", "debug")]:
    manifest["configurations"][cfg] = {
        "type": "library",
        "configuration": conf,
        "sources": sources,
        "includePaths": inc,
        "defines": defines,
        "compileFlags": ["-include", "lus_lang.h", "-include", "lus_cvars.h", "-Wno-everything",
                         "-U_WIN32", "-U_WIN64", "-U__MINGW32__", "-U__MINGW64__",
                         # SoH gates asset includes (and gbi.h's _LANGUAGE_C) on
                         # defined(_MSC_VER)||defined(__GNUC__). The engine's base recipe adds
                         # -fms-compatibility, which suppresses clang's natural __GNUC__ on the
                         # gnu target; turn it back off here so __GNUC__ is defined again.
                         "-fno-ms-compatibility"],
        "cppStandard": "c++23",
        "createIso": False,
        "outputDir": "out/" + cfg
    }
os.makedirs('xbox/soh-src', exist_ok=True)
with open('xbox/soh-src/rxdk.project.json', 'w') as fh:
    json.dump(manifest, fh, indent=2)
print('wrote xbox/soh-src/rxdk.project.json')
