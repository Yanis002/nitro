#include "nitro/fs/fs_overlay.h"
#include "nitro/dgt.h"
#include "nitro/fs/fs_archive.h"
#include "nitro/fs/fs_file.h"
#include "nitro/mi.h"
#include "nitro/os.h"
#include "nitro/reg.h"

#include <global_destructor_chain.h>

typedef struct HmacKey {
    /* 00 */ const u8 *key;
    /* 04 */ u32 keyLength;
    /* 08 */
} HmacKey;

typedef struct FS_UnkStruct23 {
    /* 00 */ FSOverlay *overlays;
    /* 04 */ u32 size;
    /* 08 */
} FS_UnkStruct23;

static u32 FSi_GetOverlayBinarySize(FSOverlay *overlay);
static void FS_ClearOverlayImage(FSOverlay *overlay);
static FS_UnkStruct21 FS_GetOverlayFileID(FSOverlay *overlay);
static BOOL FSi_LoadOverlayInfoCore(FSOverlayInfo *info, OSCpu cpu, u32 id, FSArchive *archive, u32 arm9ovt,
                                    u32 arm9ovtSize, u32 arm7ovt, u32 arm7ovtSize);
static BOOL FS_LoadOverlayImage(FSOverlay *arg0);
static s32 FSi_CompareDigest(u8 (*signature)[0x14], void *data, u32 length);
static void FS_EndOverlay(FSOverlay *overlay);
static s32 FS_UnloadOverlayImage(FSOverlay *overlay);
static BOOL FS_LoadOverlayInfo(FSOverlayInfo *arg0, u32 arg1, u32 id);
static void FS_StartOverlay(FSOverlay *overlay);

static HmacKey sHmacKey = {
    .key       = FS_data_0001,
    .keyLength = sizeof(FS_data_0001),
};

extern u8 OverlaySignatures[0x14];
extern u8 OverlaySignaturesEnd[0x14];
static FS_UnkStruct23 sArm9OverlayTable;
static FS_UnkStruct23 sArm7OverlayTable;

static u32 FSi_GetOverlayBinarySize(FSOverlay *overlay) {
    if (FS_OVERLAY_FLAGS(overlay) & FS_OVERLAY_FLAG_COMPRESSED) {
        return overlay->fileSize << 8 >> 8;
    }
    return overlay->textSize;
}

static void FS_ClearOverlayImage(FSOverlay *overlay) {
    void *addr;
    s32 textSize;
    s32 end;

    addr     = overlay->addr;
    textSize = overlay->textSize;
    end      = textSize + overlay->bssSize;
    IC_InvalidateRange(addr, end);
    DC_InvalidateRange(addr, end);
    MI_CpuFill8(addr + textSize, 0, end - textSize);
}

static FS_UnkStruct21 FS_GetOverlayFileID(FSOverlay *overlay) {
    FS_UnkStruct21 sp0;
    sp0.archive = &FS_romArchive;
    sp0.fileId  = overlay->fileId;
    return sp0;
}

static BOOL FSi_LoadOverlayInfoCore(FSOverlayInfo *info, OSCpu cpu, u32 id, FSArchive *archive, u32 arm9ovt,
                                    u32 arm9ovtSize, u32 arm7ovt, u32 arm7ovtSize) {
    FSFile file;
    FS_UnkStruct21 spC;
    u32 ovtOffset;
    u32 overlayOffset;
    u32 ovtSize;

    if (cpu == OS_CPU_ARM9) {
        ovtOffset = arm9ovt;
        ovtSize   = arm9ovtSize;
    } else {
        ovtOffset = arm7ovt;
        ovtSize   = arm7ovtSize;
    }
    overlayOffset = id * sizeof(FSOverlay);
    if (overlayOffset >= ovtSize) {
        return false;
    }
    FS_InitFile(&file);
    if (!FS_OpenFileDirect(&file, archive, ovtOffset + overlayOffset, ovtOffset + ovtSize, -1)) {
        return false;
    }
    if (FS_ReadFile(&file, &info->overlay, sizeof(info->overlay)) != sizeof(info->overlay)) {
        FS_CloseFile(&file);
        return false;
    }
    FS_CloseFile(&file);
    info->cpu = cpu;
    spC       = FS_GetOverlayFileID(&info->overlay);
    if (!FS_OpenFileFast(&file, spC)) {
        return false;
    }
    info->unk_24 = file.unk_20.unk_04u;
    info->unk_28 = file.unk_20.unk_08u - file.unk_20.unk_04u;
    FS_CloseFile(&file);
    return true;
}

static BOOL FS_LoadOverlayInfo(FSOverlayInfo *info, OSCpu cpu, u32 id) {
    FSFile file;
    FS_UnkStruct21 sp10;
    FS_UnkStruct23 *var_r0;
    void *overlays;
    u32 offset;

    if (cpu == OS_CPU_ARM9) {
        var_r0 = &sArm9OverlayTable;
    } else {
        var_r0 = &sArm7OverlayTable;
    }
    overlays = var_r0->overlays;
    if (overlays != NULL) {
        offset = id * sizeof(FSOverlay);
        if (offset >= var_r0->size) {
            return false;
        }
        MI_CpuCopy8(overlays + offset, &info->overlay, sizeof(info->overlay));
        info->cpu = cpu;
        FS_InitFile(&file);
        sp10 = FS_GetOverlayFileID(&info->overlay);
        if (!FS_OpenFileFast(&file, sp10)) {
            return false;
        }
        info->unk_24 = file.unk_20.unk_04u;
        info->unk_28 = file.unk_20.unk_08u - file.unk_20.unk_04u;
        FS_CloseFile(&file);
        return true;
    }
    return FSi_LoadOverlayInfoCore(info, cpu, id, &FS_romArchive, REG_ROM_HEADER.arm9ovt.offset,
                                   REG_ROM_HEADER.arm9ovt.size, REG_ROM_HEADER.arm7ovt.offset,
                                   REG_ROM_HEADER.arm7ovt.size);
}

