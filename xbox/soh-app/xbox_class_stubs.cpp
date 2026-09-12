// Inert C++ class stubs for the Ship of Harkinian Xbox boot-first build.
//
// The Xbox library manifests drop whole subsystems that are deferred on the original
// Xbox (see gen_lus_manifest.py / gen_sohsoh_manifest.py):
//   - libultraship/src/ship/controller/ + libultraship/controller/  (SDL gamepad input)
//   - libultraship/src/libultraship/window/                          (Gfx debugger window)
//   - libultraship/src/ship/debug/CrashHandler.cpp                   (SDL crash handler)
//   - soh/soh/Network/        (SDL_net netplay: Network base + Anchor/CrowdControl/Sail)
//   - soh/soh/Extractor/      (desktop ROM -> o2r tool; we pre-generate assets on PC)
//   - soh/soh/Enhancements/controls/ (InputViewer / SohInputEditor windows)
//   - soh/soh/resource/importer/AudioSampleFactory.cpp (dr/ogg/vorbis sample codecs)
//
// The title still *references* a handful of symbols from those translation units. This
// file provides inert no-op definitions so the final executable links and boots; each
// returns a safe default (nullptr / empty / false / do-nothing). Real implementations
// (XInput controls, DirectSound audio, etc.) land in later phases.
//
// vtable emission note: for the deferred polymorphic classes the whole defining .cpp is
// dropped, so their vtable (and, where the base is also dropped, the base's vtable) is
// undefined. Defining a class's out-of-line virtual functions here causes the compiler
// to emit that class's vtable/typeinfo in this translation unit. Because the original
// .cpp is excluded from the link there is no duplicate definition.
//
// Xbox-only; no LUS_XBOX guards needed. Include paths mirror soh-src/rxdk.project.json.

#include <cstdint>
#include <memory>
#include <string>
#include <vector>
#include <atomic>
#include <unordered_map>

// ---------------------------------------------------------------------------
// Controller subsystem (libultraship/src/ship/controller/ + libultraship/controller/)
// ---------------------------------------------------------------------------
#include <ship/controller/controldeck/ControlDeck.h>
#include <libultraship/controller/controldeck/ControlDeck.h>
#include <ship/controller/controldevice/controller/Controller.h>
#include <ship/controller/controldevice/controller/ControllerButton.h>
#include <ship/controller/controldevice/controller/ControllerLED.h>
#include <ship/controller/controldevice/controller/ControllerRumble.h>
#include <ship/controller/physicaldevice/ConnectedPhysicalDeviceManager.h>
#include <ship/controller/physicaldevice/SDLAddRemoveDeviceEventHandler.h>

