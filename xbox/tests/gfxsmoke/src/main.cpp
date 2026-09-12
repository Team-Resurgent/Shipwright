// Phase-1b gfx smoke: drives the real Fast3D Xbox backend to draw a Gouraud RGB triangle
// through GfxRenderingAPIXbox::DrawTriangles (CPU clip->screen + fixed-function shading),
// on a dark background, presented every frame. Proves the device + draw path end to end in
// xemu without the full engine. The shader layout is configured directly (position + RGBA)
// so no combiner/OTR is needed. Textured + combiner fidelity come next.
#include <xtl.h>
#include <d3d8.h>
#include <cstdio>

#include "fast/backends/gfx_xbox_window.h"
#include "fast/backends/gfx_xbox_d3d8.h"

void __cdecl main() {
    printf("[gfxsmoke] start\n");

    Fast::GfxWindowBackendXbox window;
    window.Init("gfxsmoke", "Xbox D3D8", true, 640, 480, 0, 0);
    IDirect3DDevice8* dev = reinterpret_cast<IDirect3DDevice8*>(window.GetDevice());
    if (!dev) {
        printf("[gfxsmoke] FAIL: no D3D8 device\n");
        for (;;) {
            Sleep(100);
        }
    }
    printf("[gfxsmoke] D3D8 device created\n");

    Fast::GfxRenderingAPIXbox rapi(&window);
    rapi.Init();

    // Configure a trivial shader layout directly: per-vertex = position(4) + RGBA(4), no texture.
    Fast::ShaderProgram* sp = rapi.CreateAndLoadNewShader(1, 1);
    sp->numInputs = 1;
    sp->usedTextures[0] = false;
    sp->usedTextures[1] = false;
    sp->usesAlpha = true;
    sp->inputSize = 4;
    sp->uv0Off = -1;
    sp->input0Off = 4; // colour follows the 4 position floats
    sp->numFloats = 8; // stride
    rapi.LoadShader(sp);

    // One triangle in clip space (z=0.5, w=1 -> ndc z=0.5 within [0,1]); RGB corners.
    // Layout per vertex: x, y, z, w, r, g, b, a
    float tri[3 * 8] = {
        0.0f,  0.8f,  0.5f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, // top    - red
        -0.8f, -0.8f, 0.5f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f, // left   - green
        0.8f,  -0.8f, 0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, // right  - blue
    };

    printf("[gfxsmoke] entering draw loop\n");
    unsigned frame = 0;
    for (;;) {
        dev->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(24, 24, 48), 1.0f, 0);
        rapi.StartFrame();
        rapi.SetViewport(0, 0, 640, 480);
        rapi.SetDepthTestAndMask(false, false);
        rapi.SetUseAlpha(false);
        rapi.DrawTriangles(tri, 3 * 8, 1);
        rapi.EndFrame();
        window.SwapBuffersEnd();
        if ((frame++ % 60) == 0) {
            printf("[gfxsmoke] frame %u, t=%.2fs\n", frame, window.GetTime());
        }
    }
}