static BOOL FS_LoadOverlayImage(FSOverlay *overlay) {
    FSFile file;
    FS_UnkStruct21 sp0;
    u32 size;

    FS_InitFile(&file);
    sp0 = FS_GetOverlayFileID(overlay);
    if (!FS_OpenFileFast(&file, sp0)) {
        return false;
    }
    size = FSi_GetOverlayBinarySize(overlay);
    FS_ClearOverlayImage(overlay);
    if (FS_ReadFile(&file, overlay->addr, size) != size) {
        FS_CloseFile(&file);
        return false;
    }
    FS_CloseFile(&file);
    return true;
}

static s32 FSi_CompareDigest(u8 (*signature)[0x14], void *data, u32 length) {
    u8 digest[0x14];
    u8 sp4[0x40];
    u32 i;

    MI_CpuFill8(digest, 0, sizeof(digest));
    MI_CpuCopy8(sHmacKey.key, &sp4, sHmacKey.keyLength);
    DGT_Hash2CalcHmac(&digest, data, length, &sp4, sHmacKey.keyLength);

    for (i = 0; i < 0x14; i += 4) {
        if (*(u32 *) (digest + i) != *(u32 *) (*signature + i)) {
            break;
        }
    }
    return i == 0x14;
}

static void FS_StartOverlay(FSOverlay *overlay) {
    BOOL validDigest;
    FSStaticInit *end;
    u32 size;
    FSStaticInit *ctor;

    size = FSi_GetOverlayBinarySize(overlay);
    if (REG_027FFC40 == 2) {
        validDigest = false;
        if (FS_OVERLAY_FLAGS(overlay) & FS_OVERLAY_FLAG_SIGNED) {
            // TODO: Link-time constants for start and end of overlay signature table
            if (overlay->id < &OverlaySignaturesEnd - &OverlaySignatures) {
                validDigest = FSi_CompareDigest(&OverlaySignatures + overlay->id, overlay->addr, size);
            }
        }
        if (!validDigest) {
            MI_CpuFill8(overlay->addr, 0, size);
            OS_Panic();
            return;
        }
    }
    if (1 & ((u32) overlay->fileSize >> 0x18)) {
        MIi_UncompressBackward(overlay->addr + size);
    }
    DC_FlushRange(overlay->addr, overlay->textSize);

    // Call static initializers
    for (ctor = overlay->ctorStart, end = overlay->ctorEnd; ctor < end; ++ctor) {
        if (*ctor != NULL) {
            (*ctor)();
        }
    }
}

static void FS_EndOverlay(FSOverlay *overlay) {
    void *start;
    DestructorChain *var_r1;
    DestructorChain *iter;
    DestructorChain *spC;
    void *end;
    void (*destructor)(void *);
    DestructorChain *next;
    DestructorChain *var_r6;
    DestructorChain *var_r7;
    OSIntrMode irq;
    void *object;

    while (true) {
        var_r6 = NULL;
        var_r7 = NULL;
        start  = overlay->addr;
        end    = start + (overlay->textSize + overlay->bssSize);
        irq    = OS_DisableInterrupts();
        var_r1 = NULL;
        iter   = __global_destructor_chain;
        spC    = iter;
        while (iter != NULL) {
            object     = iter->object;
            next       = iter->next;
            destructor = iter->destructor;
            if ((object == NULL && (void *) destructor >= start && (void *) destructor < end) ||
                (object >= start && object < end)) {
                if (var_r7 == NULL) {
                    var_r6 = iter;
                } else {
                    var_r7->next = iter;
                }
                if (spC == iter) {
                    spC                       = next;
                    __global_destructor_chain = next;
                }
                var_r7     = iter;
                iter->next = NULL;
                if (var_r1 != NULL) {
                    var_r1->next = next;
                }
            } else {
                var_r1 = iter;
            }
            iter = next;
        }
        OS_RestoreInterrupts(irq);

        if (var_r6 != NULL) {
            do {
                destructor = var_r6->destructor;
                next       = var_r6->next;
                if (destructor != NULL) {
                    destructor(var_r6->object);
                }
                var_r6 = next;
            } while (next != NULL);
        } else {
            break;
        }
    }
}

static BOOL FS_UnloadOverlayImage(FSOverlay *overlay) {
    FS_EndOverlay(overlay);
    return true;
}

BOOL FS_LoadOverlay(u32 arg0, u32 id) {
    FSOverlayInfo info;

    if (!FS_LoadOverlayInfo(&info, arg0, id) || !FS_LoadOverlayImage(&info.overlay)) {
        return false;
    }
    FS_StartOverlay(&info.overlay);
    return true;
}

BOOL FS_UnloadOverlay(u32 arg0, u32 id) {
    FSOverlayInfo info;

    if (!FS_LoadOverlayInfo(&info, arg0, id) || !FS_UnloadOverlayImage(&info.overlay)) {
        return false;
    }
    return true;
}
