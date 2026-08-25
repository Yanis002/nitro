#include "nitro/fs.h"

#include "nitro/card.h"
#include "nitro/mi.h"
#include "nitro/os.h"
#include "nitro/wm.h"

// `true` if this is the first byte of a wide character in the Shift JIS character set
#define FSi_CHAR_IS_WIDE(ch) ((((u8) (ch) ^ 0x20) - 0xa1) < 0x3cu)

typedef struct FS_UnkStruct3 {
    /* 00 */ u32 dmaCount;
    /* 04 */ u32 lock;
    /* 08 */
} FS_UnkStruct3;

typedef struct FS_UnkStruct5 {
    /* 00 */ FSArchive *unk_00;
    /* 04 */ FSArchive *unk_04;
    /* 08 */ u16 unk_08;
    /* 0a */ u16 unk_0a;
    /* 0c */ u32 unk_0c;
    /* 10 */
} FS_UnkStruct5;

typedef struct FS_UnkStruct10 {
    /* 00 */ union {
        void *dst;
        u8 unk_00b;
        u32 unk_00u;
        struct {
            u16 unk_00s;
            u16 unk_02s;
        };
    };
    /* 04 */ union {
        u32 unk_04u;
        struct {
            u16 unk_04s;
            u16 unk_06s;
        };
    };
    /* 08 */
} FS_UnkStruct10;

typedef struct FS_UnkStruct11 {
    /* 00 */ FSArchive *archive;
    /* 04 */ void *src;
    /* 08 */
} FS_UnkStruct11;

typedef struct FS_UnkStruct14 {
    /* 00 */ s32 pos;
    /* 04 */
} FS_UnkStruct14;

typedef struct FS_UnkStruct17 {
    /* 00 */ FSArchive *archive;
    /* 04 */ union {
        u32 unk_04u;
        struct {
            u16 unk_04s;
            u16 unk_06s;
        };
    };
    /* 06 */ u32 unk_08;
    /* 0c */ u32 unk_0c;
    /* 10 */ u32 unk_10;
    /* 14 */ u8 unk_14[0x80];
    /* 94 */
} FS_UnkStruct17;

typedef struct FS_UnkStruct18 {
    /* 00 */ FSArchive *archive;
    /* 04 */ u32 unk_04;
    /* 08 */ u32 unk_08;
    /* 0c */ u32 unk_0c;
    /* 10 */ u32 unk_10;
    /* 14 */ const u8 *unk_14;
    /* 18 */ u32 unk_18;
    /* 1c */
} FS_UnkStruct18;

static BOOL FS_func_0035(u8 arg0);
static FSResult FS_func_0037(FSFile *file, FSResult arg1);
static FSFile *FSi_NextCommand(FSArchive *arg0, BOOL arg1);
static void FSi_ExecuteAsyncCommand(FSFile *file);
static void FSi_ExecuteSyncCommand(FSFile *file);
static BOOL FSi_SendCommand(FSFile *file, s32 cmdType, BOOL sync);
static s32 FSi_TranslateCommand(FSFile *file, u8 cmdType);
static void FSi_ReleaseCommand(FSFile *file, s32 arg1);
static BOOL FSi_SeekFileFromRom(FSFile *arg0, FS_UnkStruct14 arg1, s32 mode);
static FSArchive *FS_FindArchive(const char *name, u32 length);
static char *FSi_GetPackedName(FSArchive *archive);
static s32 FS_func_0053(char *arg0, s32 arg1);
static s32 FS_func_0054(char *arg0);
static BOOL FS_func_0044(char *arg0);
static s32 FS_func_0045(char *arg0, s32 arg1, const char *arg2, s32 arg3, BOOL *arg4);
static FSArchive *FS_func_0046(const char *path, u32 *arg1, char (*arg2)[FS_MAX_PATH]);
static void FS_InitArchive(FSArchive *archive);
static s32 FS_RegisterArchiveName(FSArchive *archive, const char *name, u32 length);
static BOOL FS_func_0049(FSArchive *archive, FS_UnkStruct7 *arg1, const FSArchiveFns *fns);
static void FS_NotifyArchiveAsyncEnd(FSArchive *archive, FSResult result);
static void FS_func_0060(void);
static FSResult FSi_ReadMemCallback(FSArchive *archive, void *arg1, void *arg2, s32 size);
static FSResult FSi_WriteMemCallback(FSArchive *archive, void *arg1, void *arg2, s32 size);
static void FS_LoadArchive(FSArchive *archive, s32 arg1, s32 arg2, s32 arg3, s32 arg4, s32 arg5,
                           FSResult (*read)(FSArchive *, void *, void *, s32),
                           FSResult (*write)(FSArchive *, void *, void *, s32));
static FSResult FSi_RomReadFile(FSArchive *archive, FSFile *file, u32 size, FSiCmdReadFile *cmd);
static FSResult FSi_RomWriteFile(FSArchive *archive, FSFile *file, u32 arg2, FSiCmd1 *cmd);
static FSResult FS_func_0066(FSArchive *archive, FSFile *file, u32 arg2, u32 arg3);
static FSResult FS_func_0067(FSArchive *archive, FSFile *file, FS_UnkStruct13 *arg2);
static FSResult FS_func_0068(FSArchive *archive, u32 arg1, char *arg2, u32 *arg3, u32 arg4);
static FSResult FS_func_0069(FSArchive *archive, FSFile *file, u32 arg2, u32 arg3, u32 *arg4);
static FSResult FS_func_0070(FSArchive *archive, FSFile *file, u32 arg2, u32 arg3);
static FSResult FS_func_0071(FSArchive *archive, FSFile *file, u32 arg2, u32 arg3, FSiCmd7 *cmd);
static FSResult FSi_RomCloseFile(FSArchive *archive, FSFile *file);
static FSResult FSi_RomLock(FSArchive *archive);
static FSResult FSi_RomUnlock(FSArchive *archive);
static FSResult FS_func_0075(FSArchive *archive);
static FSResult FS_func_0076(FSArchive *archive);
static FSResult FSi_RomOpenFile(FSArchive *archive, FSFile *file, u32 arg2, char *arg3, u32 flags);
static FSResult FSi_RomSeekFile(FSArchive *archive, struct FSFile *file, s32 *pos, u32 mode);
static FSResult FSi_RomGetLength(FSArchive *archive, struct FSFile *file, FSiCmdGetLength *cmd);
static FSResult FSi_RomGetPosition(FSArchive *archive, FSFile *file, FSiCmdGetPosition *cmd);
static FSResult FS_func_0080(FSArchive *archive);
static FSResult FS_func_0081(FSArchive *archive, FSiCmd19 *cmd);
static FSResult FS_func_0084(FSArchive *archive, u32 arg1, char *arg2, FS_UnkStruct15 *arg3);
static FSResult FS_func_0085(FSArchive *archive, FS_UnkStruct16 *arg1);
static FSResult FSi_RomOpenDir(FSArchive *archive, FSFile *file, u32 arg2, char *arg3, u32 arg4);
static FSResult FSi_RomCloseDir(FSArchive *archive, FSFile *file);
static FSResult FSi_FileProcRead(FSFile *arg0);
static FSResult FSi_FileProcWrite(FSFile *arg0);
static FSResult FS_func_0007(FSFile *arg0);
static FSResult FS_func_0008(FSFile *arg0);
static FSResult FS_func_0009(FSFile *arg0);
static FSResult FS_func_0010(FSFile *arg0);
static FSResult FS_func_0011(FSFile *arg0);
static FSResult FS_func_0012(FSFile *arg0);
static FSResult FSi_FileProcNop(FSFile *arg0);
static FSResult FSi_ExecuteFileProc(FSFile *file, u32 index, BOOL wait);
static s32 FS_func_0003(FS_UnkStruct11 *arg0, void *dst, s32 size);
static void *FS_func_0092(FSArchive *archive, void *arg1);
static BOOL FS_func_0093(FSArchive *arg0);
static BOOL FSi_GetLengthFromRom(FSFile *file, FSiCmdGetLength *cmd);
static BOOL FSi_GetPositionFromRom(FSFile *arg0, FSiCmdGetPosition *arg1);
static s32 FSi_GetPosition(FSFile *file);
static s32 FS_func_0095(FSArchive *archive, u32 arg1);
static s32 FS_func_0051(char *path, s32 arg1);
static s32 FS_func_0052(char *path, s32 start);
static void FS_func_0055(FSFile *file, FSArchive *archive, s32 arg2, s32 arg3, s32 arg4);
static FSResult FS_func_0057(FSFile *file, const char *path, s32 arg2);
static FSResult FS_func_0058(FSFile *file, FS_UnkStruct13 *arg1);
static void FS_func_0059(FSDirEntry *dir, FSArchive *archive, FS_UnkStruct13 *arg2);
static void FS_func_0062(FSFile *file, u32 arg1);
static void FS_SetArchiveProc(FSArchive *archive, FSResult (*proc)(FSFile *file, FSFileProc index), u32 overrideMask);
static s32 FS_func_0094(FSFile *file);
static void FSi_OnRomReadDone(void *archive);
static FSResult FSi_ReadRomCallback(FSArchive *archive, void *arg1, void *arg2, s32 size);
static FSResult FSi_RomArchiveProc(FSFile *file, u32 index);
static FSResult FSi_EmptyArchiveProc(FSFile *file, u32 index);
static s32 FSi_ReadDummyCallback(FSArchive *archive, void *arg1, void *arg2, s32 size);
static s32 FSi_WriteDummyCallback(FSArchive *archive, void *arg1, void *arg2, s32 size);
static s32 FS_func_0099(FSArchive *arg0);
static BOOL FSi_InitRom(s32 dmaCount);

static const u8 FS_data_0001[64] = {
    0x21, 0x06, 0xC0, 0xDE, 0xBA, 0x98, 0xCE, 0x3F, 0xA6, 0x92, 0xE3, 0x9D, 0x46, 0xF2, 0xED, 0x01,
    0x76, 0xE3, 0xCC, 0x08, 0x56, 0x23, 0x63, 0xFA, 0xCA, 0xD4, 0xEC, 0xDF, 0x9A, 0x62, 0x78, 0x34,
    0x8F, 0x6D, 0x63, 0x3C, 0xFE, 0x22, 0xCA, 0x92, 0x20, 0x88, 0x97, 0x23, 0xD2, 0xCF, 0xAE, 0xC2,
    0x32, 0x67, 0x8D, 0xFE, 0xCA, 0x83, 0x64, 0x98, 0xAC, 0xFD, 0x3E, 0x37, 0x87, 0x46, 0x58, 0x24,
};

