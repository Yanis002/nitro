#ifndef _NITRO_OS_ARENA_H
#define _NITRO_OS_ARENA_H

#ifdef __cplusplus
extern "C" {
#endif

#include "nitro/types.h"

#define OS_ARENA_MAIN 0
#define OS_ARENA_2 2
#define OS_ARENA_ITCM 3
#define OS_ARENA_DTCM 4
#define OS_ARENA_5 5
#define OS_ARENA_6 6

extern u32 _OS_unk_linker_1; // 0xffffd9b8
extern u32 _OS_unk_linker_2; // gtactw_eu, diamondtrust_us: 0x800
extern u32 _OS_unk_linker_3; // 0x027e0080
extern u32 _OS_unk_linker_4; // gtactw_eu, diamondtrust_us: 0
#define OS_unk_linker_1 ((s32) (&_OS_unk_linker_1))
#define OS_unk_linker_2 ((u32) (&_OS_unk_linker_2))
#define OS_unk_linker_3 ((u8 *) (&_OS_unk_linker_3))
#define OS_unk_linker_4 ((s32) (&_OS_unk_linker_4))

extern BOOL OSi_ArenaInitialized;

void *OS_InitAlloc(u32 arena, u32 addrLo, u32 addrHi, u32);
void *OS_GetArenaLo(u32 arena);
void *OS_GetArenaHi(u32 arena);

void OS_SetArenaLo(u32 arena, void *addr);
void OS_SetArenaHi(u32 arena, void *addr);
void *OS_AllocFromArenaLo(u32 arena, u32 size, u32 num);

inline void *OS_GetMainArenaLo(void) {
    return OS_GetArenaLo(OS_ARENA_MAIN);
}
inline void *OS_GetMainArenaHi(void) {
    return OS_GetArenaHi(OS_ARENA_MAIN);
}
inline void *OS_GetITCMArenaLo(void) {
    return OS_GetArenaLo(OS_ARENA_ITCM);
}
inline void *OS_GetITCMArenaHi(void) {
    return OS_GetArenaHi(OS_ARENA_ITCM);
}
inline void *OS_GetDTCMArenaLo(void) {
    return OS_GetArenaLo(OS_ARENA_DTCM);
}
inline void *OS_GetDTCMArenaHi(void) {
    return OS_GetArenaHi(OS_ARENA_DTCM);
}

inline void OS_SetMainArenaLo(void *addr) {
    OS_SetArenaLo(OS_ARENA_MAIN, addr);
}

inline void *OS_AllocFromMainArenaLo(u32 size, u32 num) {
    return OS_AllocFromArenaLo(OS_ARENA_MAIN, size, num);
}

#ifdef __cplusplus
} // extern "C"
#endif

#endif
