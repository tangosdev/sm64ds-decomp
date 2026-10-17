/* FLOOR under 1.2/sp2p3 — PTMF staging wall + conditional func_0204302c (size 0x84 vs 0x78).
 * ROM: push {r4,r5,lr}; sub sp,#0xc; ldrh r4,[r0,#0xc]; stage PMFs; bl Process;
 * if (r==1) func_0204302c(r4); return r;
 */
typedef struct { int a; int b; } PMF;
extern int _ZN7fBase_c7ProcessEMS_FivEMS_FbvEMS_FvjE(void *self, PMF a, PMF b, PMF c);
extern PMF data_02099e84;
extern PMF data_02099e7c;
extern PMF data_02099e8c;
extern void func_0204302c(int id);

/* fBase_c (include/fBase_c.h); only the actor ID read here. It is taken
 * before Process runs and handed to the func_0204302c hook on a result
 * of 1. */
typedef struct fBaseView {
    unsigned char pad_00[0xc];
    unsigned short actorID; /* 0x0c */
} fBaseView;

int func_020432e4(void *self)
{
    unsigned short id = ((fBaseView *)self)->actorID;
    int r = _ZN7fBase_c7ProcessEMS_FivEMS_FbvEMS_FvjE(
        self, data_02099e8c, data_02099e7c, data_02099e84);
    if (r == 1)
        func_0204302c(id);
    return r;
}
