#include "nitro/os.h"
#include "nitro/reg.h"
#include "nitro/types.h"

static void *OS_GetInitArenaHi(u32);
static void *OS_GetInitArenaLo(u32);
static void OS_SetProtectionRegion1(u32);
static void OS_SetProtectionRegion2(u32);

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
    OS_SetArenaHi(2, OS_GetInitArenaHi(2U));
    OS_SetArenaLo(2, OS_GetInitArenaLo(2U));
    if (!sMainExArenaEnabled || (OS_GetConsoleType() & 3) == 1) {
        OS_SetProtectionRegion1(0x0200002B);
        OS_SetProtectionRegion2(0x023E0021);
    }
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
            if ((sMainExArenaEnabled == 0) || ((OS_GetConsoleType() & 3) == 1)) {
                return NULL;
            }
            return (void *) 0x02700000;
        case OS_ARENA_ITCM:
            return (void *) 0x02000000;
        case OS_ARENA_DTCM:
            var_r2 = &DTCM_LO[0x3f80] - OS_unk_linker_2;
            if (OS_unk_linker_4 == 0) {
                if ((u32) DTCM_LO >= 0x027e0a20) {
                    break;
                }
                return (void *) 0x027e0a20;
            }
            if (OS_unk_linker_4 < 0) {
                return (void *) 0x027e0a20;
            }
            return var_r2 - OS_unk_linker_4;
        case OS_ARENA_5:
            return (void *) 0x027ff680;
        case OS_ARENA_6:
            return (void *) 0x037f8000;
    }
    return NULL;
}

void *OS_GetInitArenaLo(u32 arg0) {
    switch (arg0) {
        case OS_ARENA_MAIN:
            return (void *) 0x0223D160;
        case OS_ARENA_2:
            if (!sMainExArenaEnabled || (OS_GetConsoleType() & 3) == 1) {
                return NULL;
            }
            return (void *) 0x023E0000;
        case OS_ARENA_ITCM:
            return (void *) 0x01FFFD60;
        case OS_ARENA_DTCM:
            return (void *) 0x027E0A20;
        case OS_ARENA_5:
            return (void *) 0x027FF000;
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

void OS_EnableMainExArena(void) {
    sMainExArenaEnabled = true;
}

#define M2C_ERROR()

THUMB_DISABLE
s32 OS_GetDTCMAddress(void) {
    u32 address;
    asm("mrc p15, 0, address, c9, c1, 0" : "=r"(address));
    return address & 0xFFFFF000;
}
THUMB_ENABLE

THUMB_DISABLE
void OS_EnableProtectionUnit(void) {
    u32 value;
    asm("mrc p15, 0, value, c1, c0, 0" : "=r"(value));
    value |= 1;
    asm("mcr p15, 0, value, c1, c0, 0");
}
THUMB_ENABLE

THUMB_DISABLE
void OS_DisableProtectionUnit() {
    u32 value;
    asm("mrc p15, 0, value, c1, c0, 0" : "=r"(value));
    value &= ~1;
    asm("mcr p15, 0, value, c1, c0, 0");
}
THUMB_ENABLE

THUMB_DISABLE
void OS_SetDPermissionsForProtectionRegion(s32 arg0, s32 arg1) {
    u32 value;
    asm("mrc p15, 0, value, c5, c0, 2" : "=r"(value));
    value = (value & ~arg0) | arg1;
    asm("mcr p15, 0, value, c5, c0, 2");
}
THUMB_ENABLE

THUMB_DISABLE
static void OS_SetProtectionRegion1(u32 arg0) {
    asm("mcr p15, 0, arg0, c6, c1, 0");
}
THUMB_ENABLE

THUMB_DISABLE
void OS_SetProtectionRegion2(u32 arg0) {
    asm("mcr p15, 0, arg0, c6, c2, 0");
}
THUMB_ENABLE
