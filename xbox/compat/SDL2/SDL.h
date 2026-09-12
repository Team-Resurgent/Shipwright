// INTERIM SDL2 type stub for the RXDK/Xbox libultraship build.
//
// NOT a functional SDL. libultraship's classes.h umbrella pulls window/controller/gui
// headers that declare SDL types in class members, so the whole library needs these
// TYPES to parse. The SDL .cpp implementations are excluded from the Xbox build; the
// real backends replace them per-subsystem (window -> libd3d8, input -> XInput) in
// Phase 1/2, at which point this stub is removed. See docs/XBOX_PORT_PLAN.md.
#pragma once
#include <stdint.h>
#include "SDL2/SDL_stdinc.h" // Uint8..Uint64, Sint*, SDL_bool/SDL_TRUE/SDL_FALSE

typedef uint8_t  Uint8;
typedef uint16_t Uint16;
typedef uint32_t Uint32;
typedef uint64_t Uint64;
typedef int8_t   Sint8;
typedef int16_t  Sint16;
typedef int32_t  Sint32;
typedef int64_t  Sint64;

typedef uint32_t SDL_AudioDeviceID;
typedef void* SDL_GLContext;

typedef struct SDL_Window SDL_Window;
typedef struct SDL_Renderer SDL_Renderer;
typedef struct SDL_GameController SDL_GameController;

typedef struct SDL_Rect { int x, y, w, h; } SDL_Rect;

typedef enum SDL_GameControllerButton {
    SDL_CONTROLLER_BUTTON_INVALID = -1,
    SDL_CONTROLLER_BUTTON_A, SDL_CONTROLLER_BUTTON_B, SDL_CONTROLLER_BUTTON_X, SDL_CONTROLLER_BUTTON_Y,
    SDL_CONTROLLER_BUTTON_BACK, SDL_CONTROLLER_BUTTON_GUIDE, SDL_CONTROLLER_BUTTON_START,
    SDL_CONTROLLER_BUTTON_LEFTSTICK, SDL_CONTROLLER_BUTTON_RIGHTSTICK,
    SDL_CONTROLLER_BUTTON_LEFTSHOULDER, SDL_CONTROLLER_BUTTON_RIGHTSHOULDER,
    SDL_CONTROLLER_BUTTON_DPAD_UP, SDL_CONTROLLER_BUTTON_DPAD_DOWN,
    SDL_CONTROLLER_BUTTON_DPAD_LEFT, SDL_CONTROLLER_BUTTON_DPAD_RIGHT,
    SDL_CONTROLLER_BUTTON_MISC1, SDL_CONTROLLER_BUTTON_PADDLE1, SDL_CONTROLLER_BUTTON_PADDLE2,
    SDL_CONTROLLER_BUTTON_PADDLE3, SDL_CONTROLLER_BUTTON_PADDLE4, SDL_CONTROLLER_BUTTON_TOUCHPAD,
    SDL_CONTROLLER_BUTTON_MAX
} SDL_GameControllerButton;

typedef enum SDL_GameControllerAxis {
    SDL_CONTROLLER_AXIS_INVALID = -1,
    SDL_CONTROLLER_AXIS_LEFTX, SDL_CONTROLLER_AXIS_LEFTY,
    SDL_CONTROLLER_AXIS_RIGHTX, SDL_CONTROLLER_AXIS_RIGHTY,
    SDL_CONTROLLER_AXIS_TRIGGERLEFT, SDL_CONTROLLER_AXIS_TRIGGERRIGHT,
    SDL_CONTROLLER_AXIS_MAX
} SDL_GameControllerAxis;

typedef enum { SDL_CONTROLLERDEVICEADDED = 0x653, SDL_CONTROLLERDEVICEREMOVED } SDL_EventType;
typedef struct SDL_Event { uint32_t type; uint8_t padding[56]; } SDL_Event;

// --- Extended SDL surface for soh/soh controller enhancements (types only; real input
//     is XInput in the Xbox controller backend). ---
typedef struct SDL_Joystick SDL_Joystick;
typedef int32_t SDL_JoystickID;
typedef enum SDL_GameControllerBindType {
    SDL_CONTROLLER_BINDTYPE_NONE = 0,
    SDL_CONTROLLER_BINDTYPE_BUTTON,
    SDL_CONTROLLER_BINDTYPE_AXIS,
    SDL_CONTROLLER_BINDTYPE_HAT
} SDL_GameControllerBindType;
#ifndef SDL_JOYSTICK_AXIS_MIN
#define SDL_JOYSTICK_AXIS_MIN (-32768)
#endif
#ifndef SDL_JOYSTICK_AXIS_MAX
#define SDL_JOYSTICK_AXIS_MAX 32767
#endif

// --- SDL GameController string/hat API used by soh/soh/Enhancements/controls (Mapper.cpp).
//     Declarations only; the real controller layer is XInput on Xbox. ---
#ifndef SDL_VERSION_ATLEAST
#define SDL_VERSION_ATLEAST(x, y, z) 0
#endif
#define SDL_HAT_CENTERED  0x00
#define SDL_HAT_UP        0x01
#define SDL_HAT_RIGHT     0x02
#define SDL_HAT_DOWN      0x04
#define SDL_HAT_LEFT      0x08
#define SDL_HAT_RIGHTUP   (SDL_HAT_RIGHT | SDL_HAT_UP)
#define SDL_HAT_RIGHTDOWN (SDL_HAT_RIGHT | SDL_HAT_DOWN)
#define SDL_HAT_LEFTUP    (SDL_HAT_LEFT | SDL_HAT_UP)
#define SDL_HAT_LEFTDOWN  (SDL_HAT_LEFT | SDL_HAT_DOWN)
#ifdef __cplusplus
extern "C" {
#endif
SDL_GameControllerAxis SDL_GameControllerGetAxisFromString(const char* str);
SDL_GameControllerButton SDL_GameControllerGetButtonFromString(const char* str);
const char* SDL_GameControllerGetStringForAxis(SDL_GameControllerAxis axis);
const char* SDL_GameControllerGetStringForButton(SDL_GameControllerButton button);
const char* SDL_GetPlatform(void);
#ifdef __cplusplus
}
#endif

// --- SDL message box + misc used by OTRGlobals (declarations; inert on Xbox). ---
#define SDL_MESSAGEBOX_ERROR       0x00000010
#define SDL_MESSAGEBOX_WARNING     0x00000020
#define SDL_MESSAGEBOX_INFORMATION 0x00000040
#ifdef __cplusplus
extern "C" {
#endif
int SDL_ShowSimpleMessageBox(Uint32 flags, const char* title, const char* message, SDL_Window* window);
Uint32 SDL_GetTicks(void);
int SDL_GameControllerAddMappingsFromFile(const char* file);
int SDL_GameControllerAddMapping(const char* mappingString);
#ifdef __cplusplus
}
#endif

// --- SDL hints + misc desktop calls used by soh/soh settings/menus (inert on Xbox). ---
#define SDL_HINT_JOYSTICK_ALLOW_BACKGROUND_EVENTS "SDL_JOYSTICK_ALLOW_BACKGROUND_EVENTS"
#define SDL_HINT_JOYSTICK_THREAD "SDL_JOYSTICK_THREAD"
#ifdef __cplusplus
extern "C" {
#endif
int SDL_OpenURL(const char* url);
SDL_bool SDL_SetHint(const char* name, const char* value);
int SDL_Init(Uint32 flags);
const char* SDL_GetError(void);
#ifdef __cplusplus
}
#endif