static const FSFileProcs sFileProcs = {
    [FS_FILE_PROC_READ]    = FSi_FileProcRead, //
    [FS_FILE_PROC_WRITE]   = FSi_FileProcWrite, //
    [FS_FILE_PROC_2]       = FS_func_0007, //
    [FS_FILE_PROC_3]       = FS_func_0008, //
    [FS_FILE_PROC_4]       = FS_func_0009, //
    [FS_FILE_PROC_5]       = FS_func_0010, //
    [FS_FILE_PROC_6]       = FS_func_0011, //
    [FS_FILE_PROC_7]       = FS_func_0012, //
    [FS_FILE_PROC_CLOSE]   = FSi_FileProcNop, //
    [FS_FILE_PROC_SUSPEND] = FSi_FileProcNop, //
    [FS_FILE_PROC_UNLOCK]  = FSi_FileProcNop, //
    [FS_FILE_PROC_11]      = FSi_FileProcNop, //
    [FS_FILE_PROC_12]      = FSi_FileProcNop, //
};

static const FSArchiveFns sArchiveFns = {
    .read        = FSi_RomReadFile,
    .write       = FSi_RomWriteFile,
    .unk_08      = FS_func_0066,
    .unk_0c      = FS_func_0067,
    .unk_10      = FS_func_0068,
    .unk_14      = FS_func_0069,
    .unk_18      = FS_func_0070,
    .unk_1c      = FS_func_0071,
    .close       = FSi_RomCloseFile,
    .lock        = FSi_RomLock,
    .unlock      = FSi_RomUnlock,
    .unk_2c      = FS_func_0075,
    .unk_30      = FS_func_0076,
    .open        = FSi_RomOpenFile,
    .seek        = FSi_RomSeekFile,
    .getLength   = FSi_RomGetLength,
    .getPosition = FSi_RomGetPosition,
    .unk_44      = NULL,
    .unk_48      = FS_func_0080,
    .unk_4c      = FS_func_0081,
    .unk_50      = NULL,
    .unk_54      = NULL,
    .unk_58      = NULL,
    .unk_5c      = FS_func_0084,
    .unk_60      = NULL,
    .unk_64      = NULL,
    .unk_68      = NULL,
    .unk_6c      = NULL,
    .unk_70      = FS_func_0085,
    .unk_74      = NULL,
    .unk_78      = NULL,
    .unk_7c      = NULL,
    .openDir     = FSi_RomOpenDir,
    .closeDir    = FSi_RomCloseDir,
    .unk_88      = NULL,
};

static FS_UnkStruct18 FS_data_0002;
static FS_UnkStruct5 FS_data_0003;
static char sLongArchiveNames[0x10][0x10];
static char FS_data_0004[FS_MAX_PATH];
static FS_UnkStruct3 FS_data_0005;
static FSArchive sRomArchive;
static BOOL FSi_Initialized;

// clang-format off
#define FS_func_0035_MASK (                \
    (1 << (FS_CMD_LOCK   - FS_CMD_LOCK)) | \
    (1 << (FS_CMD_UNLOCK - FS_CMD_LOCK)) | \
    (1 << (FS_CMD_11     - FS_CMD_LOCK)) | \
    (1 << (FS_CMD_12     - FS_CMD_LOCK)) | \
    (1 << (FS_CMD_17     - FS_CMD_LOCK)) | \
    (1 << (FS_CMD_18     - FS_CMD_LOCK)) | \
    (1 << (FS_CMD_COUNT  - FS_CMD_LOCK))   \
)
// clang-format on

static BOOL FS_func_0035(u8 cmdType) {
    BOOL result;
    u32 bit;

    result = false;
    bit    = cmdType - FS_CMD_LOCK;
    if (bit > FS_CMD_COUNT - FS_CMD_LOCK) {
        return false;
    }
    if (FS_func_0035_MASK & (1 << bit)) {
        result = true;
    }
    return result;
}

static void FSi_ReleaseCommand(FSFile *file, FSResult result) {
    FSFile *var_r0;
    FSFile **var_r1;
    OSIntrMode irq;
    u8 cmdType;
    FSArchive *archive;

    irq     = OS_DisableInterrupts();
    archive = file->archive;
    if (archive != NULL) {
        var_r0 = archive->currentFile;
        var_r1 = &archive->currentFile;
        while (var_r0 != NULL) {
            if (var_r0 == file) {
                *var_r1 = file->next;
                break;
            }
            var_r1 = &var_r0->next;
            var_r0 = var_r0->next;
        }
        file->next = NULL;
    }
    cmdType = FS_FILE_FLAG_CMD_TYPE(file->flags);
    if ((FS_func_0035(cmdType) == 0) && (archive != NULL)) {
        archive->unk_18 = (s32) cmdType;
        archive->unk_1c = result;
    }
    file->unk_14 = result;
    file->flags  = (u32) (file->flags & ~(FS_FILE_FLAG_SEND_CMD | FS_FILE_FLAG_0x2 | FS_FILE_FLAG_AWAIT_SYNC |
                                          FS_FILE_FLAG_0x8 | FS_FILE_FLAG_0x40 | FS_FILE_FLAG_0x80));
    OS_WakeupThread(&file->unk_18);
    OS_RestoreInterrupts(irq);
}

static FSResult FS_func_0037(FSFile *file, FSResult result) {
    OSIntrMode irq;

    if (result == 0x100) {
        irq = OS_DisableInterrupts();
        while (!(file->flags & FS_FILE_FLAG_0x8)) {
            OS_SleepThread(&file->unk_18);
        }
        OS_RestoreInterrupts(irq);
        result = file->unk_14;
        file->flags &= ~FS_FILE_FLAG_0x8;
    }
    return result;
}

static FSResult FSi_TranslateCommand(FSFile *file, u8 cmdType) {
    FSResult result;
    const FSArchiveFns *fns;
    FSArchive *archive;

    archive = file->archive;
    fns     = archive->fns;
    if (cmdType >= FS_CMD_COUNT) {
        result = FS_RESULT_INVALID_COMMAND;
    } else if (fns->array[cmdType] == NULL) {
        result = FS_RESULT_INVALID_COMMAND;
    } else {
        switch (cmdType) {
            case FS_CMD_READ_FILE:
                result = fns->read(archive, file, file->cmd.readFile->size, file->cmd.readFile);
                break;
            case FS_CMD_WRITE_FILE:
                result = fns->write(archive, file, file->cmd.cmd1->unk_04, file->cmd.cmd1);
                break;
            case FS_CMD_2:
                result = fns->unk_08(archive, file, file->cmd.cmd2->unk_00, file->cmd.cmd2->unk_04);
                break;
            case FS_CMD_3:
                result = fns->unk_0c(archive, file, file->cmd.cmd3->unk_00);
                break;
            case FS_CMD_4:
                result = fns->unk_10(archive, file->cmd.cmd4->unk_00, file->cmd.cmd4->path, &file->cmd.cmd4->unk_08,
                                     file->cmd.cmd4->unk_0c);
                break;
            case FS_CMD_5:
                result =
                    fns->unk_14(archive, file, file->cmd.cmd5->unk_00, file->cmd.cmd5->unk_04, &file->cmd.cmd5->unk_08);
                break;
            case FS_CMD_6:
                result = fns->unk_18(archive, file, file->cmd.cmd6->unk_00, file->cmd.cmd6->unk_04);
                break;
            case FS_CMD_7:
                result = fns->unk_1c(archive, file, file->cmd.cmd7->unk_04, file->cmd.cmd7->unk_08, file->cmd.cmd7);
                break;
            case FS_CMD_CLOSE_FILE:
                result = fns->close(archive, file);
                break;
            case FS_CMD_LOCK:
                fns->lock(archive);
                return 0;
            case FS_CMD_UNLOCK:
                fns->unlock(archive);
                return 0;
            case FS_CMD_11:
                fns->unk_2c(archive);
                return 0;
            case FS_CMD_12:
                fns->unk_30(archive);
                return 0;
            case FS_CMD_OPEN_FILE:
                result = fns->open(archive, file, file->cmd.openFile->unk_00, file->cmd.openFile->path,
                                   file->cmd.openFile->flags);
                break;
            case FS_CMD_SEEK_FILE:
                result = fns->seek(archive, file, &file->cmd.seekFile->pos, file->cmd.seekFile->mode);
                break;
            case FS_CMD_GET_LENGTH:
                result = fns->getLength(archive, file, file->cmd.getLength);
                break;
            case FS_CMD_GET_POSITION:
                result = fns->getPosition(archive, file, file->cmd.getPosition);
                break;
            case FS_CMD_17:
                fns->unk_44(archive);
                return 0;
            case FS_CMD_18:
                fns->unk_48(archive);
                return 0;
            case FS_CMD_19:
                result = fns->unk_4c(archive, file->cmd.cmd19);
                break;
            case FS_CMD_20:
                result =
                    fns->unk_50(archive, file->cmd.cmd20->unk_00, file->cmd.cmd20->unk_04, file->cmd.cmd20->unk_08);
                break;
            case FS_CMD_21:
                result = fns->unk_54(archive, file->cmd.cmd21->unk_00, file->cmd.cmd21->unk_04);
                break;
            case FS_CMD_22:
                result = fns->unk_58(archive, file->cmd.cmd22->unk_00, file->cmd.cmd22->unk_04, file->cmd.cmd22->unk_08,
                                     file->cmd.cmd22->unk_0c);
                break;
            case FS_CMD_23:
                result = fns->unk_5c(archive, file->cmd.cmd23->unk_00, file->cmd.cmd23->path, file->cmd.cmd23->unk_08);
                break;
            case FS_CMD_24:
                result =
                    fns->unk_60(archive, file->cmd.cmd24->unk_00, file->cmd.cmd24->unk_04, file->cmd.cmd24->unk_08);
                break;
            case FS_CMD_25:
                result =
                    fns->unk_64(archive, file->cmd.cmd25->unk_00, file->cmd.cmd25->unk_04, file->cmd.cmd25->unk_08);
                break;
            case FS_CMD_26:
                result = fns->unk_68(archive, file->cmd.cmd26->unk_00, file->cmd.cmd26->unk_04);
                break;
            case FS_CMD_27:
                result = fns->unk_6c(archive, file->cmd.cmd27->unk_00, file->cmd.cmd27->unk_04, file->cmd.cmd27->unk_08,
                                     file->cmd.cmd27->unk_0c);
                break;
            case FS_CMD_28:
                result = fns->unk_70(archive, file->cmd.cmd28->unk_00);
                break;
            case FS_CMD_30:
                result = fns->unk_78(archive, file);
                break;
            case FS_CMD_31:
                result = fns->unk_7c(archive, file, file->cmd.cmd31->unk_00);
                break;
            case FS_CMD_OPEN_DIR:
                result = fns->openDir(archive, file, file->cmd.cmd32->unk_00, file->cmd.cmd32->path,
                                      file->cmd.cmd32->unk_08);
                break;
            case FS_CMD_CLOSE_DIR:
                result = fns->closeDir(archive, file);
                break;
            case FS_CMD_34:
                result = fns->unk_88(archive, file, file->cmd.cmd34->unk_00, file->cmd.cmd34->unk_04);
                break;
            case FS_CMD_29:
            default:
                result = FS_RESULT_INVALID_COMMAND;
                break;
        }
    }
    if (FS_func_0035(cmdType) == 0) {
        if (file->flags & FS_FILE_FLAG_AWAIT_SYNC) {
            result = FS_func_0037(file, result);
        } else if (result != FS_RESULT_AWAIT_ASYNC) {
            FSi_ReleaseCommand(file, result);
        }
    }
    return result;
}

