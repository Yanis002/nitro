#include "nitro/card.h"
#include "nitro/fs.h"
#include "nitro/mi.h"
#include "nitro/wm.h"

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

static FSResult FSi_ExecuteFileProc(FSFile *file, u32 index, BOOL wait);
static FSResult FSi_FileProcRead(FSFile *arg0);
static FSResult FSi_FileProcWrite(FSFile *arg0);
static FSResult FS_func_0007(FSFile *arg0);
static FSResult FS_func_0008(FSFile *arg0);
static FSResult FS_func_0009(FSFile *arg0);
static FSResult FS_func_0010(FSFile *arg0);
static FSResult FS_func_0011(FSFile *arg0);
static FSResult FS_func_0012(FSFile *arg0);
static FSResult FSi_FileProcNop(FSFile *arg0);
static FSResult FSi_RomReadFile(FSArchive *archive, FSFile *file, u32 size, FSiCmdReadFile *cmd);
static FSResult FSi_RomWriteFile(FSArchive *archive, FSFile *file, u32 arg2, FSiCmd1 *cmd);
static FSResult FS_func_0066(FSArchive *archive, FSFile *file, u32 arg2, u32 arg3);
static FSResult FS_func_0067(FSArchive *archive, FSFile *file, FS_UnkStruct13 *arg2);
static FSResult FS_func_0068(FSArchive *archive, u32 arg1, char *arg2, u32 *arg3, BOOL arg4);
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
static s32 FS_func_0095(FSArchive *archive, u32 arg1);
static BOOL FS_func_0093(FSArchive *arg0);
static void *FS_func_0092(FSArchive *archive, void *arg1);

const FSFileProcs FS_fileProcs = {
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
    int iVar10;
    FS_UnkStruct8 *puVar11;
    char *path;
    u32 uVar13;
    FS_UnkStruct8 uStack_bc;
    u32 uStack_b0;
    u32 uStack_ac;
    char acStack_a8[128];

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
            result = FS_fileProcs[proc](file);
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

void FS_LoadArchive(FSArchive *archive, s32 arg1, s32 arg2, s32 arg3, s32 arg4, s32 arg5,
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

BOOL FSi_GetLengthFromRom(FSFile *file, FSiCmdGetLength *cmd) {
    BOOL result;
    FSArchive *archive;

    archive = file->archive;
    result  = false;
    if (archive->fns == &sArchiveFns && FSi_RomGetLength(archive, file, cmd) == FS_RESULT_SUCCESS) {
        result = true;
    }
    return result;
}

BOOL FSi_GetPositionFromRom(FSFile *file, FSiCmdGetPosition *cmd) {
    BOOL result;
    FSArchive *archive;

    archive = file->archive;
    result  = false;
    if (archive->fns == &sArchiveFns && FSi_RomGetPosition(archive, file, cmd) == FS_RESULT_SUCCESS) {
        result = true;
    }
    return result;
}

BOOL FSi_SeekFileFromRom(FSFile *file, FS_UnkStruct14 arg1, s32 mode) {
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

void FS_SetArchiveProc(FSArchive *archive, FSResult (*proc)(FSFile *file, FSFileProc index), u32 overrideMask) {
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

s32 FS_func_0094(FSFile *file) {
    return file->unk_20.unk_04u;
}

static s32 FS_func_0095(FSArchive *archive, u32 arg1) {
    return 0;
}
