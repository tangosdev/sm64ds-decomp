typedef unsigned short u16;
typedef long long s64;

struct Vec3 { int x, y, z; };

extern int func_ov007_020c249c(void *grp);
extern int func_020531a4(int a);
extern void *func_ov007_020c798c(void *a, int count, void *b, int flags);
extern void *func_ov007_020c3df4(int heap, int size);
extern void *func_ov007_020c80a4(void);
extern void func_ov007_020c8010(void *a, void *b, void *c);
extern void func_ov007_020bfe4c(char *thiz, int x, int z, int y, struct Vec3 *out);
extern void func_ov007_020c7804(void *obj);

void func_ov007_020c6e68(char *self, int idx)
{
    int i;
    int off;
    char *grp;
    char *cfg;
    int flags;
    int count;
    int total;
    int dx, dz;
    int proj, lensq;
    void *node;
    void *path;

    grp = (*(char ***)(self + 0x38))[idx];
    cfg = *(char **)(self + 0x1c);
    count = *(u16 *)(grp + 8);
    flags = 1;
    total = func_ov007_020c249c(grp);

    proj = (int)(((s64)total * (*(int *)(*(char **)(self + 0x1c) + 0x1c)) + 0x800) >> 12);
    dx = (*(int **)(grp + 0x24))[0] - (*(int **)(grp + 0x24))[count - 1];
    dz = (*(int **)(grp + 0x28))[0] - (*(int **)(grp + 0x28))[count - 1];
    lensq = (int)(((s64)dx * dx + 0x800) >> 12) + (int)(((s64)dz * dz + 0x800) >> 12);

    if (func_020531a4(lensq) <= proj)
        flags |= 2;

    node = func_ov007_020c798c((void *)2, *(u16 *)(grp + 8), *(void **)cfg, flags);
    (*(void ***)(self + 0x3c))[idx] = node;
    path = (*(void ***)(self + 0x3c))[idx];

    (*(void ***)(self + 0x28))[idx] = func_ov007_020c3df4(0, count * 0xc);
    (*(void ***)(self + 0x2c))[idx] = func_ov007_020c3df4(0, count * 0xc);
    (*(void ***)(self + 0x34))[idx] = func_ov007_020c3df4(0, count << 2);

    i = 0;
    if (count > 0) {
        off = 0;
        do {
            void *p = func_ov007_020c80a4();
            ((void **)((*(void ***)(self + 0x34))[idx]))[i] = p;
            func_ov007_020c8010(p, *(void **)(self + 0x6c), (*(void ***)path)[i]);

            *(struct Vec3 **)((*(void ***)path)[i]) =
                (struct Vec3 *)((*(char ***)(self + 0x28))[idx] + off);

            {
                char *a8 = *(char **)(self + 0xa8);
                struct Vec3 *out = (struct Vec3 *)((*(char ***)(self + 0x28))[idx] + off);
                func_ov007_020bfe4c(a8, (*(int **)(grp + 0x24))[i], (*(int **)(grp + 0x28))[i],
                                    -(*(int *)(a8 + 0x2c)), out);
            }

            {
                struct Vec3 *d = (struct Vec3 *)((*(char ***)(self + 0x2c))[idx] + off);
                struct Vec3 *s = (struct Vec3 *)((*(char ***)(self + 0x28))[idx] + off);
                *d = *s;
            }
            off += 0xc;
            i++;
        } while (i < count);
    }

    (*(void ***)(self + 0x40))[idx] = func_ov007_020c80a4();
    (*(void ***)(self + 0x44))[idx] = func_ov007_020c80a4();
    func_ov007_020c7804(path);
}
