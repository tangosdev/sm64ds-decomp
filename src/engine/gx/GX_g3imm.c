/* NitroSDK g3imm.c (pret pokediamond arm9/lib/NitroSDK/src/GX_g3imm.c),
 * .text 0x0205532c..0x020553c0: the immediate-mode matrix senders. Each
 * writes its geometry command to the FIFO port and streams the matrix after
 * it. mwccarm emits C in reverse, so these four run ROM-descending in the
 * SDK's own source order; G3_LoadMtx44, G3_MultMtx44 and G3_Shininess are
 * dead-stripped.
 *
 * The SDK's parameters are const MtxFx43 *, const MtxFx33 * and
 * const VecFx32 *. They stay int * here, as the callers' declarations spell
 * them: retyping them is call-site matching work. */

#include "types.h"
#include "nitro/gx/g3.h"
#include "nitro/gx/gxasm.h"
#include "nitro/hw/registers.h"

void G3_LoadMtx43(int *m) {
    REG_GXFIFO = G3OP_MTX_LOAD_4x3;
    GX_SendFifo48B(m, (void *)&REG_GXFIFO);
}

void G3_MultMtx43(int *m) {
    REG_GXFIFO = G3OP_MTX_MULT_4x3;
    GX_SendFifo48B(m, (void *)&REG_GXFIFO);
}

void G3_MultMtx33(int *m) {
    REG_GXFIFO = G3OP_MTX_MULT_3x3;
    GX_SendFifo36B(m, (void *)&REG_GXFIFO);
}

/* The 3x3 matrix plus a translation row: sent as a 4x3 multiply. */
void G3_MultTransMtx33(int *mtx, int *trans) {
    REG_GXFIFO = G3OP_MTX_MULT_4x3;
    GX_SendFifo36B(mtx, (void *)&REG_GXFIFO);
    REG_GXFIFO = trans[0];
    REG_GXFIFO = trans[1];
    REG_GXFIFO = trans[2];
}