namespace Ship {

// --- ControlDeck (abstract base; 3-arg ctor is needed by LUS::ControlDeck below) ---
ControlDeck::ControlDeck(std::vector<CONTROLLERBUTTONS_T> additionalBitmasks,
                         std::shared_ptr<ControllerDefaultMappings> controllerDefaultMappings,
                         std::unordered_map<CONTROLLERBUTTONS_T, std::string> buttonNames) {
    (void)additionalBitmasks;
    (void)controllerDefaultMappings;
    (void)buttonNames;
}
ControlDeck::~ControlDeck() {
}
void ControlDeck::Init(uint8_t* controllerBits) {
    mControllerBits = controllerBits;
}
std::shared_ptr<Controller> ControlDeck::GetControllerByPort(uint8_t port) {
    (void)port;
    return nullptr;
}
std::shared_ptr<ConnectedPhysicalDeviceManager> ControlDeck::GetConnectedPhysicalDeviceManager() {
    return nullptr;
}
bool ControlDeck::ProcessKeyboardEvent(KbEventType eventType, KbScancode scancode) {
    (void)eventType;
    (void)scancode;
    return false;
}
bool ControlDeck::ProcessMouseButtonEvent(bool isPressed, MouseBtn button) {
    (void)isPressed;
    (void)button;
    return false;
}

// --- Controller ---
std::shared_ptr<ControllerButton> Controller::GetButton(uint32_t bitmask) {
    (void)bitmask;
    return nullptr;
}
std::shared_ptr<ControllerLED> Controller::GetLED() {
    return nullptr;
}
std::shared_ptr<ControllerRumble> Controller::GetRumble() {
    return nullptr;
}

// --- ControllerButton ---
void ControllerButton::AddButtonMapping(std::shared_ptr<ControllerButtonMapping> mapping) {
    (void)mapping;
}
std::unordered_map<std::string, std::shared_ptr<ControllerButtonMapping>> ControllerButton::GetAllButtonMappings() {
    return {};
}

// --- ControllerLED ---
void ControllerLED::SetLEDColor(Color_RGB8 color) {
    (void)color;
}

// --- ControllerRumble ---
std::unordered_map<std::string, std::shared_ptr<ControllerRumbleMapping>> ControllerRumble::GetAllRumbleMappings() {
    return {};
}
void ControllerRumble::StartRumble() {
}
void ControllerRumble::StopRumble() {
}

// --- ConnectedPhysicalDeviceManager ---
std::unordered_map<int32_t, SDL_GameController*>
ConnectedPhysicalDeviceManager::GetConnectedSDLGamepadsForPort(uint8_t portIndex) {
    (void)portIndex;
    return {};
}

// --- SDLAddRemoveDeviceEventHandler (GuiWindow subclass; emit its vtable) ---
SDLAddRemoveDeviceEventHandler::~SDLAddRemoveDeviceEventHandler() {
}
void SDLAddRemoveDeviceEventHandler::InitElement() {
}
void SDLAddRemoveDeviceEventHandler::DrawElement() {
}
void SDLAddRemoveDeviceEventHandler::UpdateElement() {
}

} // namespace Ship

// --- LUS::ControlDeck (concrete N64 deck; emit its vtable via WriteToPad) ---
namespace LUS {
ControlDeck::ControlDeck(std::vector<CONTROLLERBUTTONS_T> additionalBitmasks)
    : Ship::ControlDeck(additionalBitmasks, nullptr, {}) {
    mPads = nullptr;
}
OSContPad* ControlDeck::GetPads() {
    static OSContPad pads[MAXCONTROLLERS] = {};
    return pads;
}
void ControlDeck::WriteToPad(void* pad) {
    (void)pad;
}
} // namespace LUS

// ---------------------------------------------------------------------------
// CrashHandler (libultraship/src/ship/debug/CrashHandler.cpp excluded)
// ---------------------------------------------------------------------------
#include <ship/debug/CrashHandler.h>

namespace Ship {
void CrashHandler::RegisterCallback(CrashHandlerCallback callback) {
    (void)callback;
}
} // namespace Ship

// ---------------------------------------------------------------------------
// Netplay (soh/soh/Network/ excluded, including the Network base)
// ---------------------------------------------------------------------------
#include "soh/Network/Network.h"
#include "soh/Network/Anchor/Anchor.h"
#include "soh/Network/CrowdControl/CrowdControl.h"
#include "soh/Network/Sail/Sail.h"

// Network base: define all virtuals so this TU emits `vtable for Network` + typeinfo,
// which the Anchor/CrowdControl/Sail vtables (and their inherited slots) reference.
void Network::OnIncomingData(char payload[512]) {
    (void)payload;
}
void Network::OnIncomingJson(nlohmann::json payload) {
    (void)payload;
}
void Network::OnConnected() {
}
void Network::OnDisconnected() {
}
void Network::ProcessOutgoingPackets() {
}
void Network::SendJsonToRemote(nlohmann::json packet) {
    (void)packet;
}

// Anchor: Enable/Disable + all overridden virtuals (emits `vtable for Anchor`).
void Anchor::Enable() {
}
void Anchor::Disable() {
}
void Anchor::OnIncomingJson(nlohmann::json payload) {
    (void)payload;
}
void Anchor::OnConnected() {
}
void Anchor::OnDisconnected() {
}
void Anchor::ProcessOutgoingPackets() {
}
void Anchor::SendJsonToRemote(nlohmann::json packet) {
    (void)packet;
}

// CrowdControl: Enable/Disable + overridden virtuals (emits `vtable for CrowdControl`).
void CrowdControl::Enable() {
}
void CrowdControl::Disable() {
}
void CrowdControl::OnIncomingJson(nlohmann::json payload) {
    (void)payload;
}
void CrowdControl::OnConnected() {
}
void CrowdControl::OnDisconnected() {
}

