#include "nitro/mic.h"

extern vu32 data_021fb4f0;

THUMB_DISABLE
void MicWaitBusy(void) {
    vu32 *var_r0;
    var_r0 = &data_021fb4f0;
    while (*var_r0 == 1) {
    }
}
THUMB_ENABLE
