#!/usr/bin/env python3
"""Generate xbox/rxdk.project.json for the libultraship-xbox Release library.

First pass: the platform-clean core. Excludes subsystems we replace/defer (SDL/GL/DX/
Metal backends, window, gui, controller input, audio-until-XboxAudioPlayer, scripting,
mobile, libzip O2R). Expand the exclude list shrinks as backends land.
"""
import glob, json, os

ROOT = r'D:/Git/Shipwright'
os.chdir(ROOT)

# Exclude by path substring (posix slashes).
EXCLUDE_DIRS = [
    'libultraship/src/fast/backends/',       # GL/DX/Metal/SDL/prism backends -> gfx_xbox_d3d8 later
    'libultraship/src/libultraship/window/', # gui bridge windows
    'libultraship/src/ship/controller/',     # input -> Phase 2 (XInput); hand-stubbed inert for now
    'libultraship/src/libultraship/controller/',
    'libultraship/src/ship/scripting/',      # dlopen mod loader
    'libultraship/src/ship/port/',           # mobile
]
EXCLUDE_FILES = [
    # Real ImGui (v1.91.9b docking) is compiled (see `vendor` below) so the whole Gui layer
    # builds for real; only the per-GPU ImGui *render backend* is a no-op on Xbox (menus run
    # but aren't rasterized until the D3D8 ImGui backend is wired). So Gui.cpp/GuiWindow.cpp/
    # GameOverlay.cpp/ConsoleWindow.cpp/Fast3dGui.cpp are INCLUDED now.
    # Bridges fronting deferred SDL subsystems (window/controller/gui/scripting):
    'libultraship/src/libultraship/bridge/controllerbridge.cpp', # input -> Phase 2 XInput
    'libultraship/src/libultraship/bridge/scriptingbridge.cpp',   # dlopen mod loader
    'libultraship/src/ship/debug/CrashHandler.cpp',           # CrashHandler.h pulls SDL
    # Audio: keep the inert base (Audio.cpp gated to NullAudioPlayer) + AudioPlayer + Null;
    # the SDL/Wasapi/CoreAudio players and SoundMatrixDecoder (SDL) are deferred.
    'libultraship/src/ship/audio/SDLAudioPlayer.cpp',
    'libultraship/src/ship/audio/WasapiAudioPlayer.cpp',
    'libultraship/src/ship/audio/CoreAudioAudioPlayer.cpp',
    # os_cache.cpp's 4 cache no-ops (osWritebackDCache{,All}/osInval{D,I}Cache) are all also
    # defined by soh/soh/stubs.c; let SoH's comprehensive libultra stub set own them so the
    # title link has a single definition (else duplicate-symbol in the projectRef group).
    'libultraship/src/libultraship/libultra/os_cache.cpp',
]

def excluded(p):
    p = p.replace('\\', '/')
    return any(d in p for d in EXCLUDE_DIRS) or p in EXCLUDE_FILES

lus = [p.replace('\\', '/') for p in (glob.glob('libultraship/src/**/*.cpp', recursive=True)
                                      + glob.glob('libultraship/src/**/*.c', recursive=True))
       if not excluded(p)]
lus.sort()

# Vendored leaf deps already proven to compile.
vendor = [
    'vendor/stb/stb_image_impl.c',
    'vendor/monocypher/monocypher.c',
    'vendor/monocypher/monocypher-ed25519.c',
    'vendor/tinyxml2/tinyxml2.cpp',
    # O2rArchive.cpp (the real one) is compiled from libultraship/src via the LUS glob now
    # that libzip is vendored (see includePaths + projectReferences below).
    # Real Dear ImGui v1.91.9b (docking) core — self-contained C++, no platform/renderer
    # backend. The whole SoH Gui layer builds against it; the D3D8 ImGui render backend is
    # a no-op for now (menus run, aren't drawn). Fetched into xbox/vendor/imgui.
    'vendor/imgui/imgui.cpp',
    'vendor/imgui/imgui_draw.cpp',
    'vendor/imgui/imgui_tables.cpp',
    'vendor/imgui/imgui_widgets.cpp',
]
# Xbox-native Fast3D backends (live in the excluded fast/backends/ dir; add explicitly).
xbox_backends = [
    'libultraship/src/fast/backends/gfx_xbox_window.cpp',
    'libultraship/src/fast/backends/gfx_xbox_d3d8.cpp',
]
lus += xbox_backends
lus.sort()
# libultraship sources are referenced relative to the xbox/ project root -> ../libultraship/...
sources = vendor + ['../' + p for p in lus]
print('libultraship core cpp:', len(lus), '| total sources:', len(sources))

inc = [
    '../libultraship/include', '../libultraship/src',
    'compat', 'vendor', 'vendor/imgui', 'vendor/stb', 'vendor/monocypher',
    'vendor/tinyxml2', 'vendor/thread-pool', 'vendor/StormLib/src',
    'vendor/libzip/src',  # <zip.h> for the real O2rArchive.cpp
]
manifest = {
    "name": "libultraship-xbox",
    "type": "library",
    "defaultConfiguration": "Release",
    "configurations": {}
}
for cfg, conf in [("Release", "release"), ("Debug", "debug")]:
    manifest["configurations"][cfg] = {
        "type": "library",
        "configuration": conf,
        "sources": sources,
        "publicIncludePaths": ['../libultraship/include'],
        "includePaths": inc,
        "projectReferences": ["vendor/StormLib", "vendor/libzip"],
        "defines": ["LUS_XBOX=1", "F3DEX_GBI_2", "CONTROLLERBUTTONS_T=uint32_t",
                    "INCLUDE_MPQ_SUPPORT", "_LANGUAGE_C_PLUS_PLUS",
                    "_POSIX_C_SOURCE=200809L", "__HAVE_POSIX_LOCALE_API=1"],
        "compileFlags": ["-include", "lus_cvars.h", "-Wno-everything"],
        "cppStandard": "c++23",
        "exceptions": True,
        "createIso": False,
        "outputDir": "out/" + cfg
    }
with open('xbox/rxdk.project.json', 'w') as fh:
    json.dump(manifest, fh, indent=2)
print('wrote xbox/rxdk.project.json')
