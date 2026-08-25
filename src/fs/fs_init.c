#include "nitro/fs.h"

static BOOL FSi_Initialized;

void FS_Init(u32 dmaCount) {
    if (FSi_Initialized) {
        return;
    }
    FSi_Initialized = true;
    FSi_InitRom(dmaCount);
    FS_func_0060();
}
