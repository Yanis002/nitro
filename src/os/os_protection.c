#include "nitro/os/os_protection.h"

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
void OS_SetProtectionRegion1(u32 arg0) {
    asm("mcr p15, 0, arg0, c6, c1, 0");
}
THUMB_ENABLE

THUMB_DISABLE
void OS_SetProtectionRegion2(u32 arg0) {
    asm("mcr p15, 0, arg0, c6, c2, 0");
}
THUMB_ENABLE
