#!/usr/bin/env python3
"""Generate xbox/soh-soh/rxdk.project.json — the SoH C++ mods (soh/soh) as a static library.

type:library, to shake out compile gates at scale (like the decomp pass). Excludes the
desktop-only Extractor (ROM->o2r; we pre-generate on PC). Mirrors soh's define set +
the language prelude. Expect imgui/SDL/GameInteractor gates — this pass is to map them.
"""
import glob, json, os

ROOT = r'D:/Git/Shipwright'
os.chdir(ROOT)

EXCLUDE_RE = ('soh/soh/Extractor/', 'soh/soh/Enhancements/controls/', 'soh/soh/Network/')  # SDL input->XInput; SDL_net netplay deferred
EXCLUDE_FILES = ('soh/soh/Enhancements/speechsynthesizer/ESpeakSpeechSynthesizer.cpp', 'soh/soh/Enhancements/speechsynthesizer/SAPISpeechSynthesizer.cpp', 'soh/soh/resource/importer/AudioSampleFactory.cpp')  # custom-audio codecs (dr/ogg/vorbis) deferred  # desktop ROM->o2r tool, dropped on Xbox
all_src = [p.replace('\\', '/') for p in
           (glob.glob('soh/soh/**/*.cpp', recursive=True) + glob.glob('soh/soh/**/*.c', recursive=True))]
srcs = sorted(p for p in all_src if not any(d in p for d in EXCLUDE_RE) and p not in EXCLUDE_FILES)
print('soh/soh sources:', len(srcs))
sources = ['../../' + p for p in srcs]

inc = [
    '../../soh/include', '../../soh/src', '../../soh', '../../soh/assets', '../../soh/soh',
    '../../libultraship/include', '../../libultraship/src',
    '../compat', '../vendor', '../vendor/stb', '../vendor/imgui', '../vendor/nlohmann',
    '../vendor/tinyxml2', '../vendor/StormLib/src', '../vendor/thread-pool',
    '../vendor/libzip/src',  # <zip.h> (OTRGlobals/OotrsArchive pull in O2rArchive.h)
]
defines = [
    'LUS_XBOX=1', 'F3DEX_GBI_2', 'CONTROLLERBUTTONS_T=uint32_t', 'INCLUDE_MPQ_SUPPORT',
    'NDEBUG', '_CONSOLE', '_CRT_SECURE_NO_WARNINGS', 'UNICODE', '_UNICODE',
    'SPDLOG_NO_THREAD_ID', 'SPDLOG_NO_TLS', 'STBI_NO_THREAD_LOCALS',
    'SPDLOG_ACTIVE_LEVEL=0', 'LOG_LEVEL_GAME_PRINTS=6', 'IMGUI_DEFINE_MATH_OPERATORS',
]
manifest = {"name": "soh-soh", "type": "library", "defaultConfiguration": "Release", "configurations": {}}
for cfg, conf in [("Release", "release"), ("Debug", "debug")]:
    manifest["configurations"][cfg] = {
        "type": "library", "configuration": conf, "sources": sources,
        "includePaths": inc, "defines": defines,
        "compileFlags": ["-include", "lus_lang.h", "-include", "lus_cvars.h", "-Wno-everything",
                         "-U_WIN32", "-U_WIN64", "-U__MINGW32__", "-U__MINGW64__", "-fno-ms-compatibility"],
        "cppStandard": "c++23", "exceptions": True, "createIso": False, "outputDir": "out/" + cfg
    }
os.makedirs('xbox/soh-soh', exist_ok=True)
with open('xbox/soh-soh/rxdk.project.json', 'w') as fh:
    json.dump(manifest, fh, indent=2)
print('wrote xbox/soh-soh/rxdk.project.json')
