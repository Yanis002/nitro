#ifndef _NITRO_FS_OVERLAY_H
#define _NITRO_FS_OVERLAY_H

#ifdef __cplusplus
extern "C" {
#endif

#include "nitro/types.h"

#define EXTERN_OVERLAY_ID(name_or_index) extern u32 OVERLAY_##name_or_index##_ID;
#define OVERLAY_ID(name_or_index) ((u32) & OVERLAY_##name_or_index##_ID)

typedef struct FSOverlay {
    /* 00 */ s32 id;
    /* 04 */ void *addr;
    /* 08 */ u32 textSize;
    /* 0C */ s32 bssSize;
    /* 10 */ s32 ctorStart;
    /* 14 */ s32 ctorEnd;
    /* 18 */ s32 fileId;
    /* 1C */ u32 fileSize;
    /* 20 */
} FSOverlay;

u32 FS_GetOverlaySize(FSOverlay *overlay);
void FS_ClearOverlayCacheAndBss(FSOverlay *overlay);
void FS_Overlay_0202d6cc(void **param1, FSOverlay *overlay); // param1 is the address of gArchiveList
BOOL FS_Overlay_0202d6f4(FSOverlay *param1, FSOverlay *param2, s32 param3, s32 param4, s32 param5, u32 param6,
                         s32 param7, u32 param8);
BOOL FS_LoadOverlayInfo(FSOverlay *overlay, s32 param2, s32 param3);
BOOL FS_LoadOverlayFile(FSOverlay *overlay);
BOOL FS_Overlay_0202d984(FSOverlay *param1, s32 param2, s32 param3);
BOOL FS_StartOverlay(FSOverlay *overlay);
BOOL FS_CleanupOverlayResources(FSOverlay *overlay);
BOOL FS_StopOverlay(FSOverlay *overlay);
BOOL FS_LoadOverlay(FSOverlay *overlay, s32 param2);
BOOL FS_UnloadOverlay(FSOverlay *overlay, s32 param2);

#ifdef __cplusplus
} // extern "C"
#endif

#endif
