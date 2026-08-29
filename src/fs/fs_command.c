#include "nitro/fs.h"
#include "nitro/fs/fs_archive.h"
#include "nitro/fs/fs_common.h"
#include "nitro/fs/fs_file.h"
#include "nitro/os.h"

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

void FSi_ReleaseCommand(FSFile *file, FSResult result) {
    FSFile *var_r0;
    FSFile **var_r1;
    OSIntrMode irq;
    u8 cmdType;
    FSArchive *archive;
    FSFile *next;
    FSFile *prev;

    irq = OS_DisableInterrupts();

#if NITRO_VERSION >= 0x5057533
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
#else
    next = file->next;
    prev = file->prev;
    if (next != NULL) {
        next->prev = prev;
    }
    if (prev != NULL) {
        prev->next = next;
    }
    file->next = NULL;
    file->prev = NULL;

    file->flags &=
        ~(FS_FILE_FLAG_SEND_CMD | FS_FILE_FLAG_0x2 | FS_FILE_FLAG_AWAIT_SYNC | FS_FILE_FLAG_0x8 | FS_FILE_FLAG_0x40);
    file->unk_14 = result;
#endif

    OS_WakeupThread(&file->unk_18);
    OS_RestoreInterrupts(irq);
}

FSResult FS_func_0037(FSFile *file, FSResult result) {
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

FSResult FSi_TranslateCommand(FSFile *file, u8 cmdType) {
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

FSFile *FSi_NextCommand(FSArchive *archive, BOOL arg1) {
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

void FSi_ExecuteAsyncCommand(FSFile *file) {
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

BOOL FSi_SendCommand(FSFile *file, s32 cmdType, BOOL sync) {
    FSFile *temp_r0_2;
    FSFile **var_r0_2;
    FSFile *var_r1;
    u32 flags;
    OSIntrMode irq;
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
