#include "types.h"

/* The 0x70-byte swing func_ov007_020ca010 builds; names as in
 * func_ov007_020c9c24 and func_ov007_020c9a84, only the slots this file
 * writes. */
typedef struct Ov007Swing {
    char pad_00[0xc];
    /* 0x0c: func_ov007_020c9d90 binds the swing to the mesh point nearest
     * it (meshPoint) */
    Vector3 anchor;
    char pad_18[8];
    /* 0x20: the side points' positions; func_ov007_020c9d90 ties each to the
     * mesh point nearest it */
    Vector3 xPointPos[2];
    Vector3 yPointPos[2]; /* 0x38 */
} Ov007Swing;

typedef char Ov007Swing_size_must_be_0x50[sizeof(Ov007Swing) == 0x50 ? 1 : -1];

/* Places the swing at pos: the anchor there, the x side pair w apart along
 * x and the y side pair h apart along y, both centred on it. */
void func_ov007_020c9f10(Ov007Swing *self, Vector3 *pos, int w, int h) {
    Vector3 *pe = &self->yPointPos[1];
    Vector3 *pd = &self->yPointPos[0];
    Vector3 *pc = &self->xPointPos[1];
    int *qd = &self->yPointPos[0].y;
    int *qe = &self->yPointPos[1].y;
    *pe = *pos;
    *pd = *pe;
    *pc = *pd;
    {
        Vector3 *pb = &self->xPointPos[0];
        *pb = *pc;
        self->anchor = *pb;
        pb->x -= w / 2;
        pc->x += w / 2;
        *qd -= h / 2;
        *qe += h / 2;
    }
}
