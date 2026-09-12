#include "nitro/os/os_protection.h"
#include "nitro/reg.h"
#include "nitro/types.h"

#define OS_PROTECTION_REGION_ENABLE 1

static void OS_func_0086(s32 arg0);
static s32 OS_func_0087(s32 arg0);
static void OS_func_0088(void);

static void (*sSetProtectionRegion[8])(u32) = {
    OS_SetProtectionRegion0, OS_SetProtectionRegion1, OS_SetProtectionRegion2, OS_SetProtectionRegion3,
    OS_SetProtectionRegion4, OS_SetProtectionRegion5, OS_SetProtectionRegion6, OS_SetProtectionRegion7,
};
static u32 (*sGetProtectionRegion[8])(void) = {
    OS_GetProtectionRegion0, OS_GetProtectionRegion1, OS_GetProtectionRegion2, OS_GetProtectionRegion3,
    OS_GetProtectionRegion4, OS_GetProtectionRegion5, OS_GetProtectionRegion6, OS_GetProtectionRegion7,
};

static void *data_02138744;
static void (*data_02138750)(void *, u32);
static u32 data_02138748;
static void *data_0213874c;
static u8 data_02138774[0x68];

BOOL OS_func_0127(u32 arg0) {
    return arg0 >= 0x05000000U && arg0 < 0x07000800U;
}

THUMB_DISABLE();

s32 OS_GetDTCMAddress(void) {
    u32 address;
    asm("mrc p15, 0, address, c9, c1, 0" : "=r"(address));
    return address & 0xFFFFF000;
}

void OS_EnableProtectionUnit(void) {
    u32 value;
    asm("mrc p15, 0, value, c1, c0, 0" : "=r"(value));
    value |= 1;
    asm("mcr p15, 0, value, c1, c0, 0");
}

void OS_DisableProtectionUnit() {
    u32 value;
    asm("mrc p15, 0, value, c1, c0, 0" : "=r"(value));
    value &= ~1;
    asm("mcr p15, 0, value, c1, c0, 0");
}

void OS_SetICachabilityForProtectionRegion(u32 arg0) {
    u32 value;
    asm("mrc p15, 0, value, c2, c0, 1" : "=r"(value));
    value |= arg0;
    asm("mcr p15, 0, value, c2, c0, 1");
}

// asm needed as mwccarm compiles `x & ~arg0` to `mvn -> and` instead of `bic`
ASM void OS_ClearICachabilityForProtectionRegion(u32 arg0) {
#if __MWERKS__ // clang-format off
    mrc p15, 0, r1, c2, c0, 1
    bic r1, r1, r0
    mcr p15, 0, r1, c2, c0, 1
    bx lr
#endif // clang-format on
}

void OS_SetDCachabilityForProtectionRegion(u32 arg0) {
    u32 value;
    asm("mrc p15, 0, value, c2, c0, 0" : "=r"(value));
    value |= arg0;
    asm("mcr p15, 0, value, c2, c0, 0");
}

ASM void OS_ClearDCachabilityForProtectionRegion(u32 arg0){
#if __MWERKS__ // clang-format off
    mrc p15, 0, r1, c2, c0, 0
    bic r1, r1, r0
    mcr p15, 0, r1, c2, c0, 0
    bx lr
#endif // clang-format on
}

ASM void OS_SetDPermissionsForProtectionRegion(s32 arg0, s32 arg1) {
#if __MWERKS__ // clang-format off
    mrc p15, 0, r2, c5, c0, 2
    bic r2, r2, r0
    orr r2, r2, r1
    mcr p15, 0, r2, c5, c0, 2
    bx lr
#endif // clang-format on
}

void OS_SetDCacheBufferabilityForProtectionRegions(u32 arg0) {
    u32 value;
    asm("mrc p15, 0, value, c3, c0, 0" : "=r"(value));
    value |= arg0;
    asm("mcr p15, 0, value, c3, c0, 0");
}

