//cpp
struct Vector3;
struct dActor_c;

struct dBgCh_Gnd {
    char pad[0x44];
    int result;       // offset 0x44
    char pad2[0xc];
    dBgCh_Gnd();
    ~dBgCh_Gnd();
    void SetObjAndPos(const Vector3 &pos, dActor_c *a);
    int DetectClsn();
};

extern "C" int _ZNK10dBgCh_Actr10IsOnGroundEv(int self);
extern "C" void _ZN8dActor_c19DropShadowRadHeightER11ShadowModelR9Matrix4x35Fix12IiES5_j(
    int self, int sm, int mat, int fix, int t, unsigned int j);

extern "C" void func_ov102_0214b444(int c)
{
    int v;
    int b = (*(int*)(c + 0xb0) & 0x40000) != 0;
    if (b)
        return;

    v = *(int*)(c + 0x60);
    if (_ZNK10dBgCh_Actr10IsOnGroundEv(c + 0x144) == 0) {
        dBgCh_Gnd rg;
        rg.SetObjAndPos(*(const Vector3*)(c + 0x5c), (dActor_c*)0);
        if (rg.DetectClsn() != 0)
            v = rg.result;
    }

    {
        int b2 = (*(int*)(c + 0xb0) & 0x4000) != 0;
        if (b2) {
            int *p = *(int**)(c + 0x390);
            if (p != 0) {
                if (*(int*)((int)p + 0xc8) != 0)
                    v = *(int*)((int)p + 0x60);
            }
        }
    }

    *(int*)(c + 0x3b8) = *(int*)(c + 0x5c) >> 3;
    *(int*)(c + 0x3bc) = v >> 3;
    *(int*)(c + 0x3c0) = *(int*)(c + 0x64) >> 3;

    _ZN8dActor_c19DropShadowRadHeightER11ShadowModelR9Matrix4x35Fix12IiES5_j(
        c, c + 0x364, c + 0x394, *(int*)(c + 0x80) * 0x50, 0x1e000, 0xf);
}
