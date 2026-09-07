#include "nitro/fx.h"
#include "nitro/reg.h"

fx32 FX_Div(fx32 numer, fx32 denom) {
    FX_DivAsync(numer, denom);
    return FX_GetDivResult();
}

fx32 FX_Inv(fx32 denom) {
    FX_InvAsync(denom);
    return FX_GetDivResult();
}

fx64c FX_GetDivResultFx64c(void) {
    while (REG_DIV_CNT & 0x8000) {
    }
    return REG_DIV_RESULT;
}

fx32 FX_GetDivResult(void) {
    while (REG_DIV_CNT & 0x8000) {
    }
    return (REG_DIV_RESULT + 0x80000) >> 20;
}

void FX_InvAsync(fx32 denom) {
    REG_DIV_CNT   = 1;
    REG_DIV.numer = (u64) 0x1000 << 32;
    REG_DIV.denom = (u32) denom;
}

void FX_DivAsync(fx32 numer, fx32 denom) {
    REG_DIV_CNT   = 1;
    REG_DIV.numer = (u64) numer << 32;
    REG_DIV.denom = (u32) denom;
}

s32 FX_DivS32(s32 numer, s32 denom) {
    REG_DIV_CNT     = 0;
    REG_DIV.numerLo = numer;
    REG_DIV.denom   = (u32) denom;
    while (REG_DIV_CNT & 0x8000) {
    }
    return REG_DIV_RESULT;
}

s32 FX_ModS32(s32 numer, s32 denom) {
    REG_DIV_CNT     = 0;
    REG_DIV.numerLo = numer;
    REG_DIV.denom   = (u32) denom;
    while (REG_DIV_CNT & 0x8000) {
    }
    return REG_REM_RESULT;
}
