#ifndef _NITRO_OS_PROTECTION_H
#define _NITRO_OS_PROTECTION_H

#ifdef __cplusplus
extern "C" {
#endif

#include "nitro/types.h"

void OS_SetProtectionRegion1(u32);
void OS_SetProtectionRegion2(u32);

#ifdef __cplusplus
}
#endif

#endif
