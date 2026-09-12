// dlfcn stub for the RXDK/Xbox libultraship build. The scripting/mod dynamic-loader
// isn't supported on the console, so dlopen() fails cleanly (LibraryLoader treats a null
// handle as "no library"). Lets interpreter.cpp / LibraryLoader.cpp / FileDropMgr.cpp
// compile without a POSIX dynamic linker.
#pragma once
#define RTLD_LAZY   0x0001
#define RTLD_NOW    0x0002
#define RTLD_LOCAL  0x0000
#define RTLD_GLOBAL 0x0100
#ifdef __cplusplus
extern "C" {
#endif
static inline void* dlopen(const char* /*file*/, int /*mode*/) { return 0; }
static inline int dlclose(void* /*handle*/) { return 0; }
static inline void* dlsym(void* /*handle*/, const char* /*name*/) { return 0; }
static inline const char* dlerror(void) { return "dlfcn not supported on Xbox"; }

typedef struct {
    const char* dli_fname;
    void* dli_fbase;
    const char* dli_sname;
    void* dli_saddr;
} Dl_info;
static inline int dladdr(const void* /*addr*/, Dl_info* info) {
    if (info) { info->dli_fname = 0; info->dli_fbase = 0; info->dli_sname = 0; info->dli_saddr = 0; }
    return 0;
}

#ifdef __cplusplus
}
#endif
