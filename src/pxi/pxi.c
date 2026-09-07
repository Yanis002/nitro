#include "nitro/pxi.h"
#include "nitro/os/os_irq.h"
#include "nitro/reg.h"
#include "nitro/types.h"

static void PXIi_HandlerRecvFifoNotEmpty(void);

static u16 PXIi_fifoInitialized;
static void (*sFifoRecvCallbacks[32])(u32, u32, u32);

void PXI_Init(void) {
    PXI_InitFifo();
}

void PXI_InitFifo(void) {
    OSIntrMode irq;
    void (**var_r1)(u32, u32, u32);
    s32 i;
    s32 attempts;
    s32 ipcOutput;
    s32 var_r3;

    irq = OS_DisableInterrupts();
    if (!PXIi_fifoInitialized) {
        PXIi_fifoInitialized = true;
        // TODO: Must be accessed relative to 027ffc00, not 027fff88 directly. Also applies to other
        // REG_IPC_FIFO_RECV_CALLBACKS accesses below.
        REG_IPC_FIFO_RECV_CALLBACKS = 0;
        for (i = 0, var_r1 = sFifoRecvCallbacks; i < ARRAY_LEN(sFifoRecvCallbacks); ++i, ++var_r1) {
            *var_r1 = NULL;
        }
        REG_IPC_FIFO_CNT = 0xC408;
        OS_ResetRequestIrqMask(0x40000);
        OS_SetIrqFunction(0x40000, PXIi_HandlerRecvFifoNotEmpty);
        OS_EnableIrqMask(0x40000);
        for (var_r3 = 0; true; ++var_r3) {
            ipcOutput    = REG_IPC_SYNC & 0xF;
            REG_IPC_SYNC = ipcOutput << 8;
            if (ipcOutput == 0 && var_r3 > 4) {
                break;
            }
            for (attempts = 1000; (REG_IPC_SYNC & 0xF) == ipcOutput; --attempts) {
                if (attempts <= 0) {
                    var_r3 = 0;
                    break;
                }
            }
        }
    }
    OS_RestoreInterrupts(irq);
}

void PXI_SetFifoRecvCallback(u32 index, void (*callback)(u32, u32, u32)) {
    OSIntrMode irq;

    irq                       = OS_DisableInterrupts();
    sFifoRecvCallbacks[index] = callback;
    if (callback != NULL) {
        REG_IPC_FIFO_RECV_CALLBACKS |= 1 << index;
    } else {
        REG_IPC_FIFO_RECV_CALLBACKS &= ~(1 << index);
    }
    OS_RestoreInterrupts(irq);
}

s32 PXI_IsCallbackReady(u32 index, u32 arg1) {
    return !!((&REG_IPC_FIFO_RECV_CALLBACKS)[arg1] & (1 << index));
}

s32 PXI_SendWordByFifo(u32 arg0, u32 arg1, u32 arg2) {
    PXI_UnkStruct1 sp0;
    OSIntrMode irq;

    sp0.unk_00_0 = arg0;
    sp0.unk_00_5 = arg2;
    sp0.unk_00_6 = arg1;
    if (REG_IPC_FIFO_CNT & PXI_IPC_FIFO_CNT_ERROR) {
        REG_IPC_FIFO_CNT |= PXI_IPC_FIFO_CNT_ENABLE | PXI_IPC_FIFO_CNT_ERROR_ACK;
        return -1;
    }
    irq = OS_DisableInterrupts();
    if (REG_IPC_FIFO_CNT & PXI_IPC_FIFO_CNT_SEND_FULL) {
        OS_RestoreInterrupts(irq);
        return -2;
    } else {
        REG_IPC_FIFO_SEND = sp0;
        OS_RestoreInterrupts(irq);
        return 0;
    }
}

static void PXIi_HandlerRecvFifoNotEmpty(void) {
    PXI_UnkStruct1 fifoSend;
    void (*callback)(u32, u32, u32);
    s32 result;
    u32 temp_r0;
    OSIntrMode irq;

    while (true) {
        if (REG_IPC_FIFO_CNT & PXI_IPC_FIFO_CNT_ERROR) {
            REG_IPC_FIFO_CNT |= PXI_IPC_FIFO_CNT_ENABLE | PXI_IPC_FIFO_CNT_ERROR_ACK;
            result = -3;
        } else {
            irq = OS_DisableInterrupts();
            if (REG_IPC_FIFO_CNT & PXI_IPC_FIFO_CNT_RECV_EMPTY) {
                OS_RestoreInterrupts(irq);
                result = -4;
            } else {
                fifoSend = REG_04100000;
                OS_RestoreInterrupts(irq);
                result = 0;
            }
        }
        if (result == -4) {
            break;
        }
        if (result != -3) {
            temp_r0 = fifoSend.unk_00_0;
            if (temp_r0 != 0) {
                callback = sFifoRecvCallbacks[temp_r0];
                if (callback != NULL) {
                    callback(temp_r0, fifoSend.unk_00_6, fifoSend.unk_00_5);
                } else if (fifoSend.unk_00_5 == 0) {
                    fifoSend.unk_00_5 = 1;
                    if (REG_IPC_FIFO_CNT & PXI_IPC_FIFO_CNT_ERROR) {
                        REG_IPC_FIFO_CNT |= PXI_IPC_FIFO_CNT_ENABLE | PXI_IPC_FIFO_CNT_ERROR_ACK;
                    } else {
                        irq = OS_DisableInterrupts();
                        if (REG_IPC_FIFO_CNT & PXI_IPC_FIFO_CNT_SEND_FULL) {
                            OS_RestoreInterrupts(irq);
                        } else {
                            REG_IPC_FIFO_SEND = fifoSend;
                            OS_RestoreInterrupts(irq);
                        }
                    }
                }
            }
        }
    }
}