static FSFile *FSi_NextCommand(FSArchive *archive, BOOL arg1) {
    FSFile tempFile;
    FSFile *fileIter;
    FSFile *result;
    FSFile *nextFile;
    s32 flags;
    OSIntrMode irq;
    s32 var_r5;

    result = NULL;

    irq   = OS_DisableInterrupts();
    flags = archive->flags;
    if (flags & 0x20) {
        fileIter       = archive->currentFile;
        archive->flags = flags & ~FS_ARCHIVE_FLAG_0x20;
        while (fileIter != NULL) {
            nextFile = fileIter->next;
            if (!!(fileIter->flags & FS_FILE_FLAG_0x2) != 0 && !(fileIter->flags & FS_FILE_FLAG_0x40)) {
                FSi_ReleaseCommand(fileIter, FS_RESULT_0x3);
                if (nextFile == NULL) {
                    nextFile = archive->currentFile;
                }
            }
            fileIter = nextFile;
        }
    }
    OS_RestoreInterrupts(irq);

    irq   = OS_DisableInterrupts();
    flags = archive->flags;
    if (!(flags & FS_ARCHIVE_FLAG_0x40) && !(flags & FS_ARCHIVE_FLAG_0x8) && (archive->currentFile != NULL)) {
        if (arg1 && !(flags & FS_ARCHIVE_FLAG_0x10)) {
            var_r5 = 1;
        } else {
            var_r5 = 0;
        }
        if (var_r5 != 0) {
            archive->flags = (s32) (archive->flags | FS_ARCHIVE_FLAG_0x10);
        }
        OS_RestoreInterrupts(irq);

        if (var_r5 != 0) {
            FSi_TranslateCommand(archive->currentFile, FS_CMD_LOCK);
        }

        irq = OS_DisableInterrupts();
        if (arg1 || var_r5 != 0) {
            result        = archive->currentFile;
            result->flags = (s32) (result->flags | FS_ARCHIVE_FLAG_0x40);
        }
        if (arg1 && (result->flags & FS_ARCHIVE_FLAG_0x4)) {
            OS_WakeupThread(&result->unk_18);
            result = NULL;
        }
    } else if (arg1) {
        if (flags & FS_ARCHIVE_FLAG_0x10) {
            FS_InitFile(&tempFile);
            tempFile.archive = archive;
            archive->flags   = (s32) (archive->flags & ~FS_ARCHIVE_FLAG_0x10);
            FSi_TranslateCommand(&tempFile, FS_CMD_UNLOCK);
        }
        flags = archive->flags;
        if (flags & FS_ARCHIVE_FLAG_0x40) {
            archive->flags = (flags & ~FS_ARCHIVE_FLAG_0x40) | FS_ARCHIVE_FLAG_0x8;
            OS_WakeupThread(&archive->unk_0c);
        }
    }
    OS_RestoreInterrupts(irq);

    return result;
}

static void FSi_ExecuteAsyncCommand(FSFile *file) {
    s32 flags;
    OSIntrMode irq;
    FSArchive *archive;

    archive = file->archive;
    while (file != NULL) {
        irq         = OS_DisableInterrupts();
        flags       = file->flags | FS_FILE_FLAG_0x40;
        file->flags = flags;
        if (flags & FS_FILE_FLAG_AWAIT_SYNC) {
            OS_WakeupThread(&file->unk_18);
            file = NULL;
        }
        OS_RestoreInterrupts(irq);

        if (file == NULL) {
            break;
        }
        if (FSi_TranslateCommand(file, FS_FILE_FLAG_CMD_TYPE(file->flags)) == FS_RESULT_AWAIT_ASYNC) {
            break;
        }
        file = FSi_NextCommand(archive, true);
    }
}

static void FSi_ExecuteSyncCommand(FSFile *file) {
    OSIntrMode irq;
    u32 flags;
    void *temp_r4_2;
    FSFile *nextFile;

    irq = OS_DisableInterrupts();
    for (flags = file->flags; !(flags & FS_FILE_FLAG_0x40) && (flags & FS_FILE_FLAG_SEND_CMD); flags = file->flags) {
        OS_SleepThread(&file->unk_18);
    }
    OS_RestoreInterrupts(irq);

    if (!(file->flags & FS_FILE_FLAG_0x40)) {
        return;
    }
    temp_r4_2 = file->archive;
    FSi_ReleaseCommand(file, FSi_TranslateCommand(file, FS_FILE_FLAG_CMD_TYPE(file->flags)));
    nextFile = FSi_NextCommand(temp_r4_2, true);
    if (nextFile == NULL) {
        return;
    }
    FSi_ExecuteAsyncCommand(nextFile);
}

static BOOL FSi_SendCommand(FSFile *file, s32 cmdType, BOOL sync) {
    FSFile *temp_r0_2;
    FSFile **var_r0_2;
    FSFile *var_r1;
    u32 flags;
    OSIntrMode irq;
    s32 var_r0;
    BOOL var_r6;
    BOOL result;
    FSArchive *archive;

    result  = false;
    archive = file->archive;
    if (!!(file->flags & FS_FILE_FLAG_SEND_CMD) != 0) {
        OS_Panic();
    }
    if (archive == NULL) {
        file->unk_14 = 6;
        return false;
    }

    flags        = (file->flags & ~FS_FILE_FLAG_CMD_TYPE_MASK) | FS_CMD_FILE_FLAG(cmdType) | FS_FILE_FLAG_SEND_CMD;
    file->flags  = flags;
    file->next   = NULL;
    file->unk_14 = 2;
    if (sync) {
        file->flags = flags | FS_FILE_FLAG_AWAIT_SYNC;
    }
    irq = OS_DisableInterrupts();
    if (archive->flags & FS_ARCHIVE_FLAG_0x80) {
        FSi_ReleaseCommand(file, FS_RESULT_0x3);
    } else {
        var_r1   = archive->currentFile;
        var_r0_2 = &archive->currentFile;
        if (var_r1 != NULL) {
            do {
                var_r0_2 = &var_r1->next;
                var_r1   = var_r1->next;
            } while (var_r1 != NULL);
        }
        *var_r0_2 = file;
    }
    var_r6 = false;
    if ((archive->currentFile == file) && !(archive->flags & FS_ARCHIVE_FLAG_0x10)) {
        var_r6 = true;
    }
    OS_RestoreInterrupts(irq);
    if (file->unk_14 != 3) {
        temp_r0_2 = FSi_NextCommand(archive, var_r6);
        if (sync) {
            FSi_ExecuteSyncCommand(file);
            result = true;
            if (file->unk_14 != 0) {
                result = false;
            }
        } else {
            if (temp_r0_2 != NULL) {
                FSi_ExecuteAsyncCommand(temp_r0_2);
            }
            result = true;
        }
    }
    return result;
}

static FSArchive *FS_FindArchive(const char *name, u32 length) {
    OSIntrMode irq;
    BOOL var_r0;
    char *archiveName;
    FSArchive *archive;

    irq = OS_DisableInterrupts();
    for (archive = FS_data_0003.unk_00; archive != NULL; archive = archive->next) {
        if (!!(archive->flags & FS_ARCHIVE_FLAG_0x2) == false) {
            continue;
        }
        archiveName = FSi_GetPackedName(archive);
        if (WM_func_0009(archiveName, name, length) == 0 && archiveName[length] == '\0') {
            break;
        }
    }
    OS_RestoreInterrupts(irq);
    return archive;
}

static BOOL FS_func_0044(char *arg0) {
    char sp5C[FS_MAX_PATH];
    FSFile file;
    FSiCmd4 sp4;
    u32 sp0;
    BOOL result;
    FSArchive *archive;

    result  = false;
    sp0     = 0;
    archive = FS_func_0046(arg0, &sp0, &sp5C);
    if (archive != NULL) {
        FS_data_0003.unk_04 = archive;
        FS_data_0003.unk_08 = 0;
        FS_data_0003.unk_0a = 0;
        FS_data_0003.unk_0c = 0;
        WM_func_0007(&FS_data_0004, &sp5C, sizeof(FS_data_0004));
        if (archive->fns->unk_10 != NULL) {
            FS_InitFile(&file);
            file.archive  = archive;
            file.cmd.cmd4 = &sp4;
            sp4.unk_00    = sp0;
            sp4.path      = sp5C;
            sp4.unk_0c    = 1;
            if (FSi_SendCommand(&file, FS_CMD_4, true) != FS_RESULT_SUCCESS) {
                FS_data_0003.unk_08 = sp4.unk_08;
                WM_func_0007(&FS_data_0004, &sp5C, sizeof(FS_data_0004));
            }
        }
        result = true;
    }
    return result;
}

