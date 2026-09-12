// RXDK/Xbox build config (hand-written stand-in for the CMake-generated install_config.h).
// The Xbox port is portable (assets on disc / T:), so NON_PORTABLE and DISABLE_DLL_LOADER
// are left undefined; CMAKE_INSTALL_PREFIX is unused on this path.
#pragma once
#define CMAKE_INSTALL_PREFIX ""
