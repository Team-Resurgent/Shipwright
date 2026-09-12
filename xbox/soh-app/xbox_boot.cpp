// Xbox boot shim for Ship of Harkinian.
//
// The decomp's main.c supplies the real entry (main -> InitOTR -> Main). This TU holds
// Xbox-only bring-up helpers that the rest of the port queries — chiefly the physical RAM
// size, which drives the texture-cache budget and the 64 MB vs 128 MB behaviour split
// described in docs/XBOX_PORT_PLAN.md (retail kits are 64 MB; debug/dev kits 128 MB).
#include <xtl.h>
#include <cstdio>

// Boot-bring-up trace: runs during C++ static init, before main(). Confirms the CRT +
// static-init path executes and the printf->serial channel is live this early.
namespace {
struct XboxBootTrace {
    XboxBootTrace() {
        std::printf("[xbox] static-init reached\n");
        std::fflush(stdout);
    }
};
XboxBootTrace g_xboxBootTrace;
} // namespace

extern "C" {

// Installed physical RAM, in MB. Retail Xbox = 64 MB; debug/dev kits = 128 MB.
// Cached after the first call. The kernel reports total physical pages (4 KB each).
unsigned int Xbox_GetPhysicalMemoryMB(void) {
    static unsigned int cached = 0;
    if (cached != 0) {
        return cached;
    }
    MM_STATISTICS stats;
    stats.Length = sizeof(stats);
    MmQueryStatistics(&stats);
    // Pages are 4 KB; round to the nearest whole MB, then snap to the real bank sizes.
    unsigned int mb = (stats.TotalPhysicalPages * 4u + 512u) / 1024u;
    cached = (mb > 64u) ? 128u : 64u;
    return cached;
}

// Convenience predicate used by memory-budget decisions.
int Xbox_IsLowMemoryKit(void) {
    return Xbox_GetPhysicalMemoryMB() <= 64u;
}

} // extern "C"