static s32 FS_func_0045(char *dst, s32 dstLen, const char *src, s32 srcLen, BOOL *arg4) {
    s32 i;
    s32 len;
    s8 temp_r1;

    len = dstLen - 1;
    if (len >= srcLen) {
        len = srcLen;
    }
    for (i = 0; i < len && src[i] != '\0'; ++i) {
        dst[i] = src[i];
    }
    if (i < srcLen && src[i] != '\0') {
        *arg4 = true;
    }
    dst[i] = '\0';
    return i;
}

static FSArchive *FS_func_0046(const char *path, u32 *arg1, char (*arg2)[FS_MAX_PATH]) {
    struct {
        FSArchive *unk_00;
        BOOL unk_04;
    } sp4;
    s32 temp_r0_3 = 0;
    BOOL wide;
    BOOL slash;
    s32 var_r6;
    s32 i;
    FSArchive *archive;
    const char *pathIter;
    u32 ch;
    u8 firstCh;
    u8 c;
    u8 temp_r1;

    var_r6     = 0;
    pathIter   = path;
    sp4.unk_04 = 0;
    if (FS_data_0003.unk_04 == NULL) {
        FS_data_0003.unk_04 = FS_data_0003.unk_00;
        FS_data_0003.unk_08 = 0U;
        FS_data_0003.unk_0c = 0;
        FS_data_0003.unk_0a = 0;
        FS_data_0004[0]     = '\0';
    }
    firstCh = *pathIter;
    if ((firstCh == '/') || (firstCh == '\\')) {
        pathIter += 1;
        sp4.unk_00 = FS_data_0003.unk_04;
        if (arg1 != NULL) {
            *arg1 = 0;
        }
    } else {
        i = 0;
        while (true) {
            c = pathIter[i];
            if (c == '\0' || c == '/' || c == '\\') {
                sp4.unk_00 = FS_data_0003.unk_04;
                if (arg1 != NULL) {
                    *arg1 = FS_data_0003.unk_08;
                }
                if (arg2 != NULL && FS_data_0003.unk_08 == 0 && FS_data_0004[0] != '\0') {
                    temp_r0_3 += FS_func_0045(*arg2, sizeof(*arg2), FS_data_0004, sizeof(FS_data_0004), &sp4.unk_04);
                    var_r6 =
                        temp_r0_3 + FS_func_0045(*arg2 + temp_r0_3, sizeof(*arg2) - temp_r0_3, "/", 1, &sp4.unk_04);
                }
                break;
            } else if (c == ':') {
                archive = FS_FindArchive(pathIter, i);
                pathIter += i + 1;
                temp_r1    = *pathIter;
                sp4.unk_00 = archive;
                if ((temp_r1 == '/') || (temp_r1 == '\\')) {
                    pathIter += 1;
                }
                if (arg1 != NULL) {
                    *arg1 = 0;
                }
                break;
            }

            wide = true;
            if (!FSi_CHAR_IS_WIDE(c)) {
                wide = false;
            }
            i = i + 1 + wide;
        }
    }
    if (arg2 != NULL) {
        i = 0;
        while (sp4.unk_04 == 0) {
            ch = pathIter[i];
            if (ch != '\0') {
                slash = true;
                if ((u8) ch != '/' && (u8) ch != '\\') {
                    slash = false;
                }
                if (!slash) {
                    wide = false;
                    if (FSi_CHAR_IS_WIDE(ch) && (pathIter[i + 1] != '\x7f') &&
                        ((u32) (u8) (pathIter[i + 1] - 0x40) <= 0xBCU)) {
                        wide = true;
                    }
                    i += wide ? 2 : 1;
                    continue;
                }
            }
            switch (i) {
                case 0:
                    break;
                case 1:
                    if (*pathIter == '.') {
                        break;
                    }
                    // fallthrough
                default:
                    if ((i == 2) && (pathIter[0] == '.') && (pathIter[1] == '.')) {
                        if (var_r6 > 0) {
                            var_r6 -= 1;
                        }
                        var_r6 = FS_func_0053(*arg2, var_r6) + 1;
                    } else {
                        var_r6 += FS_func_0045(*arg2 + var_r6, sizeof(*arg2) - var_r6, pathIter, i, &sp4.unk_04);
                        if (ch != 0) {
                            var_r6 += FS_func_0045(*arg2 + var_r6, sizeof(*arg2) - var_r6, "/", 1, &sp4.unk_04);
                        }
                    }
                    break;
            }
            if (ch == 0) {
                break;
            }
            pathIter += i + 1;
            i = 0;
        }
        (*arg2)[var_r6] = 0;
        FS_func_0054(*arg2);
    }
    if (sp4.unk_04 != 0) {
        sp4.unk_00 = NULL;
    }
    return sp4.unk_00;
}

static void FS_InitArchive(FSArchive *archive) {
    MI_CpuFill8(archive, 0, sizeof(*archive));
    archive->unk_0c.tail = NULL;
    archive->unk_0c.head = NULL;
}

static s32 FS_RegisterArchiveName(FSArchive *archive, const char *name, u32 length) {
    OSIntrMode irq;
    BOOL result;
    s32 var_r5_2;
    FSArchive **var_r1;
    char (*temp_r5)[0x10];
    FSArchive *var_r0;

    result = false;
    irq    = OS_DisableInterrupts();
    if (FS_FindArchive(name, length) == NULL) {
        var_r1 = &FS_data_0003.unk_00;
        var_r0 = FS_data_0003.unk_00;
        if (var_r0 != NULL) {
            do {
                var_r1 = &var_r0->next;
                var_r0 = var_r0->next;
            } while (var_r0 != NULL);
        }
        *var_r1 = archive;
        if (length <= 3) {
            archive->pName = NULL;
            WM_func_0007(archive->name, name, length + 1);
        } else if (length <= 15) {
            for (var_r5_2 = 0;; var_r5_2++) {
                if (var_r5_2 >= 16) {
                    OS_Panic();
                    continue;
                }

                if (sLongArchiveNames[var_r5_2][0] == '\0') {
                    temp_r5 = &sLongArchiveNames[var_r5_2];
                    WM_func_0007(temp_r5, name, length + 1);
                    archive->pName = *temp_r5;
                    break;
                }
            }

        } else {
            OS_Panic();
        }
        result = true;
        archive->flags |= FS_ARCHIVE_FLAG_0x1;
    }
    OS_RestoreInterrupts(irq);
    return result;
}

static char *FSi_GetPackedName(FSArchive *archive) {
    char *name;

    name = archive->name;
    if (name[3] != '\0') {
        name = archive->pName;
    }
    return name;
}

static BOOL FS_func_0049(FSArchive *archive, FS_UnkStruct7 *arg1, const FSArchiveFns *fns) {
    FSFile file;

    archive->unk_20 = arg1;
    archive->fns    = fns;
    FS_InitFile(&file);
    file.archive = archive;
    FSi_TranslateCommand(&file, FS_CMD_17);
    archive->flags |= FS_ARCHIVE_FLAG_0x2;
    return true;
}

static void FS_NotifyArchiveAsyncEnd(FSArchive *archive, FSResult result) {
    FSFile *file;
    OSIntrMode irq;
    FSFile *cmd;

    file = archive->currentFile;
    if (file->flags & FS_FILE_FLAG_AWAIT_SYNC) {
        irq = OS_DisableInterrupts();
        file->flags |= FS_FILE_FLAG_0x8;
        file->unk_14 = result;
        OS_WakeupThread(&file->unk_18);
        OS_RestoreInterrupts(irq);
        return;
    }
    FSi_ReleaseCommand(file, result);
    cmd = FSi_NextCommand(archive, true);
    if (cmd != NULL) {
        FSi_ExecuteAsyncCommand(cmd);
    }
}

static s32 FS_func_0051(char *path, s32 arg1) {
    s32 var_r3;

    var_r3 = arg1 - 1;
    while ((var_r3 > 0) && FSi_CHAR_IS_WIDE(path[var_r3 - 1])) {
        var_r3 -= 1;
    }
    arg1 -= 1;
    return arg1 - ((arg1 - var_r3) & 1);
}

static s32 FS_func_0052(char *path, s32 start) {
    s32 index;
    BOOL wide;
    u32 temp_lr;

    index = start;
    while ((temp_lr = path[index]) != '\0' && (u8) temp_lr != '/' && (u8) temp_lr != '\\') {
        if (FSi_CHAR_IS_WIDE(temp_lr)) {
            wide = true;
        } else {
            wide = false;
        }
        index = index + 1 + wide;
    }
    return index;
}

static s32 FS_func_0053(char *path, s32 arg1) {
    char ch;

    while (true) {
        arg1 = FS_func_0051(path, arg1);
        if (arg1 < 0) {
            break;
        }
        ch = path[arg1];
        if (ch == '/' || ch == '\\') {
            break;
        }
    }
    return arg1;
}

static s32 FS_func_0054(char *path) {
    s32 temp_r0;
    s32 var_r4;
    char ch;

    var_r4  = WM_func_0008();
    temp_r0 = FS_func_0051(path, var_r4);
    if (temp_r0 >= 0) {
        ch = path[temp_r0];
        if (ch == '/' || ch == '\\') {
            var_r4        = temp_r0;
            path[temp_r0] = '\0';
        }
    }
    return var_r4;
}

void FS_InitFile(FSFile *file) {
    file->archive     = NULL;
    file->cursor      = NULL;
    file->next        = NULL;
    file->unk_18.tail = NULL;
    file->unk_18.head = NULL;
    file->flags       = file->unk_14 | FS_CMD_FILE_FLAG(FS_CMD_COUNT);
    file->cmd.ptr     = NULL;
    file->unk_14      = 0;
}

