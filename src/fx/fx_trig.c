#include "nitro/fx.h"
#include "nitro/math.h"

extern s16 sAtanTable[];
extern s16 sAtanIdxTable[];

s16 FX_Atan2(fx32 y, fx32 x) {
    BOOL clockwise;
    s32 baseAngle;
    s32 temp_r0;
    s32 numer;
    s32 denom;

    denom = x;
    if (y > 0) {
        if (denom > 0) {
            if (denom > y) {
                numer     = y;
                baseAngle = 0;
                clockwise = true;
            } else if (denom < y) {
                numer     = denom;
                denom     = y;
                baseAngle = FX_F32_TO_FX32(M_PI / 2);
                clockwise = false;
            } else {
                return FX_F32_TO_FX32(M_PI / 4);
            }
        } else if (denom < 0) {
            denom = -denom;
            if (denom < y) {
                numer     = denom;
                denom     = y;
                baseAngle = FX_F32_TO_FX32(M_PI / 2);
                clockwise = true;
            } else if (denom > y) {
                numer     = y;
                baseAngle = FX_F32_TO_FX32(M_PI);
                clockwise = false;
            } else {
                return FX_F32_TO_FX32(3 * M_PI / 4);
            }
        } else {
            return FX_F32_TO_FX32(M_PI / 2);
        }
    } else if (y < 0) {
        temp_r0 = -y;
        if (denom < 0) {
            denom = 0 - denom;
            if (denom > temp_r0) {
                numer     = temp_r0;
                baseAngle = -FX_F32_TO_FX32(M_PI);
                clockwise = true;
            } else if (denom < temp_r0) {
                numer     = denom;
                denom     = temp_r0;
                baseAngle = -FX_F32_TO_FX32(M_PI / 2);
                clockwise = false;
            } else {
                return -FX_F32_TO_FX32(3 * M_PI / 4);
            }
        } else if (denom > 0) {
            if (denom < temp_r0) {
                numer     = denom;
                denom     = temp_r0;
                baseAngle = -FX_F32_TO_FX32(M_PI / 2);
                clockwise = true;
            } else if (denom > temp_r0) {
                baseAngle = 0;
                numer     = temp_r0;
                clockwise = false;
            } else {
                return -FX_F32_TO_FX32(M_PI / 4);
            }
        } else {
            return -FX_F32_TO_FX32(M_PI / 2);
        }
    } else if (denom >= 0) {
        return 0;
    } else {
        return FX_F32_TO_FX32(M_PI);
    }
    if (denom == 0) {
        return 0;
    } else if (clockwise) {
        return baseAngle + sAtanTable[FX_Div(numer, denom) >> 5];
    } else {
        return baseAngle - sAtanTable[FX_Div(numer, denom) >> 5];
    }
}

u16 FX_AtanIdx(s32 tan) {
    if (tan >= 0) {
        if (tan > FX32_ONE) {
            return 4 * FX32_ONE - sAtanIdxTable[FX_Inv(tan) >> 5];
        } else if (tan < FX32_ONE) {
            return sAtanIdxTable[tan >> 5];
        } else {
            return 2 * FX32_ONE;
        }
    } else if (tan < -FX32_ONE) {
        return sAtanIdxTable[FX_Inv(-tan) >> 5] - 4 * FX32_ONE;
    } else if (tan > -FX32_ONE) {
        return -sAtanIdxTable[-tan >> 5];
    } else {
        return 14 * FX32_ONE;
    }
}

u16 FX_Atan2Idx(s32 y, s32 x) {
    BOOL clockwise;
    s32 baseAngle;
    s32 temp_r0;
    s32 numer;
    s32 denom;

    denom = x;
    if (y > 0) {
        if (denom > 0) {
            if (denom > y) {
                numer     = y;
                baseAngle = 0;
                clockwise = true;
            } else if (denom < y) {
                numer     = denom;
                denom     = y;
                baseAngle = 0x4000;
                clockwise = false;
            } else {
                return 0x2000;
            }
        } else if (denom < 0) {
            denom = 0 - denom;
            if (denom < y) {
                numer     = denom;
                denom     = y;
                baseAngle = 0x4000;
                clockwise = true;
            } else if (denom > y) {
                numer     = y;
                baseAngle = 0x8000;
                clockwise = false;
            } else {
                return 0x6000;
            }
        } else {
            return 0x4000;
        }
    } else if (y < 0) {
        temp_r0 = -y;
        if (denom < 0) {
            denom = 0 - denom;
            if (denom > temp_r0) {
                numer     = temp_r0;
                baseAngle = -0x8000;
                clockwise = true;
            } else if (denom < temp_r0) {
                numer     = denom;
                denom     = temp_r0;
                baseAngle = -0x4000;
                clockwise = false;
            } else {
                return -0x6000;
            }
        } else if (denom > 0) {
            if (denom < temp_r0) {
                numer     = denom;
                denom     = temp_r0;
                baseAngle = -0x4000;
                clockwise = true;
            } else if (denom > temp_r0) {
                baseAngle = 0;
                numer     = temp_r0;
                clockwise = false;
            } else {
                return -0x2000;
            }
        } else {
            return 0xC000U;
        }
    } else if (denom >= 0) {
        return 0;
    } else {
        return 0x8000;
    }
    if (denom == 0) {
        return 0;
    } else if (clockwise != 0) {
        return (baseAngle + sAtanIdxTable[FX_Div(numer, denom) >> 5]);
    } else {
        return (baseAngle - sAtanIdxTable[FX_Div(numer, denom) >> 5]);
    }
}
