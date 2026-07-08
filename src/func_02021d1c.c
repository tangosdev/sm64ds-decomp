// NONMATCHING: different op / idiom (div=22). Logic verified correct vs ROM; not
// byte-matchable from C at mwccarm 1.2/sp2p3 (see notes/matching-style.md).
// Counts as decompiled, not matched.
typedef struct { short x, y, z; } Vec3s;
extern void *_ZN8Particle7Manager9AddSystemEiR7Vector3(void *thiz, int id, void *pos);
extern void *PARTICLE_SYS_TRACKER;

struct Bf74 {
    unsigned int a : 6;
    unsigned int b : 6;
    unsigned int c : 6;
};

int func_02021d1c(char *self, int a1, int id, void *pos, Vec3s *p5, void **p6)
{
    void *sys = *(void **)((char *)*(void **)((char *)*(void **)((char *)PARTICLE_SYS_TRACKER + 4) + 0x1c) + (id << 5));
    *(int *)sys = *(int *)sys & ~0x4000;
    *(void **)(self + 0xc) = _ZN8Particle7Manager9AddSystemEiR7Vector3(
        *(void **)((char *)PARTICLE_SYS_TRACKER + 4), id, pos);
    if (*(void **)(self + 0xc) == 0)
        return 0;
    if (id == 0x52 || id == 0x50) {
        struct Bf74 *bf = (struct Bf74 *)(*(char **)(self + 0xc) + 0x74);
        if (bf->a <= 5) {
            if (bf->b >= 5) {
                bf->c = 5;
            }
        }
    }
    *(unsigned short *)(self + 8) = *(unsigned short *)((char *)sys + 0x28);
    *(unsigned short *)((char *)sys + 0x28) = 0;
    if (p5 != 0) {
        char *r = *(char **)(self + 0xc);
        *(short *)(r + 0x3c) = p5->x;
        *(short *)(r + 0x3e) = p5->y;
        *(short *)(r + 0x40) = p5->z;
    }
    if (p6 != 0) {
        (*(void (***)(void *))p6)[0](*(void **)(self + 0xc));
    }
    *(int *)self = a1;
    *(int *)(self + 4) = id;
    *(unsigned char *)(self + 0xa) = 1;
    *(void ***)(self + 0x10) = p6;
    *(int *)(self + 0x14) = 0;
    *(int *)(self + 0x18) = 0;
    return 1;
}