static void FS_func_0055(FSFile *file, FSArchive *archive, s32 arg2, s32 arg3, s32 arg4) {
    FSiCmd7 cmd;

    file->archive  = archive;
    file->cmd.cmd7 = &cmd;
    cmd.unk_04     = arg2;
    cmd.unk_00     = arg4;
    cmd.unk_08     = arg3;
    cmd.unk_0c     = 0;
    FSi_SendCommand(file, FS_CMD_7, true);
}

BOOL FS_OpenFileEx(FSFile *file, const char *path, u32 flags) {
    char sp10[FS_MAX_PATH];
    FSiCmdOpenFile cmd;
    u32 sp0;
    BOOL result;
    FSArchive *archive;

    result  = false;
    sp0     = 0;
    archive = FS_func_0046(path, &sp0, &sp10);
    if (archive != NULL) {
        FS_InitFile(file);
        file->cmd.openFile = &cmd;
        file->archive      = archive;
        cmd.unk_00         = sp0;
        cmd.path           = sp10;
        cmd.flags          = flags;
        if (FSi_SendCommand(file, FS_CMD_OPEN_FILE, true) != 0) {
            result = true;
        } else {
            file->archive = NULL;
        }
    }
    return result;
}

BOOL FS_CloseFile(FSFile *file) {
    return FSi_SendCommand(file, FS_CMD_CLOSE_FILE, true);
}

static u32 FSi_GetLength(FSFile *file) {
    FSiCmdGetLength sp4;
    FSiCmdGetLength sp0;

    sp4.length = 0;
    if (!FSi_GetLengthFromRom(file, &sp4)) {
        file->cmd.getLength = &sp0;
        sp0.length          = 0;
        if (FSi_SendCommand(file, FS_CMD_GET_LENGTH, true)) {
            sp4.length = sp0.length;
        }
    }
    return sp4.length;
}

static s32 FSi_GetPosition(FSFile *file) {
    FSiCmdGetPosition sp4;
    FSiCmdGetPosition sp0;

    sp4.pos = 0;
    if (!FSi_GetPositionFromRom(file, &sp4)) {
        file->cmd.getPosition = &sp0;
        sp0.pos               = 0;
        if (FSi_SendCommand(file, FS_CMD_GET_POSITION, true)) {
            sp4.pos = sp0.pos;
        }
    }
    return sp4.pos;
}

BOOL FS_SeekFile(FSFile *file, s32 pos, u32 mode) {
    BOOL var_r0;
    FS_UnkStruct14 arg1;
    FSiCmdSeekFile cmd;

    arg1.pos = pos;
    var_r0   = FSi_SeekFileFromRom(file, arg1, mode);
    if (var_r0) {
        return var_r0;
    }
    file->cmd.seekFile = &cmd;
    cmd.pos            = pos;
    cmd.mode           = mode;
    return FSi_SendCommand(file, FS_CMD_SEEK_FILE, true);
}

u32 FS_ReadFile(FSFile *file, void *buf, u32 size) {
    FSiCmdReadFile cmd;
    s32 bytesRead;

    file->cmd.readFile = &cmd;
    cmd.buf            = buf;
    cmd.size           = size;
    if (FSi_SendCommand(file, FS_CMD_READ_FILE, true)) {
        bytesRead = cmd.size;
    } else {
        bytesRead = -1;
        if ((u32) (file->unk_14 - 5) > 1U) {
            bytesRead = cmd.size;
        }
    }
    return bytesRead;
}

static FSResult FS_func_0057(FSFile *file, const char *path, s32 arg2) {
    char sp10[FS_MAX_PATH];
    FSiCmd32 sp4;
    u32 sp0;
    FSResult result;
    FSArchive *archive;

    result  = FS_RESULT_SUCCESS;
    sp0     = 0;
    archive = FS_func_0046(path, &sp0, &sp10);
    if (archive != NULL) {
        FS_InitFile(file);
        file->cmd.cmd32 = &sp4;
        file->archive   = archive;
        sp4.unk_00      = sp0;
        sp4.path        = sp10;
        sp4.unk_08      = arg2;
        if (FSi_SendCommand(file, FS_CMD_OPEN_DIR, true) != FS_RESULT_SUCCESS) {
            result = FS_RESULT_FAILURE;
        } else {
            file->archive = NULL;
        }
    }
    return result;
}

FSResult FS_CloseDirectory(FSFile *file) {
    FSResult result;

    result = FS_RESULT_SUCCESS;
    if (FSi_SendCommand(file, FS_CMD_CLOSE_DIR, true) != FS_RESULT_SUCCESS) {
        result = FS_RESULT_FAILURE;
    }
    return result;
}

static FSResult FS_func_0058(FSFile *file, FS_UnkStruct13 *arg1) {
    FSiCmd3 sp0;
    FSResult result;

    result         = FS_RESULT_SUCCESS;
    file->cmd.cmd3 = &sp0;
    sp0.unk_00     = arg1;
    MI_CpuFill8(&arg1->unk_00, 0, sizeof(arg1->unk_00));
    arg1->unk_16c = -1;
    if (FSi_SendCommand(file, FS_CMD_3, true) != FS_RESULT_SUCCESS) {
        result = FS_RESULT_FAILURE;
    }
    return result;
}

static void FS_func_0059(FSDirEntry *dir, FSArchive *archive, FS_UnkStruct13 *arg2) {
    u32 temp_r2_2;

    dir->unk_10 = arg2->unk_118;
    if (dir->unk_10 > sizeof(dir->unk_14) - 1) {
        dir->unk_10 = sizeof(dir->unk_14) - 1;
    }
    MI_CpuCopy8(&arg2->unk_14, &dir->unk_14, dir->unk_10);
    temp_r2_2                = arg2->unk_16c;
    dir->unk_14[dir->unk_10] = '\0';
    if (temp_r2_2 == -1) {
        dir->unk_0c  = 0;
        dir->unk_04u = -1;
        dir->archive = 0;
    } else if (!(arg2->unk_11c & 0x100)) {
        dir->unk_0c  = 0;
        dir->unk_04u = temp_r2_2;
        dir->archive = archive;
    } else {
        dir->unk_0c  = 1;
        dir->archive = archive;
        dir->unk_04s = (s16) temp_r2_2;
        dir->unk_06s = (s16) (temp_r2_2 >> 0x10);
        dir->unk_08  = 0;
    }
}

u32 FS_GetLength(FSFile *file) {
    return FSi_GetLength(file);
}

FSResult FS_FindDir(FSFile *file, const char *path) {
    return FS_func_0057(file, path, 1);
}

FSResult FS_ReadDir(FSFile *file, FSDirEntry *dir) {
    FSResult result;
    FS_UnkStruct13 sp0;

    result = FS_RESULT_SUCCESS;
    if (FS_func_0058(file, &sp0) != FS_RESULT_SUCCESS) {
        FS_func_0059(dir, file->archive, &sp0);
        result = FS_RESULT_FAILURE;
    }
    return result;
}

static void FS_func_0060(void) {
    if (OS_func_0159() != 2) {
        FS_data_0002.unk_04 = 0;
        FS_data_0002.unk_08 = 0;
        FS_data_0002.unk_0c = 0;
    } else {
        FS_data_0002.unk_04 = -1;
        FS_data_0002.unk_08 = 0;
        FS_data_0002.unk_0c = -1;
    }
    FS_data_0002.unk_10  = 0;
    FS_data_0002.unk_14  = FS_data_0001;
    FS_data_0002.unk_18  = sizeof(FS_data_0001);
    FS_data_0002.archive = FS_FindArchive("rom", 3);
}

static FSResult FS_func_0003(FS_UnkStruct11 *arg0, void *dst, s32 size) {
    FSResult result;
    FS_UnkStruct7 *temp_r3;
    FSArchive *archive;

    archive = arg0->archive;
    temp_r3 = archive->unk_20;
    if (temp_r3->unk_1c != 0) {
        MI_CpuCopy8(arg0->src, dst, size);
        result = FS_RESULT_SUCCESS;
    } else {
        result = temp_r3->read(archive, dst, arg0->src, size);
        result = FS_func_0037(archive->currentFile, result);
    }
    arg0->src += size;
    return result;
}

static void FS_func_0062(FSFile *file, u32 arg1) {
    file->unk_30.unk_04s = arg1;
    file->unk_30.unk_00p = file->archive;
    file->unk_30.unk_06s = 0;
    file->unk_30.unk_08u = 0;
    FSi_ExecuteFileProc(file, FS_FILE_PROC_2, true);
}

static FSResult FS_func_0007(FSFile *file) {
    FS_UnkStruct10 sp8;
    FS_UnkStruct11 sp0;
    FSResult result;
    FS_UnkStruct7 *temp_r4;
    FS_UnkStruct8 *temp_r5;

    temp_r5     = &file->unk_30;
    temp_r4     = file->archive->unk_20;
    sp0.archive = file->archive;
    sp0.src     = (void *) temp_r4->unk_0c + (temp_r5->unk_04u * 8);
    result      = FS_func_0003(&sp0, &sp8, 8);
    if (result == FS_RESULT_SUCCESS) {
        file->unk_20 = *temp_r5;
        if ((temp_r5->unk_06s == 0) && (temp_r5->unk_08p == NULL)) {
            file->unk_20.unk_06s = sp8.unk_04s;
            file->unk_20.unk_08p = (void *) (temp_r4->unk_0c + (u32) sp8.dst);
        }
        file->unk_2cu = (s32) (sp8.unk_06s & 0xFFF);
    }
    return result;
}

