#!/usr/bin/env python3
"""Generate xbox/soh-app/rxdk.project.json — the Ship of Harkinian Xbox executable.

This is the final link: the OoT decomp (soh-src), the SoH mods (soh-soh) and the
libultraship engine + D3D8 backend (libultraship-xbox, which transitively pulls
StormLib) are linked together into an XBE/ISO.

The decomp's soh/src/code/main.c is compiled HERE rather than into soh-src.lib: it
defines main()/Main() and the OS globals, and RXDK's XapiTitleStartup CRT is linked
after the projectReference group, so `main` must be an always-linked object (exe's own
sources) not an on-demand archive member. gen_soh_manifest.py excludes it from the lib.

Include paths / defines / compile flags mirror soh-src exactly (main.c is decomp C).
"""
import json, os

ROOT = r'D:/Git/Shipwright'
os.chdir(ROOT)

# The decomp entry translation unit (main/Main + OS globals), plus the Xbox boot shim.
sources = [
    '../../soh/src/code/main.c',
    'xbox_boot.cpp',
    'build.c',          # gBuildVersion/gGit* constants (CMake build.c.in stand-in)
    'xbox_stubs.cpp',   # inert platform stubs: Extractor/netplay/SDL/opus leaves (deferred)
    'xbox_class_stubs.cpp',  # inert C++ class stubs: deferred controller/netplay/window/audio-factory vtables
]

# Same include surface as the decomp library (main.c is decomp C); relative to xbox/soh-app/.
inc = [
    '../../soh/include', '../../soh/src', '../../soh', '../../soh/assets', '../../soh/soh',
    '../../libultraship/include', '../../libultraship/src',
    '../compat', '../vendor', '../vendor/imgui', '../vendor/StormLib/src', '../vendor/thread-pool',
    '../vendor/libzip/src',  # <zip.h> for TUs that transitively include O2rArchive.h
]
defines = [
    'LUS_XBOX=1', 'F3DEX_GBI_2', 'CONTROLLERBUTTONS_T=uint32_t', 'INCLUDE_MPQ_SUPPORT',
    '_GNU_SOURCE',
    'NDEBUG', '_CONSOLE', '_CRT_SECURE_NO_WARNINGS', 'UNICODE', '_UNICODE',
    'SPDLOG_NO_THREAD_ID', 'SPDLOG_NO_TLS', 'STBI_NO_THREAD_LOCALS',
    'SPDLOG_ACTIVE_LEVEL=0', 'LOG_LEVEL_GAME_PRINTS=6',
]
compile_flags = ["-include", "lus_lang.h", "-include", "lus_cvars.h", "-Wno-everything",
                 "-U_WIN32", "-U_WIN64", "-U__MINGW32__", "-U__MINGW64__",
                 "-fno-ms-compatibility"]

# projectReferences: the three game libraries. libultraship-xbox (xbox/ root, i.e. "..")
# already references vendor/StormLib, so that comes in transitively. The engine wraps
# the whole set in --start-group/--end-group, so their circular deps resolve.
project_refs = ['..', '../soh-src', '../soh-soh']

# RXDK system libraries. libxapi selects the XapiTitleStartup entry (CRT -> main); the
# renderer is libd3d8/libd3dx8/libxgraphics, audio is libdsound (XboxAudioPlayer).
libraries = ['libxapi', 'libd3d8', 'libd3dx8', 'libxgraphics', 'libdsound',
             'libkernel', 'libc', 'libcpp']

SAMPLE_COMMON = 'C:/ProgramData/RXDK/samples/RxdkSamples/Common'
image_build = {
    # The engine + STL + InitOTR run on the main thread; 64KB (sample default) is far too
    # small. 1 MB main-thread stack. Game worker threads allocate their own stacks.
    "stackSize": 1024 * 1024,
    "debug": True,
    "noLogo": True,
    "noLibWarn": True,
    # 128 MB profile first (plan: 64 MB = retail, 128 MB = debug/dev). xemu-devkit is 128 MB.
    "limitMemory": False,
    "dontModifyHardDisk": False,
    "dontMountUtilityDrive": True,
    "formatUtilityDrive": False,
    "testId": "0x534f4801",
    "testName": "ShipOfHarkinian",
    "testVersion": "4096",
    "titleImage": SAMPLE_COMMON + "/rxdk-titleimage.xbx",
    "defaultSaveImage": SAMPLE_COMMON + "/rxdk-saveimage.xbx",
}

manifest = {
    "name": "soh-app",
    "defaultConfiguration": "Release",
    "configurations": {}
}
for cfg, conf in [("Release", "release"), ("Debug", "debug")]:
    manifest["configurations"][cfg] = {
        "configuration": conf,
        "sources": sources,
        "includePaths": inc,
        "defines": defines,
        "compileFlags": compile_flags,
        "projectReferences": project_refs,
        "libraries": libraries,
        "cppStandard": "c++23",
        "createIso": True,
        "imageBuild": image_build,
        # Stage the game/port asset archives at the ISO root (D:\), next to default.xbe, where
        # GetAppDirectoryPath()=="D:" resolves them. Files are copied into the project dir so
        # their deploy destination is the bare filename (deployPaths strips ./.. segments).
        # oot-mq.o2r is the extracted game data; soh.o2r is the port assets (built by the packer).
        "deployPaths": ["oot-mq.o2r", "soh.o2r"],
        "outputDir": "out/" + cfg,
    }

os.makedirs('xbox/soh-app', exist_ok=True)
with open('xbox/soh-app/rxdk.project.json', 'w') as fh:
    json.dump(manifest, fh, indent=2)
print('wrote xbox/soh-app/rxdk.project.json  sources:', len(sources))
