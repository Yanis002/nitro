#ifndef _NITRO_MXI_H
#define _NITRO_MXI_H

#ifdef __cplusplus
extern "C" {
#endif

#include "nitro/types.h"

void PXI_Init(void);
BOOL PXI_IsCallbackReady(u32, u32);
void PXI_SetFifoRecvCallback(u32, void *);
s32 PXI_SendWordByFifo(u32, u32, u32);

#ifdef __cplusplus
}
#endif

#endif