static FSResult FS_func_0008(FSFile *file) {
    FS_UnkStruct11 sp4;
    FS_UnkStruct10 sp0;
    s32 temp_r2;
    FSResult result;
    FS_UnkStruct17 *temp_r5;

    temp_r5     = file->unk_30.unk_00p;
    sp4.archive = file->archive;
    sp4.src     = file->unk_20.unk_08p;
    result      = FS_func_0003(&sp4, &sp0, 1);
    if (result != FS_RESULT_SUCCESS) {
        return result;
    }
    temp_r2         = sp0.unk_00u & 0x7F;
    temp_r5->unk_10 = temp_r2;
    temp_r5->unk_0c = (s32) (((s32) sp0.unk_00u >> 7) & 1);
    if (temp_r2 == 0) {
        return FS_RESULT_FAILURE;
    }
    if (file->unk_30.unk_04p == NULL) {
        result = FS_func_0003(&sp4, &temp_r5->unk_14, 1);
        if (result != 0) {
            return result;
        }
        temp_r5->unk_14[temp_r5->unk_10] = 0;
    } else {
        sp4.src += temp_r2;
    }
    if (temp_r5->unk_0c == 0) {
        temp_r5->archive = file->archive;
        temp_r5->unk_04u = file->unk_20.unk_06s;
        file->unk_20.unk_06s++;
    } else {
        result = FS_func_0003(&sp4, &sp0.unk_02s, 2);
        if (result == 0) {
            temp_r5->archive = file->archive;
            temp_r5->unk_04s = sp0.unk_02s & 0xFFF;
            temp_r5->unk_06s = 0;
            temp_r5->unk_08  = 0;
        }
    }
    if (result == 0) {
        file->unk_20.unk_08p = sp4.src;
    }
    return result;
}

static FSResult FS_func_0009(FSFile *file) {
    char ch;
    u32 uVar2;
    int iVar4;
    u32 uVar5;
    FSResult result;
    u32 uVar7;
    u32 uVar8;
    u32 uVar9;
    int iVar10;
    FS_UnkStruct8 *puVar11;
    char *path;
    u32 uVar13;
    FS_UnkStruct8 uStack_bc;
    u32 uStack_b0;
    u32 uStack_ac;
    char acStack_a8[128];
    u32 uStack_28;

    iVar10 = file->unk_40;
    path   = file->unk_3c;
    FSi_ExecuteFileProc(file, FS_FILE_PROC_2, true);
    ch = *path;
    while (ch != '\0') {
        uVar2  = FS_func_0052(path, 0);
        uVar13 = path[uVar2] != 0 || iVar10 != 0;
        if (uVar2 == 0) {
            return FS_RESULT_INVALID_PARAM;
        }
        if (*path == '.') {
            if (uVar2 == 1) {
                path = path + 1;
            } else {
                if (uVar2 != 2 || path[1] != '.') {
                    goto LAB_020255f4;
                } else if (file->unk_20.unk_04s != 0) {
                    FS_func_0062(file, file->unk_2cu);
                }
                path = path + 2;
            }
        } else {
            if (*path == '*') {
                break;
            }
        LAB_020255f4:
            if (0x7f < (int) uVar2) {
                return FS_RESULT_0xB;
            }
            file->unk_30.unk_00p = &uStack_bc;
            file->unk_30.unk_04u = 0;
            do {
                do {
                    iVar4 = FSi_ExecuteFileProc(file, FS_FILE_PROC_3, true);
                    if (iVar4 != 0) {
                        return FS_RESULT_0xB;
                    }
                    uVar5 = uStack_b0;
                    if (uVar13 == uStack_b0) {
                        uVar5 = uStack_ac;
                    }
                } while (uVar13 != uStack_b0 || uVar2 != uVar5);
                iVar4 = 0;
                for (uVar5 = 0; uVar5 < uVar2; ++uVar5) {
                    uVar8 = (u8) path[uVar5] - 'A';
                    uVar7 = (u8) acStack_a8[uVar5] - 'A';
                    if (uVar8 <= 25) {
                        uVar8 = uVar8 + 0x20;
                    }
                    if (uVar7 <= 25) {
                        uVar7 = uVar7 + 0x20;
                    }
                    iVar4 = uVar8 - uVar7;
                    if (iVar4 != 0) {
                        break;
                    }
                }
            } while (iVar4 != 0);
            if (uVar13 != 0) {
                file->unk_30 = uStack_bc;
                path         = path + uVar2;
                FSi_ExecuteFileProc(file, FS_FILE_PROC_2, true);
            } else if (iVar10 != 0) {
                return FS_RESULT_0xB;
            } else {
                puVar11          = file->unk_44;
                puVar11->unk_00p = uStack_bc.unk_00p;
                puVar11->unk_04u = uStack_bc.unk_04u;
                return FS_RESULT_SUCCESS;
            }
        }
        path = path + (*path != '\0');
        ch   = *path;
    }
    if (iVar10 == 0) {
        result = FS_RESULT_0xB;
    } else {
        *file->unk_44 = file->unk_20;
        result        = FS_RESULT_SUCCESS;
    }
    return result;
}

static FSResult FS_func_0010(FSFile *file) {
    FS_UnkStruct17 sp4C;
    FSFile sp4;
    FSArchive *archive;
    s32 var_r0;
    s32 var_r10_2;
    s32 var_r4;
    s32 var_r6;
    char *temp_r11;
    s8 *temp_r7;
    u32 temp_r6;
    u32 var_r5;
    u32 var_r9;
    u32 var_r9_2;
    u32 temp_r0;
    u32 temp_r4;
    u32 var_r10;

    archive = file->archive;
    FS_InitFile(&sp4);
    sp4.archive = archive;
    if (file->flags & FS_FILE_FLAG_DIR) {
        var_r0 = 1;
    } else {
        var_r0 = 0;
    }
    if (var_r0 != 0) {
        var_r5 = file->unk_20.unk_04s;
        var_r4 = 0x10000;
    } else {
        var_r9  = 0;
        var_r4  = file->unk_20.unk_00u;
        var_r10 = 0;
        var_r5  = 0x10000;
        do {
            FS_func_0062(&sp4, var_r9);
            sp4.unk_30.unk_00p = &sp4C;
            if (var_r9 == 0) {
                var_r10 = sp4.unk_2cu;
            }
            sp4.unk_30.unk_04u = 1;
            while (FSi_ExecuteFileProc(&sp4, FS_FILE_PROC_3, true) == FS_RESULT_SUCCESS) {
                if ((sp4C.unk_0c == 0) && (sp4C.unk_04u == var_r4)) {
                    var_r5 = sp4.unk_20.unk_04s;
                    break;
                }
            }
            if (var_r5 != 0x10000) {
                break;
            }
            var_r9 += 1;
        } while ((u32) var_r9 < var_r10);
    }
    if (var_r5 == 0x10000) {
        file->unk_30.unk_08s = 0U;
        return 0xB;
    }
    var_r9_2 = var_r5;
    FSi_GetPackedName(archive);
    var_r10_2 = WM_func_0008() + 2;
    FS_func_0062(&sp4, var_r5);
    if (var_r4 != 0x10000) {
        var_r10_2 += sp4C.unk_10;
    }
    if (var_r5 != 0) {
        do {
            FS_func_0062(&sp4, sp4.unk_2cu);
            sp4.unk_30.unk_00p = &sp4C;
            sp4.unk_30.unk_04u = 1;
            while (FSi_ExecuteFileProc(&sp4, FS_FILE_PROC_3, true) == FS_RESULT_SUCCESS) {
                if ((sp4C.unk_0c != 0) && (sp4C.unk_04s == var_r9_2)) {
                    var_r10_2 += sp4C.unk_10 + 1;
                    break;
                }
            }
            var_r9_2 = sp4.unk_20.unk_04s;
        } while (var_r9_2 != 0);
    }
    temp_r7              = file->unk_30.unk_00p;
    file->unk_30.unk_08s = var_r10_2 + 1;
    file->unk_30.unk_0as = var_r5;
    if (temp_r7 != NULL) {
        temp_r6 = file->unk_30.unk_08u;
        if (file->unk_30.unk_04s >= temp_r6) {
            temp_r11 = FSi_GetPackedName(archive);
            temp_r0  = (u32) WM_func_0008();
            MI_CpuCopy8(temp_r11, temp_r7, temp_r0);
            MI_CpuCopy8(":/", &temp_r7[temp_r0], 2);
            FS_func_0062(&sp4, var_r5);
            if (var_r4 != 0x10000) {
                sp4.unk_30.unk_00p = &sp4C;
                sp4.unk_30.unk_04u = 0;
                while (FSi_ExecuteFileProc(&sp4, FS_FILE_PROC_3, true) == FS_RESULT_SUCCESS) {
                    if ((sp4C.unk_0c == 0) && (sp4C.unk_04u == var_r4)) {
                        break;
                    }
                }
                temp_r4 = sp4C.unk_10 + 1;
                MI_CpuCopy8(&sp4C.unk_14, &temp_r7[temp_r6] - temp_r4, temp_r4);
                var_r6 = temp_r6 - temp_r4;
            } else {
                temp_r7[temp_r6 - 1] = '\0';
                var_r6               = temp_r6 - 1;
            }
            if (var_r5 != 0) {
                do {
                    FS_func_0062(&sp4, sp4.unk_2cu);
                    sp4.unk_30.unk_00p  = &sp4C;
                    sp4.unk_30.unk_04u  = 0;
                    temp_r7[var_r6 - 1] = '/';
                    var_r6 -= 1;
                    while (FSi_ExecuteFileProc(&sp4, FS_FILE_PROC_3, true) == FS_RESULT_SUCCESS) {
                        if ((sp4C.unk_0c != 0) && (sp4C.unk_04s == var_r5)) {
                            MI_CpuCopy8(&sp4C.unk_14, &temp_r7[var_r6] - sp4C.unk_10, sp4C.unk_10);
                            var_r6 -= sp4C.unk_10;
                            break;
                        }
                    }
                    var_r5 = sp4.unk_20.unk_04u;
                } while (var_r5 != 0);
            }
        }
    }
    return FS_RESULT_SUCCESS;
}

static FSResult FS_func_0011(FSFile *file) {
    FS_UnkStruct10 sp8;
    FS_UnkStruct11 sp0;
    s32 temp_r0;
    s32 temp_r2;
    s32 temp_r3;
    s32 temp_r4;
    FS_UnkStruct7 *temp_r1;
    FSArchive *archive;

    archive = file->archive;
    temp_r4 = file->unk_30.unk_04u;
    temp_r2 = temp_r4 * 8;
    temp_r1 = archive->unk_20;
    if (temp_r1->unk_08 <= temp_r2) {
        return FS_RESULT_0xB;
    }
    sp0.archive = archive;
    sp0.src     = (void *) (temp_r1->unk_04 + temp_r2);
    temp_r0     = FS_func_0003(&sp0, &sp8, 8);
    if (temp_r0 != 0) {
        return temp_r0;
    }
    file->unk_30.unk_00p = sp8.dst;
    file->unk_30.unk_04u = sp8.unk_04u;
    file->unk_30.unk_08u = temp_r4;
    return FSi_ExecuteFileProc(file, FS_FILE_PROC_7, true);
}

