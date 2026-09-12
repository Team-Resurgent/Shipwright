// Inert platform stubs for the Ship of Harkinian Xbox boot-first build.
//
// These satisfy the link for subsystems that are deferred on the original Xbox (desktop ROM
// extractor, SDL_net netplay, custom-music opus, SDL desktop calls, mouse, gamepad input).
// Each is an inert no-op returning a safe default so the game boots and renders via Fast3D;
// real implementations (XInput controls, DirectSound audio, etc.) land in later phases.
#include <cstddef>

// ---------------------------------------------------------------------------
// SDL desktop calls (declared in xbox/compat/SDL2/SDL.h, no SDL runtime on Xbox)
// ---------------------------------------------------------------------------
#include <SDL2/SDL.h>

extern "C" {
int SDL_ShowSimpleMessageBox(Uint32, const char*, const char*, SDL_Window*) {
    return 0;
}
int SDL_OpenURL(const char*) {
    return 0;
}
SDL_bool SDL_SetHint(const char*, const char*) {
    return SDL_FALSE;
}

// SDL_net (declared in xbox/compat/SDL2/SDL_net.h) — netplay deferred.
int SDLNet_Init(void) {
    return 0;
}
void SDLNet_Quit(void) {
}

// POSIX process spawn used by the desktop file-dialog helper; never called on Xbox.
int execvp(const char*, char* const[]) {
    return -1;
}
} // extern "C"

// ---------------------------------------------------------------------------
// opusfile (custom music) — declared in xbox/compat/opusfile.h
// ---------------------------------------------------------------------------
#include <opusfile.h>

extern "C" {
OggOpusFile* op_open_memory(const unsigned char*, size_t, int* error) {
    if (error) {
        *error = -1;
    }
    return nullptr;
}
void op_free(OggOpusFile*) {
}
int op_read(OggOpusFile*, short*, int, int*) {
    return 0;
}
long op_pcm_seek(OggOpusFile*, long long) {
    return -1;
}
} // extern "C"

// ---------------------------------------------------------------------------
// soh mouse controls (soh/soh/Enhancements/controls/Mouse.h is in the excluded controls tree)
// ---------------------------------------------------------------------------
extern "C" {
void Mouse_UpdateAll(void) {
}
void Mouse_HandleThirdPerson(float*, float*) {
}
} // extern "C"
