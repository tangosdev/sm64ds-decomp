/* func_0203faa8 at 0x0203faa8 (arm9), size 0xb4
 * Compiler mwccarm 1.2/sp2p3, flags:
 * -O4,p -enum int -lang c99 -char signed -interworking -proc arm946e -gccext,on -msgstyle gcc
 * The ne-path must be textually first: plain ==/!= return spellings canonicalize to
 * moveq-first (see notes/mwccarm-codegen.md); return r == 2, ?1:0, if/else and a
 * switch were re-tried and differ, so the r == 2 arm is left empty. */
#include "WmWork.h"
#include "nitro/wm/wm_api.h"

/* The WMParentParam func_0203fbc4 hands to WM_SetParentParameter. */
extern WMParentParam *data_020a0f44;
extern unsigned int data_020a0f58;
extern unsigned int data_020a0f64;
extern void *data_020a0f68;
extern void *data_020a0f60;

extern void func_0203fdac(void *arg);

/* WM_StartMPEx's mpFreq (the sixth argument) is 1 unless the parent
 * parameters ask for continuous MP (CS_Flag). */
int func_0203faa8(void)
{
    unsigned short flag = (unsigned short)(data_020a0f44->CS_Flag == 0);
    unsigned short s0 = (unsigned short)data_020a0f58;

    int r = WM_StartMPEx(func_0203fdac, (u16 *)data_020a0f68, (unsigned short)data_020a0f64,
                         (u16 *)data_020a0f60, s0, flag, 0, 0, 0, 0, 1);

    if (r == WM_ERRCODE_OPERATING) {
    } else {
        return 0;
    }
    return 1;
}
