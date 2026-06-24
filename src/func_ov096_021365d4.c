struct Vec3 { int x, y, z; };
void _Z14ApproachLinearRiii(int* dst, int target, int step);
void func_ov096_021358c8(char* c);
int func_ov096_02135838(char* c);
extern unsigned char DecIfAbove0_Byte(unsigned char* p);
void* _ZN5Actor5SpawnEjjRK7Vector3PK10Vector3_16ii(unsigned int a, unsigned int b, const struct Vec3* pos, const void* rot, int e, int f);
void func_ov096_02136928(char* c, int n);
char* func_ov096_021357b4(char* c);
void _ZN5Actor9UpdatePosEP12CylinderClsn(void* thiz, void* clsn);
void func_ov096_02135e2c(int* self, void* clsn);
void func_ov096_02135948(char* c);
void _ZN12CylinderClsn5ClearEv(void* p);
void _ZN12CylinderClsn6UpdateEv(void* p);
int _ZNK12WithMeshClsn10IsOnGroundEv(void* p);
void _ZN8Particle20RunningSlidingDustAtE5Fix12IiES1_S1_(int a, int b, int c);

int func_ov096_021365d4(char* c) {
    int t = *(int*)(c + 0x80);
    int v;
    _Z14ApproachLinearRiii(&t, 0x1000, 0x12c);
    v = t;
    *(int*)(c + 0x80) = v;
    *(int*)(c + 0x84) = v;
    *(int*)(c + 0x88) = v;
    *(short*)(c + 0x3aa) = *(short*)(c + 0x3aa) + 0x500;
    *(int*)(c + 0x378) = 0;
    *(int*)(c + 0x37c) = 0;
    *(int*)(c + 0x380) = 0;
    func_ov096_021358c8(c);
    if (func_ov096_02135838(c) < 4) {
        if (DecIfAbove0_Byte((unsigned char*)(c + 0x3ac)) == 0) {
            void* sp = _ZN5Actor5SpawnEjjRK7Vector3PK10Vector3_16ii(
                0xf1, *(unsigned int*)(c + 4), (struct Vec3*)(c + 0x5c), c + 0x8c,
                *(signed char*)(c + 0xcc), -1);
            if (sp != 0) {
                *(void**)(c + 0x394) = sp;
                func_ov096_02136928(c, 4);
                if (func_ov096_02135838(c) >= 3) {
                    *(unsigned char*)(func_ov096_021357b4(c) + 0x3a8) = 0;
                }
            }
        }
    }
    _ZN5Actor9UpdatePosEP12CylinderClsn(c, c + 0x14c);
    func_ov096_02135e2c((int*)c, c + 0x180);
    func_ov096_02135948(c);
    _ZN12CylinderClsn5ClearEv(c + 0x14c);
    _ZN12CylinderClsn6UpdateEv(c + 0x14c);
    if (_ZNK12WithMeshClsn10IsOnGroundEv(c + 0x180) != 0) {
        _ZN8Particle20RunningSlidingDustAtE5Fix12IiES1_S1_(*(int*)(c + 0x5c), *(int*)(c + 0x60), *(int*)(c + 0x64));
    }
    return 1;
}
