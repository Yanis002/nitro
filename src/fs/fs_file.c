#include "nitro/fs.h"
#include "nitro/mi.h"
#include "nitro/os.h"

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

static const u8 FS_data_0001[64] = {
    0x21, 0x06, 0xC0, 0xDE, 0xBA, 0x98, 0xCE, 0x3F, 0xA6, 0x92, 0xE3, 0x9D, 0x46, 0xF2, 0xED, 0x01,
    0x76, 0xE3, 0xCC, 0x08, 0x56, 0x23, 0x63, 0xFA, 0xCA, 0xD4, 0xEC, 0xDF, 0x9A, 0x62, 0x78, 0x34,
    0x8F, 0x6D, 0x63, 0x3C, 0xFE, 0x22, 0xCA, 0x92, 0x20, 0x88, 0x97, 0x23, 0xD2, 0xCF, 0xAE, 0xC2,
    0x32, 0x67, 0x8D, 0xFE, 0xCA, 0x83, 0x64, 0x98, 0xAC, 0xFD, 0x3E, 0x37, 0x87, 0x46, 0x58, 0x24,
};

static FS_UnkStruct18 FS_data_0002;

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

void FS_func_0055(FSFile *file, FSArchive *archive, s32 arg2, s32 arg3, s32 arg4) {
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

s32 FSi_GetPosition(FSFile *file) {
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

void FS_func_0060(void) {
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

FSResult FS_func_0003(FS_UnkStruct11 *arg0, void *dst, s32 size) {
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
