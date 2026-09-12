#include "nitro/fx.h"
#include "nitro/reg.h"
#include "nitro/types.h"

#ifdef NITRO_NO_ASM
void MTX_Transpose33_(const MtxFx33 *src, MtxFx33 *dst) {
    dst->_00 = src->_00;
    dst->_01 = src->_10;
    dst->_02 = src->_20;
    dst->_10 = src->_01;
    dst->_11 = src->_11;
    dst->_12 = src->_21;
    dst->_20 = src->_02;
    dst->_21 = src->_12;
    dst->_22 = src->_22;
}
#else
THUMB_DISABLE();
ASM void MTX_Transpose33_(const MtxFx33 *src, MtxFx33 *dst){
    #if __MWERKS__ // clang-format off
    stmdb sp!, {r4-r9}
    ldmia r0, {r2-r9, ip}
    stmia r1!, {r2, r5, r8}
    stmia r1!, {r3, r6, r9}
    stmia r1!, {r4, r7, ip}
    ldmia sp!, {r4-r9}
    bx lr
    #endif // clang-format on
} THUMB_ENABLE();
#endif

void MTX_RotX33_(MtxFx33 *mtx, fx32 sin, fx32 cos) {
    mtx->_00 = FX32_ONE;
    mtx->_01 = 0;
    mtx->_02 = 0;
    mtx->_10 = 0;
    mtx->_11 = cos;
    mtx->_12 = sin;
    mtx->_20 = 0;
    mtx->_21 = -sin;
    mtx->_22 = cos;
}

void MTX_RotZ33_(MtxFx33 *mtx, fx32 sin, fx32 cos) {
    mtx->_00 = cos;
    mtx->_01 = sin;
    mtx->_02 = 0;
    mtx->_10 = -sin;
    mtx->_11 = cos;
    mtx->_12 = 0;
    mtx->_20 = 0;
    mtx->_21 = 0;
    mtx->_22 = FX32_ONE;
}

THUMB_DISABLE();
void MTX_Concat33(MtxFx33 *a, MtxFx33 *b, MtxFx33 *dst) {
    MtxFx33 temp;
    MtxFx33 *x;
    VecFx32 va, vb;

    if (dst == b) {
        x = &temp;
    } else {
        x = dst;
    }

    // SLOW: mwccarm generates more instructions than needed when storing and reusing `va` and `vb`.
    // It's better to always access `a` and `b` directly.
    VEC_Set(va, a->_00, a->_01, a->_02);
    x->_00 = ((fx64) va.x * (fx64) b->_00 + (fx64) va.y * (fx64) b->_10 + (fx64) va.z * (fx64) b->_20) >> FX32_SHIFT;
    x->_01 = ((fx64) va.x * (fx64) b->_01 + (fx64) va.y * (fx64) b->_11 + (fx64) va.z * (fx64) b->_21) >> FX32_SHIFT;

    VEC_Set(vb, b->_02, b->_12, b->_22);
    x->_02 = ((fx64) va.x * (fx64) vb.x + (fx64) va.y * (fx64) vb.y + (fx64) va.z * (fx64) vb.z) >> FX32_SHIFT;

    VEC_Set(va, a->_10, a->_11, a->_12);
    x->_12 = ((fx64) va.x * (fx64) vb.x + (fx64) va.y * (fx64) vb.y + (fx64) va.z * (fx64) vb.z) >> FX32_SHIFT;
    x->_11 = ((fx64) va.x * (fx64) b->_01 + (fx64) va.y * (fx64) b->_11 + (fx64) va.z * (fx64) b->_21) >> FX32_SHIFT;

    VEC_Set(vb, b->_00, b->_10, b->_20);
    x->_10 = ((fx64) va.x * (fx64) vb.x + (fx64) va.y * (fx64) vb.y + (fx64) va.z * (fx64) vb.z) >> FX32_SHIFT;

    VEC_Set(va, a->_20, a->_21, a->_22);
    x->_20 = ((fx64) va.x * (fx64) vb.x + (fx64) va.y * (fx64) vb.y + (fx64) va.z * (fx64) vb.z) >> FX32_SHIFT;
    x->_21 = ((fx64) va.x * (fx64) b->_01 + (fx64) va.y * (fx64) b->_11 + (fx64) va.z * (fx64) b->_21) >> FX32_SHIFT;
    x->_22 = ((fx64) va.x * (fx64) b->_02 + (fx64) va.y * (fx64) b->_12 + (fx64) va.z * (fx64) b->_22) >> FX32_SHIFT;

    if (x != &temp) {
        return;
    }

    *dst = temp;
}