// Sail: Enable/Disable + overridden virtuals (emits `vtable for Sail`).
void Sail::Enable() {
}
void Sail::Disable() {
}
void Sail::OnIncomingJson(nlohmann::json payload) {
    (void)payload;
}
void Sail::OnConnected() {
}
void Sail::OnDisconnected() {
}

// AnchorRoomWindow: GuiWindow subclass (emit its vtable via DrawElement/Draw).
void AnchorRoomWindow::DrawElement() {
}
void AnchorRoomWindow::Draw() {
}

// ---------------------------------------------------------------------------
// Desktop ROM extractor (soh/soh/Extractor/ excluded)
// ---------------------------------------------------------------------------
#include "soh/Extractor/Extract.h"

bool Extractor::CallTorch(std::string installPath, std::string exportdir, std::atomic<size_t>* extractCount,
                          std::atomic<size_t>* totalExtract) {
    (void)installPath;
    (void)exportdir;
    (void)extractCount;
    (void)totalExtract;
    return false;
}
void Extractor::GetRoms(std::vector<std::string>& roms) {
    (void)roms;
}
bool Extractor::IsMasterQuest() const {
    return false;
}
bool Extractor::ManuallySearchForRomMatchingType(RomSearchMode searchMode) {
    (void)searchMode;
    return false;
}
bool Extractor::RunFileStandalone(std::string file) {
    (void)file;
    return false;
}
void Extractor::SetSearchPath(const std::string& path) {
    (void)path;
}
void Extractor::ShowErrorBox(const char* title, const char* text) {
    (void)title;
    (void)text;
}

// ---------------------------------------------------------------------------
// Debug / input-editor windows (soh/soh/Enhancements/controls/ excluded;
// libultraship/src/libultraship/window/ excluded)
// ---------------------------------------------------------------------------
#include <libultraship/window/gui/GfxDebuggerWindow.h>
#include "soh/Enhancements/controls/InputViewer.h"
#include "soh/Enhancements/controls/SohInputEditorWindow.h"

namespace LUS {
GfxDebuggerWindow::~GfxDebuggerWindow() {
}
void GfxDebuggerWindow::InitElement() {
}
void GfxDebuggerWindow::UpdateElement() {
}
void GfxDebuggerWindow::DrawElement() {
}
} // namespace LUS

// InputViewer (emit vtable via Draw/DrawElement; dtor is referenced directly).
void InputViewer::Draw() {
}
void InputViewer::DrawElement() {
}
InputViewer::~InputViewer() {
}

// InputViewerSettingsWindow (emit vtable via DrawElement; dtor referenced directly).
void InputViewerSettingsWindow::DrawElement() {
}
InputViewerSettingsWindow::~InputViewerSettingsWindow() {
}

// SohInputEditorWindow (emit vtable via ~dtor key function + element overrides).
SohInputEditorWindow::~SohInputEditorWindow() {
}
void SohInputEditorWindow::InitElement() {
}
void SohInputEditorWindow::DrawElement() {
}
void SohInputEditorWindow::UpdateElement() {
}
bool SohInputEditorWindow::TestingRumble() {
    return false;
}

// ---------------------------------------------------------------------------
// Audio sample resource factories (soh/soh/resource/importer/AudioSampleFactory.cpp excluded)
// ---------------------------------------------------------------------------
#include "soh/resource/importer/AudioSampleFactory.h"

namespace SOH {
std::shared_ptr<Ship::IResource>
ResourceFactoryBinaryAudioSampleV2::ReadResource(std::shared_ptr<Ship::File> file,
                                                 std::shared_ptr<Ship::ResourceInitData> initData) {
    (void)file;
    (void)initData;
    return nullptr;
}
std::shared_ptr<Ship::IResource>
ResourceFactoryXMLAudioSampleV0::ReadResource(std::shared_ptr<Ship::File> file,
                                              std::shared_ptr<Ship::ResourceInitData> initData) {
    (void)file;
    (void)initData;
    return nullptr;
}
} // namespace SOH
