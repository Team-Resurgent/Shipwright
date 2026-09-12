// Xbox build-info constants — the stand-in for CMake-configured soh/src/boot/build.c.in
// (the Xbox build has no CMake configure step). Values are informational only (shown on the
// crash screen / file-select), not gameplay-affecting.
#include <libultraship/libultra.h>

// Must match the version the asset archives (soh.o2r / oot-mq.o2r) were packed with, or
// OTRGlobals treats them as outdated (sohArchiveVersionMatch / VerifyArchiveVersion) and
// drops into the ROM re-extract flow. The packer used the repo's project VERSION 9.2.3.
const char gBuildVersion[] = "Ship of Harkinian - Xbox (9.2.3)";
const u16 gBuildVersionMajor = 9;
const u16 gBuildVersionMinor = 2;
const u16 gBuildVersionPatch = 3;

const char gGitBranch[] = "xbox";
const char gGitCommitHash[] = "";
const char gGitCommitTag[] = "";

const char gBuildTeam[] = "RXDK";
const char gBuildDate[] = __DATE__ " " __TIME__;
const char gBuildMakeOption[] = "";
