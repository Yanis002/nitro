#include "nitro/os.h"
#include "nitro/reg.h"
#include "nitro/types.h"

static void *OS_GetInitArenaHi(u32);
static void *OS_GetInitArenaLo(u32);

static BOOL sMainExArenaEnabled;
BOOL OSi_ArenaInitialized;

void OS_InitArena(void) {
    if (OSi_ArenaInitialized) {
        return;
    }
    OSi_ArenaInitialized = true;
    OS_SetArenaHi(OS_ARENA_MAIN, OS_GetInitArenaHi(OS_ARENA_MAIN));
    OS_SetArenaLo(OS_ARENA_MAIN, OS_GetInitArenaLo(OS_ARENA_MAIN));
    OS_SetArenaLo(OS_ARENA_2, NULL);
    OS_SetArenaHi(OS_ARENA_2, NULL);
    OS_SetArenaHi(OS_ARENA_ITCM, OS_GetInitArenaHi(OS_ARENA_ITCM));
    OS_SetArenaLo(OS_ARENA_ITCM, OS_GetInitArenaLo(OS_ARENA_ITCM));
    OS_SetArenaHi(OS_ARENA_DTCM, OS_GetInitArenaHi(OS_ARENA_DTCM));
    OS_SetArenaLo(OS_ARENA_DTCM, OS_GetInitArenaLo(OS_ARENA_DTCM));
    OS_SetArenaHi(OS_ARENA_5, OS_GetInitArenaHi(OS_ARENA_5));
    OS_SetArenaLo(OS_ARENA_5, OS_GetInitArenaLo(OS_ARENA_5));
    OS_SetArenaHi(OS_ARENA_6, OS_GetInitArenaHi(OS_ARENA_6));
    OS_SetArenaLo(OS_ARENA_6, OS_GetInitArenaLo(OS_ARENA_6));
}

void OS_InitArenaEx(void) {
#if NITRO_VERSION >= 0x5057533
    OS_GetConsoleType();
#endif
    OS_SetArenaHi(OS_ARENA_2, OS_GetInitArenaHi(2U));
    OS_SetArenaLo(OS_ARENA_2, OS_GetInitArenaLo(2U));
#if NITRO_VERSION >= 0x5057533
    OS_func_0149(1, 0x2000000, 0x2a);
#else
    if (!sMainExArenaEnabled || (OS_GetConsoleType() & 3) == 1) {
        OS_SetProtectionRegion1(0x0200002B);
        OS_SetProtectionRegion2(0x023E0021);
    }
#endif
}

void *OS_GetArenaHi(u32 arena) {
    return REG_027FFDC4[arena];
}

void *OS_GetArenaLo(u32 arena) {
    return REG_027FFDA0[arena];
}

void *OS_GetInitArenaHi(u32 arena) {
    void *var_r2;

    switch (arena) {
        case OS_ARENA_MAIN:
            return (void *) 0x023e0000;
        case OS_ARENA_2:
#if NITRO_VERSION >= 0x5057533
            if (!sMainExArenaEnabled) {
                return NULL;
            }
            if ((OS_GetConsoleType() & 0xf) == 1) {
                return NULL;
            }
#else
            if ((sMainExArenaEnabled == 0) || ((OS_GetConsoleType() & 3) == 1)) {
                return NULL;
            }
#endif
            return (void *) 0x02700000;
        case OS_ARENA_ITCM:
            return (void *) 0x02000000;
        case OS_ARENA_DTCM:
            var_r2 = &DTCM_LO[0x3f80] - OS_unk_linker_2;
            if (OS_unk_linker_4 == 0) {
                if (DTCM_LO >= DTCM_HI) {
                    return DTCM_LO;
                }
                return DTCM_HI;
            }
            if (OS_unk_linker_4 < 0) {
                return DTCM_HI - OS_unk_linker_4;
            }
            return var_r2 - OS_unk_linker_4;
        case OS_ARENA_5:
            return (void *) (_BIOS_REG_BASE | 0x680);
        case OS_ARENA_6:
            return (void *) 0x037f8000;
    }
    return NULL;
}

void *OS_GetInitArenaLo(u32 arg0) {
    switch (arg0) {
        case OS_ARENA_MAIN:
            return CODE_HI;
        case OS_ARENA_2:
#if NITRO_VERSION >= 0x5057533
            if (!sMainExArenaEnabled) {
                return NULL;
            }
            if ((OS_GetConsoleType() & 0xf) == 1) {
                return NULL;
            }
#else
            if (!sMainExArenaEnabled || (OS_GetConsoleType() & 3) == 1) {
                return NULL;
            }
#endif
            return (void *) 0x023E0000;
        case OS_ARENA_ITCM:
            return ITCM_HI;
        case OS_ARENA_DTCM:
            return DTCM_HI;
        case OS_ARENA_5:
            return (void *) _BIOS_REG_BASE;
        case OS_ARENA_6:
            return (void *) 0x037F8000;
        default:
            return NULL;
    }
}

void OS_SetArenaHi(u32 arena, void *addr) {
    REG_027FFDC4[arena] = addr;
}

void OS_SetArenaLo(u32 arena, void *addr) {
    REG_027FFDA0[arena] = addr;
}

void *OS_AllocFromArenaLo(u32 arena, u32 size, u32 align) {
    void *lo;
    void *addr;
    void *end;

    lo = OS_GetArenaLo(arena);
    if (lo == NULL) {
        return NULL;
    }
    addr = (void *) (((u32) lo + align - 1) & ~(align - 1));
    end  = addr + size;
    lo   = (void *) (((u32) end + align - 1) & ~(align - 1));
    if (lo > OS_GetArenaHi(arena)) {
        return NULL;
    }
    OS_SetArenaLo(arena, lo);
    return addr;
}

void OS_EnableMainExArena(void) {
    sMainExArenaEnabled = true;
}
