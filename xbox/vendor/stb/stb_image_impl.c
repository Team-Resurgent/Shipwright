// stb_image implementation TU for the RXDK/Xbox libultraship build.
// libultraship only decodes PNGs from memory (stbi_load_from_memory), so drop the
// stdio path. Xbox CPU is a Pentium III (SSE1, no SSE2) — disable stb's SIMD, whose
// fast paths are SSE2-gated, to keep codegen safe on -march=pentium3.
#define STB_IMAGE_IMPLEMENTATION
#define STBI_NO_STDIO
#define STBI_NO_SIMD
#include "stb_image.h"
