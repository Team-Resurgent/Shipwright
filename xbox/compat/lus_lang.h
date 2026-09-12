// Force-included prelude for the RXDK/Xbox build. libultraship's N64 headers (gbi.h,
// gs2dex.h, ...) gate their `extern "C"`, the Gfx type, and the whole C data-structure
// section on _LANGUAGE_C / _LANGUAGE_C_PLUS_PLUS. gbi.h only auto-defines _LANGUAGE_C when
// _MSC_VER/__GNUC__ is set, which isn't reliable under our clang flags — so set the right
// one per language explicitly. Must be per-TU (soh is mixed C/C++), hence a force-include
// rather than a blanket -D.
#pragma once
#ifdef __cplusplus
#ifndef _LANGUAGE_C_PLUS_PLUS
#define _LANGUAGE_C_PLUS_PLUS 1
#endif
#else
#ifndef _LANGUAGE_C
#define _LANGUAGE_C 1
#endif
#endif
