//cpp
struct Callback {
    virtual void Run(void *arg);
};

typedef struct { short x, y, z; } Vec3s;

extern "C" {
extern void *_ZN8Particle7Manager9AddSystemEiR7Vector3(void *thiz, int id, void *pos);
extern void *data_0209ee74;
int func_02021d1c(char *self, int a1, int id, void *pos, Vec3s *p5, Callback *p6);
}

struct Bf74 {
    unsigned int a : 6;
    unsigned int b : 6;
    unsigned int c : 6;
};

int func_02021d1c(char *self, int a1, int id, void *pos, Vec3s *p5, Callback *p6)
{
    void *sys = *(void **)((char *)*(void **)((char *)*(void **)((char *)data_0209ee74 + 4) + 0x1c) + (id << 5));
    *(int *)sys = *(int *)sys & ~0x4000;
    *(void **)(self + 0xc) = _ZN8Particle7Manager9AddSystemEiR7Vector3(
        *(void **)((char *)data_0209ee74 + 4), id, pos);
    void *e = *(void **)(self + 0xc);
    if (e == 0)
        return 0;
    if (id == 0x52 || id == 0x50) {
        if (((struct Bf74 *)((char *)e + 0x74))->a <= 5) {
            if (((struct Bf74 *)((char *)e + 0x74))->b >= 5) {
                int *cw = (int *)(((unsigned long long)((char *)e + 0x74)) & 0xFFFFFFFFFFFFFFFFULL);
                *cw = (*cw & ~0x3f000) | 0x5000;
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
        p6->Run(*(void **)(self + 0xc));
    }
    *(int *)self = a1;
    *(int *)(self + 4) = id;
    *(unsigned char *)(self + 0xa) = 1;
    *(Callback **)(self + 0x10) = p6;
    *(int *)(self + 0x14) = 0;
    *(int *)(self + 0x18) = 0;
    return 1;
}