static FSResult FS_func_0012(FSFile *file) {
    file->unk_20.unk_04u = file->unk_30.unk_00u;
    file->unk_2cu        = file->unk_30.unk_00u;
    file->unk_20.unk_08u = file->unk_30.unk_04u;
    file->unk_20.unk_00u = file->unk_30.unk_08u;
    return FS_RESULT_SUCCESS;
}

static FSResult FSi_FileProcRead(FSFile *file) {
    FSArchive *archive;
    FS_UnkStruct7 *temp_ip;
    void *temp_r2;

    archive = file->archive;
    temp_ip = archive->unk_20;
    temp_r2 = file->unk_2cp;
    file->unk_2cp += file->unk_30.unk_08u;

    return temp_ip->read(archive, file->unk_30.unk_00p, temp_r2, file->unk_30.unk_08u);
}

static FSResult FSi_FileProcWrite(FSFile *file) {
    FSArchive *archive;
    FS_UnkStruct7 *temp_ip;
    void *temp_r2;

    archive = file->archive;
    temp_ip = archive->unk_20;
    temp_r2 = file->unk_2cp;
    file->unk_2cp += file->unk_30.unk_08u;

    return temp_ip->write(archive, file->unk_30.unk_00p, temp_r2, file->unk_30.unk_08u);
}

static FSResult FSi_FileProcNop(FSFile *file) {
    return FS_RESULT_SUCCESS;
}

static FSResult FSi_ExecuteFileProc(FSFile *file, FSFileProc proc, BOOL wait) {
    FSResult result;
    FS_UnkStruct7 *temp_r6;

    temp_r6 = file->archive->unk_20;
    result  = FS_RESULT_0x101;
    if (temp_r6->overrideMask & (1 << proc)) {
        result = temp_r6->proc(file, proc);
        switch (result) {
            case FS_RESULT_SUCCESS:
            case FS_RESULT_FAILURE:
            case FS_RESULT_INVALID_COMMAND:
            case FS_RESULT_AWAIT_ASYNC:
                break;
            case FS_RESULT_0x102:
                result = FS_RESULT_0x101;
                temp_r6->overrideMask &= ~(1 << proc);
                break;
        }
    }
    if (result == FS_RESULT_0x101) {
        if (proc >= FS_FILE_PROC_COUNT) {
            result = FS_RESULT_INVALID_COMMAND;
        } else {
            result = sFileProcs[proc](file);
        }
    }
    if (wait) {
        result = FS_func_0037(file, result);
    }
    return result;
}

static FSResult FSi_RomReadFile(FSArchive *archive, FSFile *file, u32 size, FSiCmdReadFile *cmd) {
    void *buf;
    u32 remaining;
    FS_UnkStruct6 *temp_r0;

    temp_r0   = file->cursor;
    buf       = cmd->buf;
    remaining = temp_r0->end - temp_r0->pos;
    if ((u32) buf > remaining) {
        cmd->buf = (void *) remaining;
    }
    file->unk_30.unk_04p = buf;
    file->unk_30.unk_00u = size;
    file->unk_30.unk_08p = cmd->buf;
    return FSi_ExecuteFileProc(file, FS_FILE_PROC_READ, false);
}

static FSResult FSi_RomWriteFile(FSArchive *archive, FSFile *file, u32 arg2, FSiCmd1 *cmd) {
    u32 temp_lr;
    u32 temp_r0_2;
    FS_UnkStruct6 *temp_r0;

    temp_r0   = file->cursor;
    temp_lr   = cmd->unk_00;
    temp_r0_2 = (u32) (temp_r0->end - temp_r0->pos);
    if (temp_lr > temp_r0_2) {
        cmd->unk_00 = temp_r0_2;
    }
    file->unk_30.unk_04u = temp_lr;
    file->unk_30.unk_00u = arg2;
    file->unk_30.unk_08u = cmd->unk_00;
    return FSi_ExecuteFileProc(file, FS_FILE_PROC_WRITE, 0);
}

static FSResult FS_func_0066(FSArchive *archive, FSFile *file, u32 arg2, u32 arg3) {
    FSResult result;

    file->unk_30.unk_04s = arg2;
    file->archive        = archive;
    file->unk_30.unk_00p = archive;
    file->unk_30.unk_06s = (s16) (arg2 >> 0x10);
    file->unk_30.unk_08u = arg3;
    result               = FSi_ExecuteFileProc(file, FS_FILE_PROC_2, true);
    if (result != FS_RESULT_SUCCESS) {
        return result;
    }
    file->flags   = ((file->flags | FS_FILE_FLAG_DIR) & ~FS_FILE_FLAG_FILE);
    file->cursor  = &file->unk_20;
    file->archive = archive;
    return result;
}

static FSResult FS_func_0067(FSArchive *archive, FSFile *file, FS_UnkStruct13 *arg2) {
    FS_UnkStruct17 sp10;
    FS_UnkStruct19 sp8;
    FS_UnkStruct11 sp0;
    FSResult result;
    FS_UnkStruct7 *temp_r1;

    file->unk_30.unk_00p = &sp10;
    file->unk_30.unk_04u = 0;
    result               = FSi_ExecuteFileProc(file, FS_FILE_PROC_3, true);
    if (result == FS_RESULT_SUCCESS) {
        arg2->unk_10  = 0;
        arg2->unk_118 = sp10.unk_10;
        MI_CpuCopy8(sp10.unk_14, &arg2->unk_14, sp10.unk_10);
        arg2->unk_14[arg2->unk_118] = 0;
        if (sp10.unk_0c != 0) {
            arg2->unk_11c = 0x100;
            arg2->unk_16c = (s32) (sp10.unk_04s | (sp10.unk_06s << 0x10));
            arg2->unk_168 = 0;
        } else {
            arg2->unk_11c = 0;
            arg2->unk_16c = sp10.unk_04u;
            arg2->unk_168 = 0;
            temp_r1       = archive->unk_20;
            if (temp_r1->unk_08 > sp10.unk_04u * 8) {
                sp0.archive = archive;
                sp0.src     = (void *) (temp_r1->unk_04 + sp10.unk_04u * 8);
                if (FS_func_0003(&sp0, &sp8, sizeof(sp8)) == 0) {
                    arg2->unk_168 = (s32) (sp8.unk_04 - sp8.unk_00);
                    if (FS_func_0095(archive, sp8.unk_00) != 0) {
                        arg2->unk_11c |= 0x400;
                    }
                }
            }
        }
        arg2->unk_138 = 0;
        arg2->unk_13c = 0;
        arg2->unk_140 = 0;
        arg2->unk_144 = 0;
        arg2->unk_148 = 0;
        arg2->unk_14c = 0;
    }
    return result;
}

static FSResult FS_func_0068(FSArchive *archive, u32 arg1, char *path, u32 *arg3, BOOL arg4) {
    FSResult result;
    FSFile file;
    FS_UnkStruct8 sp0;

    FS_InitFile(&file);
    file.unk_30.unk_04s = arg1;
    file.unk_3c         = path;
    file.unk_40         = arg4;
    file.archive        = archive;
    file.unk_30.unk_00p = archive;
    file.unk_30.unk_06s = 0;
    file.unk_30.unk_08u = 0;
    file.unk_44         = &sp0;
    result              = FSi_ExecuteFileProc(&file, FS_FILE_PROC_4, true);
    if (result != 0) {
        return result;
    }
    if (arg4 != 0) {
        *arg3 = sp0.unk_04s;
        return result;
    }
    *arg3 = sp0.unk_04u;
    return result;
}

static FSResult FS_func_0069(FSArchive *archive, FSFile *file, u32 arg2, u32 arg3, u32 *arg4) {
    FSResult result;

    file->unk_30.unk_08s = 0;
    file->unk_30.unk_0as = 0;
    file->unk_30.unk_00u = arg3;
    file->unk_30.unk_04u = *arg4;
    result               = FSi_ExecuteFileProc(file, FS_FILE_PROC_5, true);
    if (result == FS_RESULT_SUCCESS) {
        *arg4 = file->unk_30.unk_04u;
    }
    return result;
}

static FSResult FS_func_0070(FSArchive *archive, FSFile *file, u32 arg2, u32 flags) {
    FSResult result;

    file->unk_30.unk_04u = arg2;
    file->unk_30.unk_00p = archive;
    result               = FSi_ExecuteFileProc(file, FS_FILE_PROC_6, true);
    if (result != FS_RESULT_SUCCESS) {
        return result;
    }
    file->flags   = (file->flags | FS_FILE_FLAG_FILE) & ~FS_FILE_FLAG_DIR;
    file->cursor  = &file->unk_20;
    file->archive = archive;
    return result;
}

static FSResult FS_func_0071(FSArchive *archive, FSFile *file, u32 arg2, u32 arg3, FSiCmd7 *cmd) {
    FSResult result;

    file->unk_30.unk_00u = arg2;
    file->unk_30.unk_04u = arg3;
    file->unk_30.unk_08u = cmd->unk_00;
    result               = FSi_ExecuteFileProc(file, FS_FILE_PROC_7, true);
    if (result != FS_RESULT_SUCCESS) {
        return result;
    }
    file->flags   = ((file->flags | FS_FILE_FLAG_FILE) & ~FS_FILE_FLAG_DIR);
    file->cursor  = &file->unk_20;
    file->archive = archive;
    return result;
}

static FSResult FSi_RomCloseFile(FSArchive *archive, FSFile *file) {
    FSResult result;

    result       = FSi_ExecuteFileProc(file, FS_FILE_PROC_CLOSE, true);
    file->cursor = NULL;
    file->flags &= ~(FS_FILE_FLAG_FILE | FS_FILE_FLAG_DIR);
    return result;
}

