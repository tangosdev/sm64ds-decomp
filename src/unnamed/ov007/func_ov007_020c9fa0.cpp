//cpp
#include "types.h"
extern "C" {
extern void func_ov007_020c8440(void *p);
extern void func_ov007_020c8098(void *p);
extern void func_ov007_020c78dc(void *p);
extern int func_ov007_020c3d1c(void *p);

/* The 0x70-byte swing func_ov007_020ca010 builds; names as in
 * func_ov007_020c9c24 and func_ov007_020c9a84, only the slots this file
 * frees. */
struct Ov007Swing {
    char pad_00[0x18];
    Vector3 *chainPositions; /* 0x18: one position per chain point (func_ov007_020ca010) */
    void *chain;             /* 0x1c: func_ov007_020c798c point list */
    char pad_20[0x30];
    void *xPoints[2]; /* 0x50: the side points spread along x (func_ov007_020c844c) */
    void *yPoints[2]; /* 0x58: spread along y */
    void *xLinks[2];  /* 0x60: xPoints[i] to the mesh (func_ov007_020c9d90) */
    void *yLinks[2];  /* 0x68 */
};

typedef char Ov007Swing_size_must_be_0x70[sizeof(Ov007Swing) == 0x70 ? 1 : -1];

/* Frees the swing: its side points (func_ov007_020c8440) and links
 * (func_ov007_020c8098), the chain (func_ov007_020c78dc), the chain's
 * positions and the swing itself. c keeps its char * spelling; `self` is the
 * typed view. */
void func_ov007_020c9fa0(char *c) {
    Ov007Swing *self = (Ov007Swing *)c;
    int i;
    for (i = 0; i < 2; i++) {
        func_ov007_020c8440(self->xPoints[i]);
        func_ov007_020c8440(self->yPoints[i]);
        func_ov007_020c8098(self->xLinks[i]);
        func_ov007_020c8098(self->yLinks[i]);
    }
    func_ov007_020c78dc(self->chain);
    func_ov007_020c3d1c(self->chainPositions);
    func_ov007_020c3d1c(c);
}
}
