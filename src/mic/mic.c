#include "nitro/mic.h"
#include "nitro/os.h"
#include "nitro/pxi.h"
#include "nitro/reg.h"
#include "nitro/types.h"

static void MicCommonCallback(u32 arg0, u8 arg1, s32 arg2);
static BOOL MicStartAutoSampling(u32 arg0, u32 arg1, u32 arg2, u8 arg3);
static BOOL MicStopAutoSampling(void);
static void MicGetResultCallback(s32 arg0, s32 arg1);

u16 data_021fb4ec;
u16 *data_021fb508;
vu32 data_021fb4f0;
u32 data_021fb504;
u32 data_021fb4f8;
u32 data_021fb4fc;
void (*data_021fb500)(u32, u32);
void (*data_021fb4f4)(s32, s32);

void MIC_Init(void) {
    if (data_021fb4ec != 0) {
        return;
    }
    data_021fb4ec = 1;
    data_021fb4f0 = 0;
    data_021fb4f4 = 0;
    PXI_Init();
    while (!PXI_IsCallbackReady(9, 1)) {
    }
    REG_027FFF90 = 0;
    PXI_SetFifoRecvCallback(9, MicCommonCallback);
}

s32 MIC_StartAutoSamplingAsync(MIC_UnkStruct2 *arg0, void (*arg1)(s32, s32), s32 arg2) {
    s32 var_r1;
    u32 var_r0;
    u8 var_r5;
    OSIntrMode irq;

    if (arg0->unk_04 & 0x1F) {
        return 2;
    } else if (arg0->unk_08 & 0x1F) {
        return 2;
    } else if (arg0->unk_08 == 0) {
        return 2;
    } else if (arg0->unk_0c < 0x400U) {
        return 2;
    }
    switch (arg0->unk_00) {
        case 0:
            var_r1 = 0;
            break;
        case 1:
            var_r1 = 1;
            break;
        case 2:
            var_r1 = 2;
            break;
        case 3:
            var_r1 = 3;
            break;
        case 4:
            var_r1 = 5;
            break;
        case 5:
            var_r1 = 7;
            break;
        default:
            return 2;
    }
    if (arg0->unk_10 != 0) {
        var_r0 = (u8) (0x10 | var_r1);
    } else {
        var_r0 = (u8) var_r1;
    }
    var_r5 = var_r0;
    irq    = OS_DisableInterrupts();
    if (data_021fb4f0 != 0) {
        OS_RestoreInterrupts(irq);
        return 1;
    }
    data_021fb4f0 = 1;
    OS_RestoreInterrupts(irq);
    data_021fb4f4 = arg1;
    data_021fb4f8 = arg2;
    data_021fb500 = arg0->unk_14;
    data_021fb504 = arg0->unk_18;
    if (MicStartAutoSampling(arg0->unk_04, arg0->unk_08, arg0->unk_0c, var_r5)) {
        return 0;
    }
    return 3;
}

s32 MIC_StartAutoSampling(MIC_UnkStruct2 *arg0) {
    s32 temp_r0;

    temp_r0       = MIC_StartAutoSamplingAsync(arg0, MicGetResultCallback, 0);
    data_021fb4fc = temp_r0;
    if (temp_r0 == 0) {
        MicWaitBusy();
    }
    return data_021fb4fc;
}

s32 MIC_StopAutoSamplingAsync(void (*arg0)(s32, s32), s32 arg1) {
    OSIntrMode irq;

    irq = OS_DisableInterrupts();
    if (data_021fb4f0 != 0) {
        OS_RestoreInterrupts(irq);
        return 1;
    }
    data_021fb4f0 = 1;
    OS_RestoreInterrupts(irq);
    data_021fb4f4 = arg0;
    data_021fb4f8 = arg1;
    if (MicStopAutoSampling()) {
        return 0;
    }
    return 3;
}

s32 MIC_StopAutoSampling(void) {
    s32 temp_r0;

    temp_r0       = MIC_StopAutoSamplingAsync(MicGetResultCallback, 0);
    data_021fb4fc = temp_r0;
    if (temp_r0 == 0) {
        MicWaitBusy();
    }
    return data_021fb4fc;
}

s32 MIC_GetLastSamplingAddress(void) {
    return REG_027FFF90;
}

static void MicCommonCallback(u32 arg0, u8 arg1, s32 arg2) {
    void (*temp_r2)(s32, s32);
    void (*temp_r3)(s32, s32);
    s32 var_r0;
    u16 temp_r0;
    u32 temp_r1;

    if (arg2 != 0) {
        if (data_021fb4f0 != 0) {
            data_021fb4f0 = 0;
        }
        temp_r2 = data_021fb4f4;
        if (temp_r2 != NULL) {
            data_021fb4f4 = NULL;
            temp_r2(6, data_021fb4f8);
        }
    }
    temp_r1 = (u32) ((0x7F00 & arg1) << 8) >> 0x10;
    temp_r0 = (u16) (u32) (u8) (u32) arg1;
    switch (temp_r0) {
        case 0:
            var_r0 = 0;
            break;
        case 1:
            var_r0 = 4;
            break;
        case 2:
            var_r0 = 2;
            break;
        case 3:
            var_r0 = 5;
            break;
        case 4:
            var_r0 = 1;
            break;
        default:
            var_r0 = 6;
            break;
    }
    if (temp_r1 == 0x51) {
        if (data_021fb500 != NULL) {
            data_021fb500(var_r0, data_021fb504);
        }
    } else {
        if ((temp_r1 == 0x40) && (data_021fb508 != NULL)) {
            *data_021fb508 = *(u16 *) 0x027FFF94;
        }
        if (data_021fb4f0 != 0) {
            data_021fb4f0 = 0;
        }
        temp_r3 = data_021fb4f4;
        if (temp_r3 != NULL) {
            data_021fb4f4 = NULL;
            temp_r3(var_r0, data_021fb4f8);
        }
    }
}

static BOOL MicStartAutoSampling(u32 arg0, u32 arg1, u32 arg2, u8 arg3) {
    if (PXI_SendWordByFifo(9, 0x2004100 | arg3, 0) < 0) {
        return false;
    } else if (PXI_SendWordByFifo(9, 0x10000 | (arg0 >> 16), 0) < 0) {
        return false;
    } else if (PXI_SendWordByFifo(9, 0x20000 | (arg0 & 0xffff), 0) < 0) {
        return false;
    } else if (PXI_SendWordByFifo(9, 0x30000 | (arg1 >> 16), 0) < 0) {
        return false;
    } else if (PXI_SendWordByFifo(9, 0x40000 | (arg1 & 0xffff), 0) < 0) {
        return false;
    } else if (PXI_SendWordByFifo(9, 0x50000 | (arg2 >> 16), 0) < 0) {
        return false;
    } else if (PXI_SendWordByFifo(9, 0x01060000 | (arg2 & 0xffff), 0) < 0) {
        return false;
    }
    return true;
}

static BOOL MicStopAutoSampling(void) {
    return PXI_SendWordByFifo(9, 0x03004200, 0) >= 0;
}

static void MicGetResultCallback(s32 arg0, s32 arg1) {
    data_021fb4fc = arg0;
}
