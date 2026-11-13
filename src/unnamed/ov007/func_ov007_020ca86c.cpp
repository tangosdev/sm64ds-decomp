//cpp
#include "types.h"
#include "common.h"
#include "nitro/gx/g3imm_cmd.h"



#define SEND_VTX(p) G3_Vtx((s16)((p)->x >> shift), (s16)((p)->y >> shift), (s16)((p)->z >> shift))
#define SEND_NRM(q) G3_Normal((q)->x, (q)->y, (q)->z)

void func_ov007_020ca86c(Vector3 *vtx, Vector3_16 *nrm, s32 w, s32 h, s32 shift) {
    s32 j, i;
    s32 dir;
    s32 w1;
    s32 rowbase;
    Vector3_16 *n;
    Vector3_16 *n2;
    Vector3 *p00, *p10, *p01, *p11;

    G3_Begin(GX_BEGIN_TRIANGLES);
    w1 = w + 1;
    dir = 1;
    for (i = 0; i < h; i++) {
        rowbase = i * w1;
        n = nrm + i * w * 2;
        for (j = 0; j < w; j++) {
            p00 = vtx + (rowbase + j);
            p10 = p00 + w1;
            p01 = p00 + 1;
            p11 = p10 + 1;
            n2 = n + 1;
            if (dir == 1) {
                SEND_NRM(n);
                SEND_VTX(p01);
                SEND_VTX(p00);
                SEND_VTX(p10);
                SEND_NRM(n2);
                SEND_VTX(p10);
                SEND_VTX(p11);
                SEND_VTX(p01);
            } else if (dir == -1) {
                SEND_NRM(n);
                SEND_VTX(p10);
                SEND_VTX(p11);
                SEND_VTX(p00);
                SEND_NRM(n2);
                SEND_VTX(p01);
                SEND_VTX(p00);
                SEND_VTX(p11);
            }
            n += 2;
        }
        dir = -dir;
    }
    G3_End();
}