void MTX_Identity43_(MtxFx43 *mtx) {
    mtx->_00 = FX32_ONE;
    mtx->_01 = 0;
    mtx->_02 = 0;
    mtx->_10 = 0;
    mtx->_11 = FX32_ONE;
    mtx->_12 = 0;
    mtx->_20 = 0;
    mtx->_21 = 0;
    mtx->_22 = FX32_ONE;
    mtx->_30 = 0;
    mtx->_31 = 0;
    mtx->_32 = 0;
}

void MTX_PerspectiveW(fx32 arg0, fx32 arg1, fx32 arg2, fx32 arg3, fx32 arg4, fx32 arg5, MtxFx44 *mtx) {
    s32 temp_r0;
    s32 temp_r2;
    s32 temp_r3;
    s32 temp_r6;
    s32 var_r4;
    s64 var_r0;

    temp_r0       = FX_Div(arg1, arg0);
    REG_DIV.numer = (fx64) FX32_ONE << 32;
    REG_DIV.denom = (u32) arg3 - arg4;
    var_r4        = temp_r0;
    if (arg5 != FX32_ONE) {
        var_r4 = var_r4 * arg5;
        var_r4 += (u32) (var_r4 >> 11) >> 20;
        var_r4 >>= FX32_SHIFT;
    }
    mtx->_01      = 0;
    mtx->_02      = 0;
    mtx->_03      = 0;
    mtx->_10      = 0;
    mtx->_11      = var_r4;
    mtx->_12      = 0;
    mtx->_13      = 0;
    mtx->_20      = 0;
    mtx->_21      = 0;
    mtx->_23      = -arg5;
    mtx->_30      = 0;
    mtx->_31      = 0;
    mtx->_33      = 0;
    var_r0        = FX_GetDivResultFx64c();
    REG_DIV.numer = (u64) var_r4 << 32;
    REG_DIV.denom = (u32) arg2;
    if (arg5 != FX32_ONE) {
        var_r0 = (var_r0 * arg5) / FX32_ONE;
    }
    temp_r2  = arg3 * 2;
    temp_r6  = arg4 + arg3;
    temp_r3  = FX_MUL(temp_r2, arg4);
    mtx->_22 = FX_MUL32x64C(var_r0, temp_r6);
    mtx->_32 = FX_MUL32x64C(var_r0, temp_r3);
    mtx->_00 = FX_GetDivResult();
}

void MTX_OrthoW(fx32 top, fx32 bottom, fx32 left, fx32 right, fx32 near, fx32 far, fx32 scale, MtxFx44 *mtx) {
    s32 temp_ip;
    s32 temp_r2;
    s32 temp_r3;
    fx64c var_r0;
    fx64c var_r4;
    fx64c var_r5;

    FX_InvAsync(right - left);
    mtx->_01      = 0;
    mtx->_02      = 0;
    mtx->_03      = 0;
    mtx->_10      = 0;
    mtx->_12      = 0;
    mtx->_13      = 0;
    mtx->_20      = 0;
    mtx->_21      = 0;
    mtx->_23      = 0;
    mtx->_33      = scale;
    var_r4        = FX_GetDivResultFx64c();
    REG_DIV.numer = (u64) FX32_ONE << 32;
    REG_DIV.denom = (u32) (top - bottom);
    if (scale != FX32_ONE) {
        var_r4 = (var_r4 * scale) / FX32_ONE;
    }
    mtx->_00      = (((var_r4 << 1) << FX32_SHIFT) + 0x80000000) >> 32;
    var_r5        = FX_GetDivResultFx64c();
    REG_DIV.numer = (u64) FX32_ONE << 32;
    REG_DIV.denom = (u32) (near - far);
    if (scale != FX32_ONE) {
        var_r5 = (var_r5 * scale) / FX32_ONE;
    }
    mtx->_11 = ((var_r5 << 13) + 0x80000000) >> 32;
    var_r0   = FX_GetDivResultFx64c();
    if (scale != FX32_ONE) {
        var_r0 = (var_r0 * scale) / FX32_ONE;
    }
    temp_ip  = -(right + left);
    temp_r3  = -(top + bottom);
    temp_r2  = far + near;
    mtx->_22 = ((var_r0 << 13) + 0x80000000) >> 32;
    mtx->_30 = FX_MUL32x64C(var_r4, temp_ip);
    mtx->_31 = FX_MUL32x64C(var_r5, temp_r3);
    mtx->_32 = FX_MUL32x64C(var_r0, temp_r2);
}
