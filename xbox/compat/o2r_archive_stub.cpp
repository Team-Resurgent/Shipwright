// Stub Ship::O2rArchive for the OTR-only Xbox build. The real O2rArchive.cpp needs libzip,
// which we don't vendor (we load OTR/MPQ via StormLib). ArchiveManager still references the
// O2rArchive symbols (it constructs one when asked to open a .o2r/.zip), so provide inert
// implementations — they resolve the link but are never reached on the OTR path. If O2R
// support is ever wanted, drop in real libzip + the upstream O2rArchive.cpp instead.
#include "ship/resource/archive/O2rArchive.h"

namespace Ship {

O2rArchive::O2rArchive(const std::string& archivePath) : Archive(archivePath) {
}
O2rArchive::~O2rArchive() {
}
bool O2rArchive::Open() {
    return false;
}
bool O2rArchive::Close() {
    return true;
}
bool O2rArchive::WriteFile(const std::string& filename, const std::vector<uint8_t>& data) {
    (void)filename;
    (void)data;
    return false;
}
std::shared_ptr<File> O2rArchive::LoadFile(const std::string& filePath) {
    (void)filePath;
    return nullptr;
}
std::shared_ptr<File> O2rArchive::LoadFile(uint64_t hash) {
    (void)hash;
    return nullptr;
}
zip_t* O2rArchive::GetZipHandle() {
    return nullptr;
}
void O2rArchive::ReleaseZipHandle(zip_t* handle) {
    (void)handle;
}
} // namespace Ship
