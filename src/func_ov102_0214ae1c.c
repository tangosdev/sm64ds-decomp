typedef struct Vec3 { int x, y, z; } Vec3;

extern int _ZN5Actor5SpawnEjjRK7Vector3PK10Vector3_16ii(unsigned int id, unsigned int p, Vec3 *pos, void *rot, int a, int b);
extern void func_0201267c(int a, void *b);
extern void _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(unsigned int id, int x, int y, int z);
extern void func_ov002_020ef228(void *a, void *b);
extern void _ZN6Player9DropActorEv(void *self);
extern void _ZN6Player4HurtERK7Vector3j5Fix12IiEjjj(void *self, Vec3 *v, unsigned int a, int fix, unsigned int b, unsigned int d, unsigned int e);
extern void *_ZN5Actor10FindWithIDEj(unsigned int id);
extern void func_ov002_020d718c(void *a);
extern void func_ov102_0214baa0(void *c);

void func_ov102_0214ae1c(char *c) {
    Vec3 v;
    int y, z, w;
    *(unsigned int *)(((int)c + 0x128)) &= ~0x8000;
    y = *(int *)(c + 0x60);
    z = *(int *)(c + 0x64);
    w = y + 0x78000;
    v.x = *(int *)(c + 0x5c);
    v.y = w;
    v.z = z;
    if (*(unsigned char *)(c + 0x108)) {
        _ZN5Actor5SpawnEjjRK7Vector3PK10Vector3_16ii(0x120, 2, &v, 0, *(signed char *)(c + 0xcc), -1);
        *(unsigned char *)(c + 0x108) = 0;
    }
    func_0201267c(0x10c, c + 0x74);
    v.y = *(int *)(c + 0x60) + 0x64000;
    _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(0x14, v.x, v.y, v.z);
    _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(0x15, v.x, v.y, v.z);
    _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(0x16, v.x, v.y, v.z);
    _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(0x17, v.x, v.y, v.z);
    _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(0x18, v.x, v.y, v.z);
    func_ov002_020ef228(c + 0x144, c);
    if (*(void **)(c + 0x390)) {
        Vec3 w;
        _ZN6Player9DropActorEv(*(void **)(c + 0x390));
        w.x = *(int *)(c + 0x5c);
        w.y = *(int *)(c + 0x60);
        w.z = *(int *)(c + 0x64);
        _ZN6Player4HurtERK7Vector3j5Fix12IiEjjj(*(void **)(c + 0x390), &w, 2, 0xc000, 1, 0, 1);
        *(void **)(c + 0x390) = 0;
    } else if (*(unsigned int *)(c + 0x134)) {
        void *a = _ZN5Actor10FindWithIDEj(*(unsigned int *)(c + 0x134));
        if (a) {
            if (*(unsigned int *)(c + 0x130) & 0x400000) {
                Vec3 w;
                w.x = *(int *)(c + 0x5c);
                w.y = *(int *)(c + 0x60);
                w.z = *(int *)(c + 0x64);
                _ZN6Player4HurtERK7Vector3j5Fix12IiEjjj(a, &w, *(unsigned int *)(c + 0x3e0), 0xc000, 1, 0, 1);
            }
        }
    }
    if (*(void **)(c + 0xd0)) {
        Vec3 w;
        w.x = *(int *)(c + 0x5c);
        w.y = *(int *)(c + 0x60);
        w.z = *(int *)(c + 0x64);
        _ZN6Player4HurtERK7Vector3j5Fix12IiEjjj(*(void **)(c + 0xd0), &w, *(unsigned int *)(c + 0x3e0), 0xc000, 1, 0, 1);
        func_ov002_020d718c(*(void **)(c + 0xd0));
        *(void **)(c + 0xd0) = 0;
    }
    func_ov102_0214baa0(c);
}
