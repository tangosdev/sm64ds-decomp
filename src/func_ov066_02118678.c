extern int Vec3_ApproachHorz(void* out, void* a, int maxStep);
extern int Vec3_HorzDist(void* a, void* b);
extern int _ZN9Animation8FinishedEv(void* anim);
extern void _ZN16MeshColliderBase6EnableEP5Actor(void* m, void* actor);
extern void func_ov066_02119454(void* c, void* p);
extern void func_ov066_02116ac4(void* c, int v);
extern void func_02012694(int a, void* p);

extern unsigned char data_ov066_0211ae04[1];
extern unsigned char data_ov066_0211ae0c[1];
extern int data_ov066_0211b06c;

int func_ov066_02118678(char* c)
{
    if (*(int*)(c + 0x494) == 0) {
        if (data_ov066_0211ae04[0] == 2) {
            *(int*)(c + 0x3bc) = 0x1000;
            if (*(int*)(c + 0x49c) == 1) {
                *(int*)(c + 0xa8) = 0x2d000;
                *(int*)(c + 0x9c) = -0x2000;
            } else {
                *(int*)(c + 0xa8) = 0xa000;
                *(int*)(c + 0x9c) = -0x800;
            }
            *(int*)(c + 0x494) = 1;
            func_02012694(0x144, c + 0x74);
        }
        return 1;
    }

    Vec3_ApproachHorz(c + 0x5c, c + 0x4a4, 0x14000);
    if (*(int*)(c + 0x9c) != 0) {
        int v = *(int*)(c + 0x4a8);
        if (v >= *(int*)(c + 0x60)) {
            *(int*)(c + 0x60) = v;
            *(int*)(c + 0xa8) = 0;
            *(int*)(c + 0x9c) = 0;
            func_ov066_02116ac4(c, 0x7d0000);
        }
    }

    if (*(int*)(c + 0x9c) == 0
        && Vec3_HorzDist(c + 0x5c, c + 0x4a4) <= 0x14000
        && _ZN9Animation8FinishedEv(c + 0x3b0)) {
        _ZN16MeshColliderBase6EnableEP5Actor(c + 0x674, c);
        data_ov066_0211ae0c[0] |= *(int*)(c + 0x49c);
        func_ov066_02119454(c, &data_ov066_0211b06c);
    }
    return 1;
}
