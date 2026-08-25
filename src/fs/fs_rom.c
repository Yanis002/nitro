#include "nitro/card.h"
#include "nitro/fs.h"
#include "nitro/os.h"

typedef struct FS_UnkStruct3 {
    /* 00 */ u32 dmaCount;
    /* 04 */ u32 lock;
    /* 08 */
} FS_UnkStruct3;

static FSArchive sRomArchive;
static FS_UnkStruct3 FS_data_0005;

static void FSi_OnRomReadDone(void *archive) {
    FS_NotifyArchiveAsyncEnd((FSArchive *) archive, CARD_IsPulledOut() ? FS_RESULT_0x5 : FS_RESULT_SUCCESS);
}

static FSResult FSi_ReadRomCallback(FSArchive *archive, void *arg1, void *arg2, s32 size) {
    CARDi_ReadRom(FS_data_0005.dmaCount, arg2, arg1, FSi_OnRomReadDone, archive, 1);
    return FS_RESULT_AWAIT_ASYNC;
}

static FSResult FSi_RomArchiveProc(FSFile *file, u32 index) {
    switch (index) {
        case FS_FILE_PROC_SUSPEND:
            CARD_LockRom(FS_data_0005.lock);
            return FS_RESULT_SUCCESS;
        case FS_FILE_PROC_UNLOCK:
            CARD_UnlockRom(FS_data_0005.lock);
            return FS_RESULT_SUCCESS;
        case FS_FILE_PROC_WRITE:
            return FS_RESULT_INVALID_COMMAND;
        default:
            return FS_RESULT_0x102;
    }
}

static FSResult FSi_EmptyArchiveProc(FSFile *file, u32 index) {
    return index == FS_CMD_WRITE_FILE ? FS_RESULT_INVALID_COMMAND : FS_RESULT_0x102;
}

static s32 FSi_ReadDummyCallback(FSArchive *archive, void *arg1, void *arg2, s32 size) {
    return FS_RESULT_INVALID_COMMAND;
}

static s32 FSi_WriteDummyCallback(FSArchive *archive, void *arg1, void *arg2, s32 size) {
    return FS_RESULT_INVALID_COMMAND;
}

static s32 FS_func_0099(FSArchive *arg0) {
    return 0;
}

BOOL FSi_InitRom(s32 dmaCount) {
    s32 temp_r1;
    s32 temp_r2;
    CARD_UnkStruct1 *temp_r5;
    CARD_UnkStruct1 *temp_r6;

    CARD_Init();
    FS_data_0005.dmaCount = dmaCount;
    FS_data_0005.lock     = OS_GetLockID();
    FS_InitArchive(&sRomArchive);
    FS_RegisterArchiveName(&sRomArchive, "rom", 3U);
    if (OS_func_0159() == 1) {
        temp_r6 = CARD_func_0059();
        temp_r5 = CARD_func_0059();
        FS_SetArchiveProc(&sRomArchive, FSi_RomArchiveProc,
                          (1 << FS_FILE_PROC_WRITE) | (1 << FS_FILE_PROC_7) | (1 << FS_FILE_PROC_SUSPEND) |
                              (1 << FS_FILE_PROC_UNLOCK));
        temp_r1 = temp_r6->unk_40;
        if ((temp_r1 != -1) && (temp_r1 != 0) && (temp_r2 = temp_r5->unk_48, (temp_r2 != -1)) && (temp_r2 != 0)) {
            FS_LoadArchive(&sRomArchive, 0, temp_r2, temp_r5->unk_4c, temp_r1, temp_r6->unk_44, FSi_ReadRomCallback,
                           NULL);
        }
    } else {
        FS_func_0099(&sRomArchive);
    }
    if (!!(sRomArchive.flags & FS_ARCHIVE_FLAG_0x2) == false) {
        FS_SetArchiveProc(&sRomArchive, FSi_EmptyArchiveProc, -1);
        FS_LoadArchive(&sRomArchive, 0, 0, 0, 0, 0, FSi_ReadDummyCallback, FSi_WriteDummyCallback);
    }
    return FS_func_0044("rom:");
}
