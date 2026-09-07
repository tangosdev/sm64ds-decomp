// @symbol func_ov007_020c6e68
#include "common.h"

/* Builds the per-group render state for path group `idx`: allocates the
 * group's node, three per-point arrays (position source, position copy, and
 * the per-point handles), then fills the copies point by point and appends two
 * trailing handles.
 *
 * The flag word starts at 1 and gains bit 2 when the group's own scale
 * (func_ov007_020c249c, times the owner's Q12 factor at ->0x1c->0x1c) is at
 * least the horizontal distance from the group's first point to its last one.
 * Byte-exact under 2004/b56. The projected scale has to be computed BEFORE
 * the dx/dz differences: with the multiply after them the compiler colours the
 * two pointer loads the other way round and the whole block moves. */

extern int func_ov007_020c249c(void *grp);
extern int func_020531a4(int a);
extern void *func_ov007_020c798c(int a, int b, void *c, int d);
extern int func_ov007_020c3df4(int a, void *b);
extern void *func_ov007_020c80a4(void);
extern void func_ov007_020c8010(void *a, int *b, int *c);
extern void func_ov007_020bfe4c(char *thiz, int a, int b, int c, Vector3 *out);
extern void func_ov007_020c7804(void *self);

#define FIXMUL(a, b) ((int)(((s64)(a) * (b) + 0x800) >> 12))

void func_ov007_020c6e68(char *self, int idx)
{
    char *grp;
    char *fp;
    int flag;
    int count;
    int i;
    void *sb;
    int scale;
    int *px, *pz;
    int last;
    int dx, dz, proj, lensq;
    char *owner;

    grp = (*(char ***)(self + 0x38))[idx];
    fp = *(char **)(self + 0x1c);
    count = *(u16 *)(grp + 8);
    flag = 1;
    scale = func_ov007_020c249c(grp);

    owner = *(char **)(self + 0x1c);
    px = *(int **)(grp + 0x24);
    pz = *(int **)(grp + 0x28);
    last = count - 1;
    proj = FIXMUL(scale, *(int *)(owner + 0x1c));
    dx = px[0] - px[last];
    dz = pz[0] - pz[last];
    lensq = FIXMUL(dx, dx) + FIXMUL(dz, dz);
    if (func_020531a4(lensq) <= proj)
        flag |= 2;

    {
        void *node = func_ov007_020c798c(2, *(u16 *)(grp + 8), *(void **)fp, flag);
        flag = count * 0xc;
        (*(void ***)(self + 0x3c))[idx] = node;
    }
    sb = (*(void ***)(self + 0x3c))[idx];

    (*(int **)(self + 0x28))[idx] = func_ov007_020c3df4(0, (void *)flag);
    (*(int **)(self + 0x2c))[idx] = func_ov007_020c3df4(0, (void *)flag);
    (*(int **)(self + 0x34))[idx] = func_ov007_020c3df4(0, (void *)(count << 2));

    i = 0;
    if (count > 0) {
        flag = 0;
        do {
            void *p = func_ov007_020c80a4();
            ((void **)((*(void ***)(self + 0x34))[idx]))[i] = p;
            func_ov007_020c8010(p, *(int **)(self + 0x6c), (*(int ***)sb)[i]);
            {
                int base = (*(int **)(self + 0x28))[idx];
                *((*(int ***)sb)[i]) = base + flag;
            }
            {
                char *a8 = *(char **)(self + 0xa8);
                int base = (*(int **)(self + 0x28))[idx];
                Vector3 *out = (Vector3 *)(base + flag);
                func_ov007_020bfe4c(a8, (*(int **)(grp + 0x24))[i], (*(int **)(grp + 0x28))[i],
                                    -(*(int *)(a8 + 0x2c)), out);
            }
            {
                int base_d = (*(int **)(self + 0x2c))[idx];
                int base_s = (*(int **)(self + 0x28))[idx];
                Vector3 *d = (Vector3 *)(base_d + flag);
                Vector3 *s = (Vector3 *)(base_s + flag);
                flag += 0xc;
                i++;
                *d = *s;
            }
        } while (i < count);
    }

    (*(void ***)(self + 0x40))[idx] = func_ov007_020c80a4();
    (*(void ***)(self + 0x44))[idx] = func_ov007_020c80a4();
    func_ov007_020c7804(sb);
}