ASM void OS_ClearDCacheBufferabilityForProtectionRegions(u32 arg0) {
#if __MWERKS__ // clang-format off
    mrc p15, 0, r1, c3, c0, 0
    bic r1, r1, r0
    mcr p15, 0, r1, c3, c0, 0
    bx lr
#endif // clang-format on
}

void OS_SetProtectionRegion(s32 region, s32 value) {
    sSetProtectionRegion[region](value);
}

u32 OS_GetProtectionRegion(u32 region) {
    return sGetProtectionRegion[region]();
}

void OS_SetProtectionRegion0(u32 arg0) {
    asm("mcr p15, 0, arg0, c6, c0, 0");
}

void OS_SetProtectionRegion1(u32 arg0) {
    asm("mcr p15, 0, arg0, c6, c1, 0");
}

void OS_SetProtectionRegion2(u32 arg0) {
    asm("mcr p15, 0, arg0, c6, c2, 0");
}

void OS_SetProtectionRegion3(u32 arg0) {
    asm("mcr p15, 0, arg0, c6, c3, 0");
}

void OS_SetProtectionRegion4(u32 arg0) {
    asm("mcr p15, 0, arg0, c6, c4, 0");
}

void OS_SetProtectionRegion5(u32 arg0) {
    asm("mcr p15, 0, arg0, c6, c5, 0");
}

void OS_SetProtectionRegion6(u32 arg0) {
    asm("mcr p15, 0, arg0, c6, c6, 0");
}

void OS_SetProtectionRegion7(u32 arg0) {
    asm("mcr p15, 0, arg0, c6, c7, 0");
}

u32 OS_GetProtectionRegion0(void) {
    u32 value;
    asm("mrc p15, 0, value, c6, c0, 0" : "=r"(value));
    return value;
}

u32 OS_GetProtectionRegion1(void) {
    u32 value;
    asm("mrc p15, 0, value, c6, c1, 0" : "=r"(value));
    return value;
}

u32 OS_GetProtectionRegion2(void) {
    u32 value;
    asm("mrc p15, 0, value, c6, c2, 0" : "=r"(value));
    return value;
}

u32 OS_GetProtectionRegion3(void) {
    u32 value;
    asm("mrc p15, 0, value, c6, c3, 0" : "=r"(value));
    return value;
}

u32 OS_GetProtectionRegion4(void) {
    u32 value;
    asm("mrc p15, 0, value, c6, c4, 0" : "=r"(value));
    return value;
}

u32 OS_GetProtectionRegion5(void) {
    u32 value;
    asm("mrc p15, 0, value, c6, c5, 0" : "=r"(value));
    return value;
}

u32 OS_GetProtectionRegion6(void) {
    u32 value;
    asm("mrc p15, 0, value, c6, c6, 0" : "=r"(value));
    return value;
}

u32 OS_GetProtectionRegion7(void) {
    u32 value;
    asm("mrc p15, 0, value, c6, c7, 0" : "=r"(value));
    return value;
}

THUMB_ENABLE();

void OS_func_0149(u32 region, u32 addr, u32 size) {
    OS_SetProtectionRegion(region, size | (addr & (0xfffff000 << ((size - 22) >> 1))) | OS_PROTECTION_REGION_ENABLE);
}

static void OS_func_0085(void);

void OS_func_0084(void) {
    BOOL var_r1;
    void *var_r2;

    var_r2        = REG_027FFD9C;
    var_r1        = false;
    data_0213874c = var_r2;
    if (var_r2 >= (void *) 0x02600000 && var_r2 < (void *) 0x02800000) {
        var_r1 = true;
    }
    if (!var_r1) {
        var_r2 = NULL;
    }
    data_02138744 = var_r2;
    if (var_r2 == NULL) {
        REG_027FFD9C            = &OS_func_0085;
        *(void **) (0x027FFD9C) = &OS_func_0085;
    }
    data_02138750 = NULL;
}

