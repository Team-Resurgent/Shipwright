// Minimal opusfile stub for the RXDK/Xbox build. Streamed custom music (Opus) is deferred;
// mixer.c includes <opusfile.h> for the streaming path. Declarations only — the opus funcs
// are unresolved at link (custom music non-functional), base-game sequence audio is separate.
#pragma once
#include <stddef.h>
typedef struct OggOpusFile OggOpusFile;
#ifdef __cplusplus
extern "C" {
#endif
OggOpusFile* op_open_memory(const unsigned char* data, size_t len, int* error);
void op_free(OggOpusFile* of);
int op_read(OggOpusFile* of, short* pcm, int buf_size, int* li);
int op_read_stereo(OggOpusFile* of, short* pcm, int buf_size);
long op_pcm_seek(OggOpusFile* of, long long pcm_offset);
long op_pcm_total(OggOpusFile* of, int li);
int op_channel_count(const OggOpusFile* of, int li);
#ifdef __cplusplus
}
#endif
