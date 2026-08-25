#ifndef _NITRO_FS_H
#define _NITRO_FS_H

#include "nitro/os/common.h"
#ifdef __cplusplus
extern "C" {
#endif

#include "nitro/types.h"

#define FS_MAX_PATH 260

#define FS_SEEK_SET 0
#define FS_SEEK_CUR 1
#define FS_SEEK_END 2

#define FS_FILE_FLAG_SEND_CMD 0x1
#define FS_FILE_FLAG_0x2 0x2
#define FS_FILE_FLAG_AWAIT_SYNC 0x4
#define FS_FILE_FLAG_0x8 0x8
#define FS_FILE_FLAG_FILE 0x10
#define FS_FILE_FLAG_DIR 0x20
#define FS_FILE_FLAG_0x40 0x40
#define FS_FILE_FLAG_0x80 0x80
#define FS_FILE_FLAG_CMD_TYPE_MASK 0xff00
#define FS_FILE_FLAG_CMD_TYPE(flags) ((FSCmd) ((flags) >> 8))
#define FS_CMD_FILE_FLAG(cmdType) ((u32) (cmdType << 8))

#define FS_ARCHIVE_FLAG_0x1 0x1
#define FS_ARCHIVE_FLAG_0x2 0x2
#define FS_ARCHIVE_FLAG_0x4 0x4
#define FS_ARCHIVE_FLAG_0x8 0x8
#define FS_ARCHIVE_FLAG_0x10 0x10
#define FS_ARCHIVE_FLAG_0x20 0x20
#define FS_ARCHIVE_FLAG_0x40 0x40
#define FS_ARCHIVE_FLAG_0x80 0x80

#define FS_FILEMODE_R 0x1

#define FS_RESULT_SUCCESS ((FSResult) 0x0)
#define FS_RESULT_FAILURE ((FSResult) 0x1)
#define FS_RESULT_0x3 ((FSResult) 0x3)
#define FS_RESULT_0x5 ((FSResult) 0x5)
#define FS_RESULT_INVALID_COMMAND ((FSResult) 0x4)
#define FS_RESULT_INVALID_PARAM ((FSResult) 0x6)
#define FS_RESULT_0xB ((FSResult) 0xb)
#define FS_RESULT_AWAIT_ASYNC ((FSResult) 0x100)
#define FS_RESULT_0x101 ((FSResult) 0x101)
#define FS_RESULT_0x102 ((FSResult) 0x102)

#define FS_CMD_READ_FILE ((FSCmd) 0)
#define FS_CMD_WRITE_FILE ((FSCmd) 1)
#define FS_CMD_2 ((FSCmd) 2)
#define FS_CMD_3 ((FSCmd) 3)
#define FS_CMD_4 ((FSCmd) 4)
#define FS_CMD_5 ((FSCmd) 5)
#define FS_CMD_6 ((FSCmd) 6)
#define FS_CMD_7 ((FSCmd) 7)
#define FS_CMD_CLOSE_FILE ((FSCmd) 8)
#define FS_CMD_LOCK ((FSCmd) 9)
#define FS_CMD_UNLOCK ((FSCmd) 10)
#define FS_CMD_11 ((FSCmd) 11)
#define FS_CMD_12 ((FSCmd) 12)
#define FS_CMD_OPEN_FILE ((FSCmd) 13)
#define FS_CMD_SEEK_FILE ((FSCmd) 14)
#define FS_CMD_GET_LENGTH ((FSCmd) 15)
#define FS_CMD_GET_POSITION ((FSCmd) 16)
#define FS_CMD_17 ((FSCmd) 17)
#define FS_CMD_18 ((FSCmd) 18)
#define FS_CMD_19 ((FSCmd) 19)
#define FS_CMD_20 ((FSCmd) 20)
#define FS_CMD_21 ((FSCmd) 21)
#define FS_CMD_22 ((FSCmd) 22)
#define FS_CMD_23 ((FSCmd) 23)
#define FS_CMD_24 ((FSCmd) 24)
#define FS_CMD_25 ((FSCmd) 25)
#define FS_CMD_26 ((FSCmd) 26)
#define FS_CMD_27 ((FSCmd) 27)
#define FS_CMD_28 ((FSCmd) 28)
#define FS_CMD_29 ((FSCmd) 29)
#define FS_CMD_30 ((FSCmd) 30)
#define FS_CMD_31 ((FSCmd) 31)
#define FS_CMD_OPEN_DIR ((FSCmd) 32)
#define FS_CMD_CLOSE_DIR ((FSCmd) 33)
#define FS_CMD_34 ((FSCmd) 34)
#define FS_CMD_COUNT 35
#define FS_MAX_CMD_COUNT 64

#define FS_FILE_PROC_READ ((FSFileProc) 0)
#define FS_FILE_PROC_WRITE ((FSFileProc) 1)
#define FS_FILE_PROC_2 ((FSFileProc) 2)
#define FS_FILE_PROC_3 ((FSFileProc) 3)
#define FS_FILE_PROC_4 ((FSFileProc) 4)
#define FS_FILE_PROC_5 ((FSFileProc) 5)
#define FS_FILE_PROC_6 ((FSFileProc) 6)
#define FS_FILE_PROC_7 ((FSFileProc) 7)
#define FS_FILE_PROC_CLOSE ((FSFileProc) 8)
#define FS_FILE_PROC_SUSPEND ((FSFileProc) 9)
#define FS_FILE_PROC_UNLOCK ((FSFileProc) 10)
#define FS_FILE_PROC_11 ((FSFileProc) 11)
#define FS_FILE_PROC_12 ((FSFileProc) 12)
#define FS_FILE_PROC_COUNT ((FSFileProc) 13)

typedef u32 FSFileProc;
typedef s32 FSResult;
typedef u8 FSCmd;

struct FSFile;
struct FSArchive;
typedef struct FS_UnkStruct7 {
    /* 00 */ u32 unk_00;
    /* 04 */ u32 unk_04;
    /* 08 */ u32 unk_08;
    /* 0c */ u32 unk_0c;
    /* 10 */ u32 unk_10;
    /* 14 */ u32 unk_14;
    /* 18 */ u32 unk_18;
    /* 1c */ u32 unk_1c;
    /* 20 */ FSResult (*read)(struct FSArchive *, void *, void *, s32 size);
    /* 24 */ FSResult (*write)(struct FSArchive *, void *, void *, s32 size);
    /* 28 */ PAD(0x28, 0x2c);
    /* 2c */ FSResult (*proc)(struct FSFile *file, FSFileProc fn);
    /* 30 */ u32 overrideMask; // bitfield of FSFileProc to override when calling FSi_ExecuteFileProc
    /* 34 */
} FS_UnkStruct7;

struct FsArchiveFns;
typedef struct FSArchive {
    union {
        /* 00 */ char name[4];
        /* 00 */ char *pName;
    };
    /* 04 */ struct FSArchive *next;
    /* 08 */ struct FSFile *currentFile;
    /* 0c */ OSLinkedList unk_0c;
    /* 10 */ vu32 flags;
    /* 18 */ u32 unk_18;
    /* 1c */ u32 unk_1c;
    /* 20 */ FS_UnkStruct7 *unk_20;
    /* 24 */ const struct FsArchiveFns *fns;
    /* 28 */ FS_UnkStruct7 unk_28;
    /* 5c */
} FSArchive;

typedef struct FS_UnkStruct13 {
    /* 000 */ u8 unk_00[4];
    /* 004 */ PAD(0x04, 0x10);
    /* 010 */ u32 unk_10;
    /* 014 */ char unk_14[FS_MAX_PATH];
    /* 118 */ u32 unk_118;
    /* 11c */ u32 unk_11c;
    /* 120 */ PAD(0x120, 0x138);
    /* 138 */ u32 unk_138;
    /* 13c */ u32 unk_13c;
    /* 140 */ u32 unk_140;
    /* 144 */ u32 unk_144;
    /* 148 */ u32 unk_148;
    /* 14c */ u32 unk_14c;
    /* 150 */ PAD(0x150, 0x168);
    /* 168 */ u32 unk_168;
    /* 16c */ u32 unk_16c;
    /* 170 */
} FS_UnkStruct13;

typedef struct FS_UnkStruct15 {
    /* 00 */ u32 unk_00;
    /* 04 */ PAD(0x04, 0x4c);
    /* 4c */ u32 unk_4c;
    /* 50 */ u32 unk_50;
    /* 54 */
} FS_UnkStruct15;

typedef struct FS_UnkStruct16 {
    /* 00 */ u32 unk_00;
    /* 04 */ u32 unk_04;
    /* 08 */ u32 unk_08;
    /* 0c */ u32 unk_0c;
    /* 10 */ u32 unk_10;
    /* 14 */ u32 unk_14;
    /* 18 */ u32 unk_18;
    /* 1c */ u32 unk_1c;
    /* 20 */ u32 unk_20;
    /* 24 */ u32 unk_24;
    /* 28 */ u32 unk_28;
    /* 2c */ u32 unk_2c;
    /* 30 */
} FS_UnkStruct16;

typedef struct FS_UnkStruct19 {
    /* 00 */ u32 unk_00;
    /* 04 */ u32 unk_04;
    /* 08 */
} FS_UnkStruct19;

typedef struct FSiCmdReadFile {
    /* 00 */ void *buf;
    /* 04 */ u32 size;
    /* 08 */
} FSiCmdReadFile;

typedef struct FSiCmd1 {
    /* 00 */ u32 unk_00;
    /* 04 */ u32 unk_04;
    /* 08 */
} FSiCmd1;

typedef struct FSiCmd2 {
    /* 00 */ u32 unk_00;
    /* 04 */ u32 unk_04;
    /* 08 */
} FSiCmd2;

typedef struct FSiCmd3 {
    /* 00 */ FS_UnkStruct13 *unk_00;
    /* 04 */
} FSiCmd3;

typedef struct FSiCmd4 {
    /* 00 */ u32 unk_00;
    /* 04 */ char *path;
    /* 08 */ u32 unk_08;
    /* 0c */ u32 unk_0c;
    /* 10 */
} FSiCmd4;

typedef struct FSiCmd5 {
    /* 00 */ u32 unk_00;
    /* 04 */ u32 unk_04;
    /* 08 */ u32 unk_08;
    /* 0c */
} FSiCmd5;

typedef struct FSiCmd6 {
    /* 00 */ u32 unk_00;
    /* 04 */ u32 unk_04;
    /* 08 */
} FSiCmd6;

typedef struct FSiCmd7 {
    /* 00 */ u32 unk_00;
    /* 04 */ u32 unk_04;
    /* 08 */ u32 unk_08;
    /* 0c */ u32 unk_0c;
    /* 10 */
} FSiCmd7;

typedef struct FSiCmdOpenFile {
    /* 00 */ u32 unk_00;
    /* 04 */ char *path;
    /* 08 */ u32 flags;
    /* 0c */
} FSiCmdOpenFile;

typedef struct FSiCmdSeekFile {
    /* 00 */ s32 pos;
    /* 04 */ u32 mode;
    /* 08 */
} FSiCmdSeekFile;

typedef struct FSiCmd15 {
    /* 00 */ u32 length;
    /* 04 */
} FSiCmdGetLength;

typedef struct FSiCmd16 {
    /* 00 */ u32 pos;
    /* 04 */
} FSiCmdGetPosition;

typedef struct FSiCmd19 {
    /* 00 */ u32 unk_00;
    /* 04 */
} FSiCmd19;

typedef struct FSiCmd20 {
    /* 00 */ u32 unk_00;
    /* 04 */ u32 unk_04;
    /* 08 */ u32 unk_08;
    /* 0c */
} FSiCmd20;

typedef struct FSiCmd21 {
    /* 00 */ u32 unk_00;
    /* 04 */ u32 unk_04;
    /* 08 */
} FSiCmd21;

typedef struct FSiCmd22 {
    /* 00 */ u32 unk_00;
    /* 04 */ u32 unk_04;
    /* 08 */ u32 unk_08;
    /* 0c */ u32 unk_0c;
    /* 10 */
} FSiCmd22;

typedef struct FSiCmd23 {
    /* 00 */ u32 unk_00;
    /* 04 */ char *path;
    /* 08 */ FS_UnkStruct15 *unk_08;
    /* 0c */
} FSiCmd23;

typedef struct FSiCmd24 {
    /* 00 */ u32 unk_00;
    /* 04 */ u32 unk_04;
    /* 08 */ u32 unk_08;
    /* 0c */
} FSiCmd24;

typedef struct FSiCmd25 {
    /* 00 */ u32 unk_00;
    /* 04 */ u32 unk_04;
    /* 08 */ u32 unk_08;
    /* 0c */
} FSiCmd25;

typedef struct FSiCmd26 {
    /* 00 */ u32 unk_00;
    /* 04 */ u32 unk_04;
    /* 08 */
} FSiCmd26;

typedef struct FSiCmd27 {
    /* 00 */ u32 unk_00;
    /* 04 */ u32 unk_04;
    /* 08 */ u32 unk_08;
    /* 0c */ u32 unk_0c;
    /* 10 */
} FSiCmd27;

typedef struct FSiCmd28 {
    /* 00 */ FS_UnkStruct16 *unk_00;
    /* 04 */
} FSiCmd28;

typedef struct FSiCmd31 {
    /* 00 */ u32 unk_00;
    /* 04 */
} FSiCmd31;

typedef struct FSiCmd32 {
    /* 00 */ u32 unk_00;
    /* 04 */ char *path;
    /* 08 */ u32 unk_08;
    /* 0c */
} FSiCmd32;

typedef struct FSiCmd34 {
    /* 00 */ u32 unk_00;
    /* 04 */ u32 unk_04;
    /* 08 */
} FSiCmd34;

typedef FSResult (*const FSFileProcs[FS_FILE_PROC_COUNT])(struct FSFile *file);

typedef struct FsArchiveFns {
    union {
        /* 00 */ void *array[FS_MAX_CMD_COUNT];
        struct {
            /* 00 */ FSResult (*read)(struct FSArchive *archive, struct FSFile *file, u32 size, FSiCmdReadFile *cmd);
            /* 04 */ FSResult (*write)(struct FSArchive *archive, struct FSFile *file, u32, FSiCmd1 *cmd);
            /* 08 */ FSResult (*unk_08)(struct FSArchive *archive, struct FSFile *file, u32, u32);
            /* 0c */ FSResult (*unk_0c)(struct FSArchive *archive, struct FSFile *file, FS_UnkStruct13 *);
            /* 10 */ FSResult (*unk_10)(struct FSArchive *archive, u32, char *path, u32 *, u32);
            /* 14 */ FSResult (*unk_14)(struct FSArchive *archive, struct FSFile *file, u32, u32, u32 *);
            /* 18 */ FSResult (*unk_18)(struct FSArchive *archive, struct FSFile *file, u32, u32);
            /* 1c */ FSResult (*unk_1c)(struct FSArchive *archive, struct FSFile *file, u32, u32, FSiCmd7 *cmd);
            /* 20 */ FSResult (*close)(struct FSArchive *archive, struct FSFile *file);
            /* 24 */ FSResult (*lock)(struct FSArchive *archive);
            /* 28 */ FSResult (*unlock)(struct FSArchive *archive);
            /* 2c */ FSResult (*unk_2c)(struct FSArchive *archive);
            /* 30 */ FSResult (*unk_30)(struct FSArchive *archive);
            /* 34 */ FSResult (*open)(struct FSArchive *archive, struct FSFile *file, u32, char *path, u32 flags);
            /* 38 */ FSResult (*seek)(struct FSArchive *archive, struct FSFile *file, s32 *pos, u32 mode);
            /* 3c */ FSResult (*getLength)(struct FSArchive *archive, struct FSFile *file, FSiCmdGetLength *cmd);
            /* 40 */ FSResult (*getPosition)(struct FSArchive *archive, struct FSFile *file, FSiCmdGetPosition *cmd);
            /* 44 */ FSResult (*unk_44)(struct FSArchive *archive);
            /* 48 */ FSResult (*unk_48)(struct FSArchive *archive);
            /* 4c */ FSResult (*unk_4c)(struct FSArchive *archive, FSiCmd19 *cmd);
            /* 50 */ FSResult (*unk_50)(struct FSArchive *archive, u32, u32, u32);
            /* 54 */ FSResult (*unk_54)(struct FSArchive *archive, u32, u32);
            /* 58 */ FSResult (*unk_58)(struct FSArchive *archive, u32, u32, u32, u32);
            /* 5c */ FSResult (*unk_5c)(struct FSArchive *archive, u32, char *path, FS_UnkStruct15 *);
            /* 60 */ FSResult (*unk_60)(struct FSArchive *archive, u32, u32, u32);
            /* 64 */ FSResult (*unk_64)(struct FSArchive *archive, u32, u32, u32);
            /* 68 */ FSResult (*unk_68)(struct FSArchive *archive, u32, u32);
            /* 6c */ FSResult (*unk_6c)(struct FSArchive *archive, u32, u32, u32, u32);
            /* 70 */ FSResult (*unk_70)(struct FSArchive *archive, FS_UnkStruct16 *);
            /* 74 */ FSResult (*unk_74)(void); // unused?
            /* 78 */ FSResult (*unk_78)(struct FSArchive *archive, struct FSFile *file);
            /* 7c */ FSResult (*unk_7c)(struct FSArchive *archive, struct FSFile *file, u32);
            /* 80 */ FSResult (*openDir)(struct FSArchive *archive, struct FSFile *file, u32, char *path, u32);
            /* 84 */ FSResult (*closeDir)(struct FSArchive *archive, struct FSFile *file);
            /* 88 */ FSResult (*unk_88)(struct FSArchive *archive, struct FSFile *file, u32, u32);
        };
    };
    /* 8c */
} FSArchiveFns;

typedef struct FS_UnkStruct6 {
    /* 00 */ PAD(0x00, 0x04);
    /* 04 */ s32 start;
    /* 08 */ s32 end;
    /* 0c */ s32 pos;
    /* 10 */
} FS_UnkStruct6;

typedef struct FS_UnkStruct8 {
    /* 00 */ union {
        void *unk_00p;
        u32 unk_00u;
    };
    /* 04 */ union {
        void *unk_04p;
        u32 unk_04u;
        struct {
            u16 unk_04s;
            u16 unk_06s;
        };
    };
    /* 08 */ union {
        void *unk_08p;
        u32 unk_08u;
        struct {
            u16 unk_08s;
            u16 unk_0as;
        };
    };
    /* 0c */
} FS_UnkStruct8;

typedef struct FSFile {
    /* 00 */ struct FSFile *next;
    /* 04 */ void *cursor;
    /* 08 */ FSArchive *archive;
    /* 0c */ vu32 flags;
    /* 10 */ union {
        FSiCmdReadFile *readFile;
        FSiCmd1 *cmd1;
        FSiCmd2 *cmd2;
        FSiCmd3 *cmd3;
        FSiCmd4 *cmd4;
        FSiCmd5 *cmd5;
        FSiCmd6 *cmd6;
        FSiCmd7 *cmd7;
        FSiCmdSeekFile *seekFile;
        FSiCmdOpenFile *openFile;
        FSiCmdGetLength *getLength;
        FSiCmdGetPosition *getPosition;
        FSiCmd19 *cmd19;
        FSiCmd20 *cmd20;
        FSiCmd21 *cmd21;
        FSiCmd22 *cmd22;
        FSiCmd23 *cmd23;
        FSiCmd24 *cmd24;
        FSiCmd25 *cmd25;
        FSiCmd26 *cmd26;
        FSiCmd27 *cmd27;
        FSiCmd28 *cmd28;
        FSiCmd31 *cmd31;
        FSiCmd32 *cmd32;
        FSiCmd34 *cmd34;
        void *ptr;
    } cmd;
    /* 14 */ u32 unk_14;
    /* 18 */ OSLinkedList unk_18;
    /* 20 */ FS_UnkStruct8 unk_20;
    /* 2c */ union {
        void *unk_2cp;
        u32 unk_2cu;
        u16 unk_2cs;
    };
    /* 30 */ FS_UnkStruct8 unk_30;
    /* 3c */ char *unk_3c; // path?
    /* 40 */ u32 unk_40;
    /* 40 */ FS_UnkStruct8 *unk_44;
    /* 48 */
} FSFile;

typedef struct FSFntDirectory {
    /* 00 */ u32 subtableOffset;
    /* 04 */ u16 firstFileId;
    /* 06 */ u16 parentId;
    /* 08 */
} FSFntDirectory;

typedef struct FSDirEntry {
    /* 00 */ FSArchive *archive;
    /* 04 */ union {
        u32 unk_04u;
        struct {
            u16 unk_04s;
            u16 unk_06s;
        };
    };
    /* 08 */ u32 unk_08;
    /* 0c */ u32 unk_0c;
    /* 10 */ u32 unk_10;
    /* 14 */ char unk_14[0x80];
    /* 94 */
} FSDirEntry;

void FS_Init(u32 dmaCount);
void FS_InitFile(FSFile *file);
BOOL FS_OpenFile(FSFile *file, const char *path);
BOOL FS_OpenFileEx(FSFile *file, const char *path, u32 flags);
BOOL FS_SeekFile(FSFile *file, s32 pos, u32 mode);
u32 FS_GetLength(FSFile *file);
u32 FS_ReadFile(FSFile *file, void *buf, u32 size);
BOOL FS_CloseFile(FSFile *file);
inline BOOL FS_IsFile(FSFile *file) {
    return !!(file->flags & FS_FILE_FLAG_FILE);
}

FSResult FS_FindDir(FSFile *file, const char *path);
FSResult FS_ReadDir(FSFile *file, FSDirEntry *dir);
FSResult FS_CloseDirectory(FSFile *file);

#ifdef __cplusplus
} // extern "C"
#endif

#endif
