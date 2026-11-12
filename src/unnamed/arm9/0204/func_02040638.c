#include "types.h"
#include "WmWork.h"
#include "decl_common.h"

extern WMParentParam *data_020a0f44; /* 0x020a0f44 - the WMParentParam (func_0203fbc4) */
extern WMParentParam data_020a1064;  /* 0x020a1064 - the template func_0203db64 keeps */
extern u16             data_020a0f20;   /* 0x020a0f20 - counter */

/* Reload the parent parameters from the template at data_020a1064
 * and give the session the next tgid. */
void func_02040638(void) {
    MIi_CpuCopy32((int *)&data_020a1064, (int *)data_020a0f44, sizeof(WMParentParam));
    data_020a0f20 = data_020a0f20 + 1;
    data_020a0f44->tgid = data_020a0f20;
}