static FSResult FSi_RomLock(FSArchive *archive) {
    FSFile file;

    FS_InitFile(&file);
    file.archive = archive;
    return FSi_ExecuteFileProc(&file, FS_FILE_PROC_SUSPEND, false);
}

static FSResult FSi_RomUnlock(FSArchive *archive) {
    FSFile file;

    FS_InitFile(&file);
    file.archive = archive;
    return FSi_ExecuteFileProc(&file, FS_FILE_PROC_UNLOCK, false);
}

static FSResult FS_func_0075(FSArchive *archive) {
    FSFile file;

    FS_InitFile(&file);
    file.archive = archive;
    return FSi_ExecuteFileProc(&file, FS_FILE_PROC_11, false);
}

static FSResult FS_func_0076(FSArchive *archive) {
    FSFile file;

    FS_InitFile(&file);
    file.archive = archive;
    return FSi_ExecuteFileProc(&file, FS_FILE_PROC_12, false);
}

static FSResult FSi_RomOpenFile(FSArchive *archive, FSFile *file, u32 arg2, char *path, u32 flags) {
    FSResult result;
    u32 sp4;

    result = FS_func_0068(archive, arg2, path, &sp4, false);
    if (result != FS_RESULT_SUCCESS) {
        return result;
    }
    return FS_func_0070(archive, file, sp4, flags);
}

static FSResult FSi_RomSeekFile(FSArchive *archive, FSFile *file, s32 *pPos, u32 mode) {
    s32 pos;
    s32 newPos;
    s32 base;
    FS_UnkStruct6 *cursor;

    cursor = file->cursor;
    pos    = *pPos;
    switch (mode) {
        case FS_SEEK_SET:
            base = cursor->start;
            break;
        default:
        case FS_SEEK_CUR:
            base = cursor->pos;
            break;
        case FS_SEEK_END:
            base = cursor->end;
            break;
    }
    newPos = pos + base;
    if ((newPos < cursor->start) || (newPos > cursor->end)) {
        return FS_RESULT_INVALID_PARAM;
    }
    cursor->pos = newPos;
    *pPos       = newPos;
    return FS_RESULT_SUCCESS;
}

static FSResult FSi_RomGetLength(FSArchive *archive, FSFile *file, FSiCmdGetLength *cmd) {
    FS_UnkStruct6 *cursor;

    cursor      = file->cursor;
    cmd->length = cursor->end - cursor->start;
    return FS_RESULT_SUCCESS;
}

static FSResult FSi_RomGetPosition(FSArchive *archive, FSFile *file, FSiCmdGetPosition *cmd) {
    FS_UnkStruct6 *cursor;

    cursor   = file->cursor;
    cmd->pos = cursor->pos - cursor->start;
    return FS_RESULT_SUCCESS;
}

static FSResult FS_func_0080(FSArchive *archive) {
    FS_UnkStruct7 *temp_r4;

    temp_r4 = archive->unk_20;
    FS_func_0093(archive);
    temp_r4->unk_00 = 0;
    temp_r4->unk_04 = 0;
    temp_r4->unk_08 = 0;
    temp_r4->unk_0c = 0;
    temp_r4->unk_10 = 0;
    temp_r4->unk_14 = 0;
    temp_r4->unk_18 = 0;
    return FS_RESULT_SUCCESS;
}

static FSResult FS_func_0081(FSArchive *archive, FSiCmd19 *cmd) {
    cmd->unk_00 = 0;
    return FS_RESULT_SUCCESS;
}

static FSResult FSi_RomOpenDir(FSArchive *archive, FSFile *file, u32 arg2, char *path, u32 arg4) {
    FSResult result;
    u32 sp4;

    sp4    = 0;
    result = FS_func_0068(archive, arg2, path, &sp4, true);
    if (result != FS_RESULT_SUCCESS) {
        return result;
    }
    return FS_func_0066(archive, file, sp4, 0);
}

static FSResult FSi_RomCloseDir(FSArchive *archive, FSFile *file) {
    file->cursor = NULL;
    file->flags &= ~(FS_FILE_FLAG_FILE | FS_FILE_FLAG_DIR);
    return FS_RESULT_SUCCESS;
}

static FSResult FS_func_0084(FSArchive *archive, u32 arg1, char *path, FS_UnkStruct15 *arg3) {
    u32 sp14;
    FS_UnkStruct10 spC;
    FS_UnkStruct11 sp4;
    FSResult result;
    FS_UnkStruct7 *temp_r3;

    sp14   = 0;
    result = 5;
    MI_CpuFill8(arg3, 0, 0x54);
    if (FS_func_0068(archive, arg1, path, &sp14, true) == FS_RESULT_SUCCESS) {
        arg3->unk_00 = 0x100;
        result       = FS_RESULT_SUCCESS;
        arg3->unk_50 = sp14;
    } else if (FS_func_0068(archive, arg1, path, &sp14, false) == FS_RESULT_SUCCESS) {
        arg3->unk_00 = 0;
        arg3->unk_50 = sp14;
        arg3->unk_4c = 0;
        temp_r3      = archive->unk_20;
        if (temp_r3->unk_08 > sp14 * 8) {
            sp4.archive = archive;
            sp4.src     = (void *) (temp_r3->unk_04 + (sp14 * 8));
            if (FS_func_0003(&sp4, &spC, 8) == 0) {
                arg3->unk_4c = spC.unk_04u - spC.unk_00u;
                if (FS_func_0095(archive, spC.unk_00u) != 0) {
                    arg3->unk_00 |= 0x400;
                }
            }
        }
        result = FS_RESULT_SUCCESS;
    }
    arg3->unk_00 |= 0x200;
    return result;
}

static FSResult FS_func_0085(FSArchive *archive, FS_UnkStruct16 *arg1) {
    CARD_UnkStruct2 *temp_r0;

    temp_r0      = CARD_func_0058();
    arg1->unk_20 = 0;
    arg1->unk_24 = 0;
    arg1->unk_28 = 0;
    arg1->unk_2c = 0;
    arg1->unk_00 = temp_r0->unk_80;
    arg1->unk_04 = 0;
    arg1->unk_08 = 0;
    arg1->unk_0c = 0;
    arg1->unk_10 = 0x7FFFFFFF;
    arg1->unk_14 = 0;
    arg1->unk_18 = 0x7FFFFFFF;
    arg1->unk_1c = 0;
    return FS_RESULT_SUCCESS;
}

static FSResult FSi_ReadMemCallback(FSArchive *archive, void *arg1, void *arg2, s32 size) {
    MI_CpuCopy8(FS_func_0092(archive, arg2), arg1, size);
    return FS_RESULT_SUCCESS;
}

static FSResult FSi_WriteMemCallback(FSArchive *archive, void *arg1, void *arg2, s32 size) {
    MI_CpuCopy8(arg1, FS_func_0092(archive, arg2), size);
    return FS_RESULT_SUCCESS;
}

static void FS_LoadArchive(FSArchive *archive, s32 arg1, s32 arg2, s32 arg3, s32 arg4, s32 arg5,
                           FSResult (*read)(FSArchive *, void *, void *, s32),
                           FSResult (*write)(FSArchive *, void *, void *, s32)) {
    if (read == NULL) {
        read = FSi_ReadMemCallback;
    }
    if (write == NULL) {
        write = FSi_WriteMemCallback;
    }
    archive->unk_28.unk_0c = arg4;
    archive->unk_28.unk_18 = arg4;
    archive->unk_28.unk_00 = arg1;
    archive->unk_28.unk_08 = arg3;
    archive->unk_28.unk_04 = arg2;
    archive->unk_28.unk_14 = arg2;
    archive->unk_28.unk_10 = arg5;
    archive->unk_28.read   = read;
    archive->unk_28.write  = write;
    archive->unk_28.unk_1c = 0;
    FS_func_0049(archive, &archive->unk_28, &sArchiveFns);
}

static BOOL FSi_GetLengthFromRom(FSFile *file, FSiCmdGetLength *cmd) {
    BOOL result;
    FSArchive *archive;

    archive = file->archive;
    result  = false;
    if (archive->fns == &sArchiveFns && FSi_RomGetLength(archive, file, cmd) == FS_RESULT_SUCCESS) {
        result = true;
    }
    return result;
}

static BOOL FSi_GetPositionFromRom(FSFile *file, FSiCmdGetPosition *cmd) {
    BOOL result;
    FSArchive *archive;

    archive = file->archive;
    result  = false;
    if (archive->fns == &sArchiveFns && FSi_RomGetPosition(archive, file, cmd) == FS_RESULT_SUCCESS) {
        result = true;
    }
    return result;
}

static BOOL FSi_SeekFileFromRom(FSFile *file, FS_UnkStruct14 arg1, s32 mode) {
    FSResult result;
    FSArchive *archive;

    archive = file->archive;
    if (archive->fns == &sArchiveFns) {
        result                = FSi_RomSeekFile(archive, file, &arg1.pos, mode);
        file->unk_14          = result;
        file->archive->unk_1c = result;
        if (result == FS_RESULT_SUCCESS) {
            return true;
        }
    }
    return false;
}

static void FS_SetArchiveProc(FSArchive *archive, FSResult (*proc)(FSFile *file, FSFileProc index), u32 overrideMask) {
    if (overrideMask == 0) {
        proc = NULL;
    } else if (proc == NULL) {
        overrideMask = 0;
    }
    archive->unk_28.proc         = proc;
    archive->unk_28.overrideMask = overrideMask;
}

static void *FS_func_0092(FSArchive *archive, void *arg1) {
    return arg1 + archive->unk_20->unk_00;
}

static BOOL FS_func_0093(FSArchive *archive) {
    return !!(archive->flags & FS_ARCHIVE_FLAG_0x4);
}

static s32 FS_func_0094(FSFile *file) {
    return file->unk_20.unk_04u;
}

static s32 FS_func_0095(FSArchive *archive, u32 arg1) {
    return 0;
}

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

static BOOL FSi_InitRom(s32 dmaCount) {
    s32 temp_r1;
    s32 temp_r2;
    BOOL var_r0;
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

void FS_Init(u32 dmaCount) {
    if (FSi_Initialized) {
        return;
    }
    FSi_Initialized = true;
    FSi_InitRom(dmaCount);
    FS_func_0060();
}