THUMB_DISABLE();

// asm needed for reading PC and writing SP
ASM void OS_func_0085(void){
#if __MWERKS__ // clang-format off
    lda ip, data_02138744
    ldr ip, [ip]
    cmp ip, 0
    movne lr, pc
    bxne ip
    ldconst ip, #0x2000000
    stmdb ip!, {r0, r1, r2, r3, sp, lr}
    and r0, sp, 1
    mov sp, ip
    mrs r1, cpsr
    and r1, r1, 0x1f
    teq r1, 0x17
    bne lbl_1
    bl OS_func_0086
    b lbl_2
lbl_1:
    teq r1, 0x1b
    bne lbl_2
    bl OS_func_0086
lbl_2:
    lda ip, data_02138744
    ldr ip, [ip]
    cmp ip, 0
lbl_3:
    beq lbl_3
lbl_4:
    mov r0, r0
    b lbl_4
    ldmia sp!, {r0, r1, r2, r3, ip, lr}
    mov sp, ip
    bx lr
#endif // clang-format on
}

// asm needed because mwccarm always reads directly into `pc` when returning with `ldmia sp!`
ASM void OS_func_0086(s32 arg0){
#if __MWERKS__ // clang-format off
    stmdb sp!, {r0, lr}
    bl OS_func_0087
    bl OS_func_0088
    ldmia sp!, {r0, lr}
    bx lr
#endif // clang-format on
}

// asm needed because `ip` is defined by the caller (breaks procedure call standard)
ASM s32 OS_func_0087(s32 arg0){
#if __MWERKS__ // clang-format off
    lda r1, data_02138774
    str r0, [r1, 0x6c]
    ldr r0, [ip, 0x0]
    str r0, [r1, 0x4]
    ldr r0, [ip, 0x4]
    str r0, [r1, 0x8]
    ldr r0, [ip, 0x8]
    str r0, [r1, 0xc]
    ldr r0, [ip, 0xc]
    str r0, [r1, 0x10]
    ldr r2, [ip, 0x10]
    bic r2, r2, 1
    add r0, r1, 0x14
    stmia r0, {r4, r5, r6, r7, r8, r9, r10, r11}
    ldr r0, [r2, 0x0]
    str r0, [r1, 0x64]
    ldr r3, [r2, 0x4]
    str r3, [r1, 0x0]
    ldr r0, [r2, 0x8]
    str r0, [r1, 0x34]
    ldr r0, [r2, 0xc]
    str r0, [r1, 0x40]
    mrs r0, cpsr
    orr r3, r3, 0x80
    bic r3, r3, 0x20
    msr cpsr_fsxc, r3
    str sp, [r1, 0x38]
    str lr, [r1, 0x3c]
    mrs r2, spsr
    msr cpsr_fsxc, r0
    bx lr
#endif // clang-format on
}

// asm needed due to stack push/pop in the middle of the function
ASM void OS_func_0088(void) {
#if __MWERKS__ // clang-format off
    stmdb sp!, {r3, lr}
    lda r0, data_02138744
    ldr r0, [r0, #0xc]
    cmp r0, #0x0
    ldmeqia sp!, {r3, pc}
    mrs r2, cpsr
    mov r0, sp
    ldconst r1, #0x9f
    msr cpsr_fsxc, r1
    mov r1, sp
    mov sp, r0
    stmdb sp!, {r1, r2}
    bl OS_EnableProtectionUnit
    lda r0, data_02138774
    lda r1, data_02138748
    ldr r1, [r1, #0x0]
    lda ip, data_02138750
    ldr ip, [ip, #0x0]
    lda lr, lbl_1
    bx ip
lbl_1:
    bl OS_DisableProtectionUnit
    ldmia sp!, {r1, r2}
    mov sp, r1
    msr cpsr_fsxc, r2
    ldmia sp!, {r3, pc}
#endif // clang-format on
}

THUMB_ENABLE();
