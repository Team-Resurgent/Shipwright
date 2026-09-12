// Minimal SDL_net stub for the RXDK/Xbox build. Netplay/Crowd Control (soh/soh/Network) is
// deferred (its .cpp are excluded); OTRGlobals still includes the Network headers, so provide
// the SDL_net types they declare. Networking would use libxnet if revived.
#pragma once
#include "SDL2/SDL_stdinc.h"
typedef struct { Uint32 host; Uint16 port; } IPaddress;
typedef struct _TCPsocket* TCPsocket;
typedef struct _UDPsocket* UDPsocket;
typedef struct _SDLNet_SocketSet* SDLNet_SocketSet;
typedef struct { int channel; IPaddress address; Uint8* data; int len; int maxlen; int status; } UDPpacket;
#ifdef __cplusplus
extern "C" {
#endif
int SDLNet_Init(void);
void SDLNet_Quit(void);
int SDLNet_ResolveHost(IPaddress* address, const char* host, Uint16 port);
TCPsocket SDLNet_TCP_Open(IPaddress* ip);
void SDLNet_TCP_Close(TCPsocket sock);
int SDLNet_TCP_Send(TCPsocket sock, const void* data, int len);
int SDLNet_TCP_Recv(TCPsocket sock, void* data, int maxlen);
const char* SDLNet_GetError(void);
#ifdef __cplusplus
}
#endif
