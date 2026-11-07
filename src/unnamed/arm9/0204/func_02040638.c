#include "types.h"
#include "WmWork.h"
extern void MultiCopy_Int(void *src, void *dst, s32 count);

extern WmParentParam *data_020a0f44;    /* 0x020a0f44 - the WMParentParam (func_0203fbc4) */
extern u8 data_020a1064[]; /* 0x020a1064 - the 0x40-byte template func_0203db64 keeps */
extern u16             data_020a0f20;   /* 0x020a0f20 - counter */

/* Reload the parent parameters from the 0x40-byte template at data_020a1064
 * and give the session the next tgid. */
void func_02040638(void) {
    MultiCopy_Int(data_020a1064, data_020a0f44, 0x40);
    data_020a0f20 = data_020a0f20 + 1;
    data_020a0f44->tgid = data_020a0f20;
}
