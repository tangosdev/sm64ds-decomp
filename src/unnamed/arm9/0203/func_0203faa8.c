/* func_0203faa8 at 0x0203faa8 (arm9), size 0xb4
 * Compiler mwccarm 1.2/sp2p3, flags:
 * -O4,p -enum int -lang c99 -char signed -interworking -proc arm946e -gccext,on -msgstyle gcc
 * The ne-path must be textually first: plain ==/!= return spellings canonicalize to
 * moveq-first (see notes/mwccarm-codegen.md); return r == 2, ?1:0, if/else and a
 * switch were re-tried and differ, so the r == 2 arm is left empty. */
#include "WmWork.h"

/* The WMParentParam func_0203fbc4 hands to WM_SetParentParameter. */
extern WmParentParam *data_020a0f44;
extern unsigned int data_020a0f58;
extern unsigned int data_020a0f64;
extern unsigned int data_020a0f68;
extern unsigned int data_020a0f60;

struct WMStartMPCallback;
extern void func_0203fdac(struct WMStartMPCallback *cb);

extern int WM_StartMPEx(void *fn, int a, unsigned short b, int c, unsigned short s0,
                        unsigned short s1, int z0, int z1, int z2, int z3, int one);

/* WM_StartMPEx's mpFreq (the sixth argument) is 1 unless the parent
 * parameters ask for continuous MP (CS_Flag). */
int func_0203faa8(void)
{
    unsigned short flag = (unsigned short)(data_020a0f44->CS_Flag == 0);
    unsigned short s0 = (unsigned short)data_020a0f58;

    int r = WM_StartMPEx((void *)func_0203fdac, data_020a0f68, (unsigned short)data_020a0f64,
                         data_020a0f60, s0, flag, 0, 0, 0, 0, 1);

    if (r == 2) {
    } else {
        return 0;
    }
    return 1;
}
