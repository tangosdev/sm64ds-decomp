//cpp
struct V16 { short x, y, z; };
struct V3 { int x, y, z; };
extern "C" {
extern void _ZN9Animation7AdvanceEv(void*);
extern int _ZNK9Animation12WillHitFrameEi(void*, int);
extern void* _ZN5Actor5SpawnEjjRK7Vector3PK10Vector3_16ii(unsigned int, unsigned int, V3*, V16*, int, int);
extern int _ZN5Actor13DistToCPlayerEv(void*);
extern void func_0201267c(unsigned int, void*);
extern int _ZN9Animation8FinishedEv(void*);
extern void _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(void*, void*, int, int, unsigned int);
extern short data_02082214[];
extern void* data_ov080_021283e8[];
extern void* data_ov080_021283d0[];
}

extern "C" void func_ov080_02123a34(char* c)
{
    _ZN9Animation7AdvanceEv(c + 0x124);
    if (_ZNK9Animation12WillHitFrameEi(c + 0x124, 0xa)) {
        V16 v16;
        V3 pos;
        unsigned short ax, ay;
        void* a;
        int d;
        short s;
        int idx;
        int px, py, pz;
        V16* pAng = &v16;

        ax = *(unsigned short*)(c + 0x8c);
        ay = *(unsigned short*)(c + 0x8e);
        *(volatile short*)&v16.y = (short)ay;
        *(volatile short*)&v16.x = (short)ax;
        {
            unsigned short z = *(unsigned short*)(c + 0x90);
            pAng->z = z;
            short yv = (short)v16.y;
            px = *(int*)(c + 0x5c);
            pos.x = px;
            yv = (short)(yv + 0x400);
            py = *(int*)(c + 0x60);
            pos.y = py;
            pz = *(int*)(c + 0x64);
            pAng->y = yv;
            pos.z = pz;
            pos.y = py + 0xa000;
        }

        idx = (unsigned short)(short)(*(short*)(c + 0x8e) - 0x4000) >> 4;
        s = data_02082214[idx << 1];
        pos.x = s * (short)0x50 + px;

        idx = (unsigned short)(short)(*(short*)(c + 0x8e) - 0x4000) >> 4;
        s = data_02082214[(idx << 1) + 1];
        pos.z = s * (short)0x50 + pz;

        a = _ZN5Actor5SpawnEjjRK7Vector3PK10Vector3_16ii(
            py ? 0x137u : 0x137u, 0, &pos, pAng, *(signed char*)(c + 0xcc), -1);
        d = _ZN5Actor13DistToCPlayerEv(c);
        if (d >= 0x258000)
            d = 0x258000;
        *(int*)((char*)a + 0x98) = 0x1e000;
        *(int*)((char*)a + 0xa4) = 0;
        *(int*)((char*)a + 0xa8) = (d << 2) / 100 + 0x4000;
        *(int*)((char*)a + 0xac) = 0;
        func_0201267c(0xd2, c + 0x74);
    }

    if (_ZN9Animation8FinishedEv(c + 0x124) == 0)
        return;

    if (_ZN5Actor13DistToCPlayerEv(c) < 0x3e8000) {
        *(int*)(c + 0x17c) = 5;
        _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(c + 0xd4, data_ov080_021283e8[1], 0x40000000, 0x1000, 0);
    } else {
        *(int*)(c + 0x17c) = 4;
        _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(c + 0xd4, data_ov080_021283d0[1], 0x40000000, 0x1000, 0);
    }
}
