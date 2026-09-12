# Ship of Harkinian → Original Xbox (RXDK) — Port Plan

**Target:** Original Xbox (NV2A GeForce3-class GPU, 733 MHz Pentium III, 64 MB retail / 128 MB devkit), built with the **RXDK** Clang/zig toolchain, packaged as an XBE inside an Xbox ISO, tested in `D:\Git\xemu-devkit\xemu.exe`.

**Renderer decision:** Implement `Fast::GfxRenderingAPI` over **Xbox Direct3D8 (`libd3d8`)** first, using Xbox pixel shaders (register-combiner programs). Native-NV2A pushbuffer is a later optimization, not the first cut.

**Guiding principle:** *Avoid CPU-bound work; push everything we can onto the NV2A and keep it GPU-resident.* The P3 is the scarcest resource after RAM. See [§6](#6-cpu-offload--gpu-caching-strategy).

---

## 1. Why this is tractable

- **SoH/libultraship is cleanly abstracted.** The whole platform surface is three virtual base classes plus an input layer. We implement backends, we don't rewrite the engine.
- **RXDK-Libs is now capable enough.** libc++ profile (`build/generated/libcxx/__config_site`) has `_LIBCPP_HAS_THREADS` (C11 `<threads.h>` → `libxapi` CreateThread, real 618-line impl in `libs/libc/xbox/threads.c`), `_LIBCPP_HAS_EXCEPTIONS`, `_LIBCPP_HAS_FILESYSTEM`, localization, wide chars, monotonic clock, random_device. The title toolchain builds `-std=c++23`.
- **32-bit little-endian is the friendly case.** No resource byte-swapping (assets are LE), and the display-list format is `2*sizeof(void*)` per `Gfx` → **8 bytes on Xbox vs 16 on a 64-bit desktop build**, halving the biggest buffer.
- **Assets already stream per-file.** O2R (libzip, default) and OTR (StormLib) both read one entry at a time into a transient buffer — the multi-GB archive never needs to be resident; it rides on the ISO and streams from `D:`.

## 2. The hard risks (ranked)

1. **RAM — `GfxPool`.** `soh/include/z64.h:96-105` inflates the 3 display-list command buffers to **1 MiB `Gfx` entries each**, double-buffered (`soh/src/buffers/gfxbuffers.c:13`) → **~48 MB static BSS**. Original N64 sizes were `0x17E0 / 0x800 / 0x400`. Must shrink and tune so heavy SoH scenes don't overflow. `osGetMemSize()` is stubbed to **1 GB** (`soh/soh/stubs.c:273`) so nothing self-limits — caches must be explicitly bounded.
2. **NV2A renderer.** Fast3D generates one shader per RDP color-combiner at runtime. NV2A has only register combiners / SM1.1 vertex shaders. We lower the combiner descriptor to Xbox pixel shaders. `prism`/TinyCC are **only** used by the text-shader backends and are dropped entirely.
3. **CPU throughput.** Fast3D interprets the entire RSP/RDP display list in software every frame **and does vertex T&L on the CPU**. This is the one risk that can only be answered by measuring on-target. §6 is the mitigation.
4. **Input.** Controller mapping classes are SDL-specific (`ship/controller/.../mapping/sdl/*`). Needs *new* Xbox-gamepad mapping classes over `libxapi` XInput, not just one shim.
5. **Build system.** SoH uses CMake + FetchContent (network). We replace it with RXDK zig/clang driving **vendored** static deps.

## 3. Toolchain & build system

No CMake/FetchContent on the Xbox target. Two viable drivers:

- **(A) Extend RXDK's `zig build`** with a `libultraship` static-lib step + a `soh` XBE step (matches the RXDK-Libs multi-project model, [[rxdk-multi-project-build]]). Preferred — one toolchain, incremental builds, deploy path already solved.
- **(B) A CMake toolchain file** pointing at RXDK Clang + flags, emitting the archive, then hand off to `imagebld`. More faithful to upstream CMake but duplicates the flag/runtime knowledge that already lives in `build/xbox_target.zig`.

**Recommendation: (A).** Add `Shipwright` as consumers of the shipped RXDK `.lib`s.

**Compile flags** (from `build/xbox_target.zig` `cppFlags`, with changes): keep `-ffreestanding -nostdinc++ -femulated-tls -U_DEBUG`; **enable `-fexceptions` and `-frtti`** (SoH needs RTTI; exceptions are sparse but present and libc++ is built with them on). C++20 minimum, C++23 fine.

### Dependency disposition

| Dep | Disposition | Notes |
|---|---|---|
| **SDL2 / SDL2_net** | **DROP** | Replace window+input+audio with libxapi/libd3d8/libdsound. Netplay dropped. |
| **GLEW / OpenGL / glfw3** | **DROP** | GL-only. |
| **prism-processor / TinyCC** | **DROP** | Only used by text-shader backends. |
| **ImGui** | **DEFER** | In-game menu/debug. Bring up headless first; optional D3D8 ImGui backend later. |
| **spdlog** | **DROP/replace** | Route logging to `DbgPrint`/OutputDebugString; it also reserves a thread. |
| **libzip + zlib** | **VENDOR** | O2R archive (default). Portable C. Build static with RXDK clang. |
| **StormLib** | **VENDOR (optional)** | OTR archive. Only if we choose OTR over O2R. |
| **stb_image** | **VENDOR** | PNG decode. Header-only. |
| **nlohmann/json** | **VENDOR** | Config / controller DB. Header-only. |
| **tinyxml2** | **VENDOR** | XML resource factories. |
| **monocypher** | **VENDOR** | ed25519 archive signing. Small C. |
| **bshoshany/thread-pool** | **VENDOR** | Header-only; uses `std::thread` (works). Cap workers (see §5). |
| **libgfxd** | **DROP** | Debug DL disassembler. |
| **dr_libs / ogg / vorbis / opus / opusfile** | **DEFER** | Custom-music codecs. Base game audio doesn't need them. |
| **tinycc scripting** | **DROP** | `ENABLE_SCRIPTING` off. |

**Archive format decision: O2R (libzip).** It's the current default exporter target; libzip+zlib are smaller and simpler to vendor than StormLib, and both stream per-file equally well.

## 4. Platform seams to implement

| Seam | Interface / file | Effort | Backing RXDK lib |
|---|---|---|---|
| Audio | `Ship::AudioPlayer` — `DoInit/DoClose/DoPlay/Buffered` (`libultraship/include/ship/audio/AudioPlayer.h:32`) | ~60 lines | `libdsound` streaming buffer (or `libxact`) |
| Window | `Fast::GfxWindowBackend` — ~40 virtuals (`libultraship/include/fast/backends/gfx_window_manager_api.h:7`) | Medium | `libd3d8` device/present + `libxapi` timing |
| **Renderer** | `Fast::GfxRenderingAPI` — ~45 virtuals (`libultraship/include/fast/backends/gfx_rendering_api.h:30`); 83 call sites in `interpreter.cpp` | **High (the real work)** | `libd3d8` + Xbox pixel shaders |
| Input | New Xbox mapping classes under `ship/controller/` | Medium | `libxapi` XInput |
| Paths/FS | `soh/platform/pathconf.c`, `std::filesystem` (~21 files) | Low–Med | libc filesystem; fix save/config roots to `T:`/`D:` |
| Threads | none — `std::thread`/`mutex`/`cv` map to C11 threads automatically | — | `libxapi` CreateThread |

## 5. Memory budget (64 MB retail; bring up on 128 MB devkit first)

**Detect installed RAM at boot and scale every budget off it — one build, three targets.** 64 MB retail, 128 MB devkits, and 128 MB-upgraded retail consoles all differ, so budgets are *runtime*, not compile-time. Primitive: `MmQueryStatistics` (RXDK kernel export, `shared/include/xboxkrnl/api/mm.h:136`) → set `Length = sizeof(MM_STATISTICS)`, read `TotalPhysicalPages` (`shared/include/xboxkrnl/types/kernel.h:31`); `TotalPhysicalPages * 4096` distinguishes 64 vs 128 MB. Feed the result into:
- `osGetMemSize()` — replace the 1 GB stub (`soh/soh/stubs.c:273`) with the real detected size so any size-aware engine/cache logic behaves.
- The texture byte-window budget (§6) and the resource-cache high-water mark — e.g. give the texture working set and resource cache a larger slice on 128 MB, a tight one on 64 MB. A single `g_RamClass` (RAM_64 / RAM_128) drives the constants.

- **Shrink `GfxPool`** (`z64.h:96-105`) from 1 MiB entries to a tuned value (start ~64 K entries/buffer → 3×64K×8B×2 ≈ 3 MB; tune upward only if a scene overflows). Consider scaling this off `g_RamClass` too (a larger safety margin on 128 MB). Add an overflow assert so we catch it in xemu rather than corrupting memory.
- **Bound the resource cache** (`ResourceManager::mResourceCache`) to a fraction of detected RAM. Add scene-scoped bulk unload (the API exists: `ResourceManager` unload-by-identifier + include/exclude bulk).
- **Audit precache** — `ResourceMgr_LoadDirectory("audio")` (`soh/soh/OTRGlobals.cpp:1142`) eagerly loads; make it lazy or bounded.
- **Texture cache** lives GPU-side once uploaded (see §6) — keep the CPU-side decode buffer single and reused (`mTexUploadBuffer`, `interpreter.cpp:4984`).
- **Cap `BS::thread_pool`** to 1 worker (`hardware_concurrency()==1` on target anyway) to avoid oversubscription on the single core.

Target: title screen on 128 MB, then chase 64 MB.

## 6. CPU-offload / GPU-caching strategy *(primary optimization axis)*

Fast3D is CPU-heavy by design. Concrete tactics, near-term first:

**Near-term (in the D3D8 backend):**
- **Per-pixel shading on the GPU.** The combiner→Xbox-pixel-shader lowering (risk #2) *is* a CPU-offload: N64 color combine runs in NV2A register combiners, not on the P3. Cache compiled pixel shaders by the 128-bit combiner id (as the engine already keys them).
- **GPU-resident texture cache with one-time swizzle.** Decode each N64 texture to RGBA **once**, upload to an NV2A texture, and **swizzle to the NV2A tiled layout on upload** (or use linear textures where cheaper). Key by the engine's texture hash so a texture is never re-decoded or re-uploaded within its cache lifetime. Never read textures back.
- **Byte-budget LRU texture window (UMA-aware).** Fast3D **already** ships an LRU texture cache — `mTextureCache.map` + `mTextureCache.lru` list + `free_texture_ids`, capped at `TEXTURE_CACHE_MAX_SIZE = 1024` (`interpreter.cpp:72`), evicting the least-recently-used entry and recycling its texture id when full (`interpreter.cpp:507-530`). **Base-game textures are vanilla N64 resolution** (CI4/CI8/RGBA16/IA, mostly ≤4 KB) — so the existing count-based 1024 cache is only a few MB and is **fine as-is for the base game**. HD only enters via the opt-in **AltAssets** `alt/*` texture packs (`OTRGlobals.cpp:841`, `game.c:493-494`), off unless the user installs one. But the cap is a **count**, and the NV2A has **no dedicated VRAM** — textures live in the shared 64 MB (UMA) — so a loaded HD pack (single textures in the MBs) would let 1024 entries blow the whole RAM budget. Making eviction byte-aware is therefore *insurance for the HD-pack case*, not a base-game requirement: it lets packs load and thrash gracefully instead of crashing, and stays a no-op for vanilla assets. Two changes:
  1. Lower `TEXTURE_CACHE_MAX_SIZE` to a tuned Xbox working-set window (the "flush older unused textures" behavior).
  2. Make eviction **byte-aware**: the D3D8 backend tracks bytes-per-texture-id (updated in `UploadTexture`, freed in `DeleteTexture`/on id reuse) and exposes the running total; the interpreter's eviction loop then evicts LRU entries until *both* `count < cap` **and** `gpu_texture_bytes < budget`. This is a small, localized extension of the existing loop — the eviction/recycle path is already there, it just needs a second (byte) predicate and backend accounting. Keeps a bounded, self-flushing GPU texture working set that never exceeds the RAM budget regardless of pack size.
- **HD-texture max-size clamp (planned: HD packs on).** The goal is to run HD packs but **clamp each texture to a max dimension** so the working set is bounded and predictable (max bytes ≈ cache_count × maxdim² × 4). Enforced in two layers, plus a critical staging-buffer fix:
  1. **`GetMaxTextureSize()` = a RAM-class policy max, not the NV2A's true limit.** This one return value is squared ×4 and `malloc`'d as the upload staging buffer (`interpreter.cpp:4983-4984`): returning the real 4096 = **64 MB** buffer, 8192 = 256 MB; returning **512 → 1 MB**, 1024 → 4 MB. So the Xbox backend returns e.g. **512 on 64 MB / 1024 on 128 MB** (driven by `g_RamClass`, §5). This both sizes the staging buffer sanely *and* advertises the clamp dimension.
  2. **Offline pre-clamp (primary, zero runtime CPU).** A PC-side pack-repack step (extend `torch`/the exporter, or a standalone tool) box-downsamples every HD texture to ≤ policy max before it's packed into the O2R. Aligns with the avoid-CPU steer: smaller archive, smaller decode, textures arrive already-right-sized so the full-image replacement path (`interpreter.cpp:1199`, which uploads `width×height` directly, *not* through the staging buffer) never exceeds the clamp.
     - **Concrete target: OoT-Reloaded** (GhostlyDark) — a ~20,000-texture, **2160p (4K)** pack that officially supports Ship of Harkinian, so it loads via AltAssets. At native res it is multi-GB / multi-MB-per-texture — infeasible on Xbox UMA. Pipeline: *OoT-Reloaded (4K, SoH format) → offline clamp/repack to ≤512 (64 MB) / ≤1024 (128 MB) → Xbox O2R → AltAssets load.* A 4K→512 downsample is ~64× area reduction; keeps the pack's art at an NV2A-holdable resolution and shrinks the shipped archive dramatically. Requirements: (a) build the pack against a **matching SoH/2Ship version** so `alt/*` paths/hashes line up with this tree; (b) confirm the per-scene *count* of simultaneously-resident clamped textures fits the byte-window. This is a strong demo target — visibly enhanced OoT on stock Xbox hardware.
  3. **Runtime safety clamp in `UploadTexture` (fallback).** If a texture still arrives with `w` or `h` > policy max (an unprocessed pack), box-downsample once in the backend before creating the NV2A texture. One-time per texture, cached by the LRU — amortized, never per-frame — so an un-preprocessed pack degrades in quality instead of OOMing.
- **Eliminate GPU→CPU readbacks where possible.** `GetPixelDepth` / `ReadFramebufferToCPU` (used for sun/occlusion visibility and framebuffer effects) force pipeline stalls. Batch/lazy-resolve them, or emulate on GPU, so the CPU never blocks on the NV2A.
- **Static/streaming vertex buffers.** `DrawTriangles(float buf_vbo[]...)` currently re-uploads a CPU-built vertex array every draw. Use `libd3d8` dynamic vertex buffers with proper discard/no-overwrite semantics so uploads pipeline instead of stalling.

**Research (bigger wins, evaluate after first light):**
- **Hardware T&L via an NV2A vertex shader.** Today Fast3D transforms + lights vertices on the CPU and hands screen-space attributes to the backend. If we intercept one level up — feed *model-space* vertices + the MVP/lighting as vertex-shader constants — the NV2A does transform+lighting, removing the single largest CPU cost. This deviates from the interpreter's current contract and needs a spike; it's the highest-value "cache to GPU" item.
- **Display-list memoization.** OoT scene geometry DLs are largely static per room. Cache the translated draw stream (vertex/index buffers + state) keyed by DL pointer+revision, and re-issue on unchanged frames instead of re-walking the DL. Large CPU saving; moderate complexity.

## 7. D3D8 renderer backend design (`gfx_xbox_d3d8`)

- Model structure on `gfx_opengl.cpp` (1,111 lines, GL2-era — closest to fixed-function).
- **Shaders:** `CreateAndLoadNewShader(id0,id1)` → build an Xbox pixel shader (register-combiner program) from the combiner descriptor; `LookupShader` hits the id-keyed cache. Vertex path via fixed-function or a small VS.
- **Textures:** `NewTexture/UploadTexture/SelectTexture/SetSamplerParameters` → `libd3d8` textures, swizzled once, clamp/wrap/mirror from `cms/cmt`, 3-point vs linear filter.
- **State:** depth test/mask, decal z-mode (polygon offset), scissor, viewport, alpha, blend — direct D3D8 render-state mapping.
- **Framebuffers:** `CreateFramebuffer/UpdateFramebufferParameters/StartDrawToFramebuffer/CopyFramebuffer/ClearFramebuffer` → D3D8 render targets. Watch MSAA (probably force off), and `ClearDepthRegion` (scissored depth clear).
- **Clip params:** `z_is_from_0_to_1 = true` for D3D-style depth; `invertY` per Xbox conventions.

## 8. Phased plan

**Phase 0 — Build spike (toolchain proof). ✅ DONE (2026-09-11).**
Built via **`rxdk.cli`** (not hand-rolled zig): a probe title exercising the exact STL surface SoH needs — `std::thread`/`mutex`/`cv` (→ C11 `thrd_create` → `CreateThread`), exception `throw`/`catch` (libunwind `.eh_frame` recovery), `std::filesystem`, `unordered_map`, `shared_ptr`, `variant`, `std::function`. Compiled, linked against libc/libcpp/libxapi/libkernel, packed to ISO, **booted in xemu and ran correctly at runtime** (`result=520`, "PASS: threads+exceptions+filesystem+containers OK"). Toolchain compile+link+runtime risk retired. The RXDK engine handles all libc++ consumer plumbing (newlib locale backend, picolibc prereqs, `exceptions` default on, `cppStandard` default c++23) — no manual flag replication needed. Next: still need the *real* `libultraship` + vendored deps to build (that's the bulk of the archive work), but the STL/runtime foundation is proven.

**Phase 1 — Renderer spike (hardest item in isolation).**
`gfx_xbox_d3d8` + `GfxWindowBackend` over libd3d8. Feed a hardcoded Fast3D triangle + textured quad through the real interpreter path.
*Exit:* textured, combiner-shaded triangle visible in xemu-devkit ISO.

**Phase 2 — Boot the game.**
Shrink `GfxPool`; `XboxAudioPlayer` (libdsound); XInput mapping classes; O2R streaming from `D:`; path/save roots. Wire `soh` main loop.
*Exit:* SoH title screen → in-game on the 128 MB devkit.

**Phase 3 — Squeeze & tune.**
Bound caches to 64 MB; profile Fast3D CPU cost; apply §6 research items (HW T&L, DL memoization) as needed; ImGui/custom-music optional.
*Exit:* playable on 64 MB retail config; measured frame budget.

## 9. Open questions / spikes

- Does `libd3d8` expose enough register-combiner control for the full RDP combiner set, or do we need native-NV2A escape hatches for some combiners? (Spike in Phase 1.)
- CPU frame cost of software Fast3D on 733 MHz for a representative OoT scene — measure early in Phase 2 to decide how much of §6-research is mandatory.
- `std::filesystem` behavior on the RXDK libc backend for the ~21 call sites (archive discovery, config, mods) — verify in Phase 0.
- Save data location (`T:` per-title FATX) and config file strategy.

## 10. Packaging & test loop

`zig build` (libultraship.lib + soh.xbe) → `imagebld` (XBE, subsystem 14, .bss zero-fill) → `xdvdfs` (ISO with the O2R asset archive) → launch `D:\Git\xemu-devkit\xemu.exe`. Incremental builds + deploy path per [[rxdk-incremental-builds]] / [[rxdk-sample-lib-deploy-path]].
