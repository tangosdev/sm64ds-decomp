//cpp
extern "C" {
extern void Matrix4x3_FromRotationY(void* m, int angle);
extern void MulVec3Mat4x3(void* out, void* m, void* in);
extern void _ZN25MovingCylinderClsnWithPos21SetPosRelativeToActorERK7Vector3(void*, void*);
extern void* _ZN5Actor10FindWithIDEj(unsigned int);
extern void _ZN6Player4HurtERK7Vector3j5Fix12IiEjjj(void*, void*, unsigned int, int, unsigned int, unsigned int, unsigned int);
extern int MATRIX_SCRATCH_PAPER[];
struct V3 { int x, y, z; };

void func_ov090_02133710(char* c) {
    struct V3 v;
    struct V3 v2;
    void* a;
    *(int*)(((int)c + 0x384) & 0xFFFFFFFFFFFFFFFF) += 1;
    if (*(int*)(c + 0x384) > 2) *(int*)(c + 0x384) = 0;
    v.x = 0;
    v.y = 0;
    v.z = 0;
    if (*(int*)(c + 0x384) == 1) {
        v.z = 0x8c000;
    } else if (*(int*)(c + 0x384) == 2) {
        v.z = -0x8c000;
    }
    Matrix4x3_FromRotationY(MATRIX_SCRATCH_PAPER, 0);
    MulVec3Mat4x3(&v, MATRIX_SCRATCH_PAPER, c + 0x374);
    _ZN25MovingCylinderClsnWithPos21SetPosRelativeToActorERK7Vector3(c + 0x110, c + 0x374);
    if (*(unsigned int*)(c + 0x134) == 0) return;
    a = _ZN5Actor10FindWithIDEj(*(unsigned int*)(c + 0x134));
    {
        int b = (*(unsigned short*)((char*)a + 0xc) == 0xbf);
        if (b == 0) return;
    }
    if (*(unsigned char*)((char*)a + 0x6fb)) return;
    v2.x = *(int*)(c + 0x5c);
    v2.y = *(int*)(c + 0x60);
    v2.z = *(int*)(c + 0x64);
    _ZN6Player4HurtERK7Vector3j5Fix12IiEjjj(a, &v2, 3, 0xc000, 1, 0, 1);
}
}
