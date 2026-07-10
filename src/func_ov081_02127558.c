typedef unsigned char u8;
typedef short s16;

extern int _Z15ApproachLinear2Riii(int* p, int a, int b);
extern void _ZN18MovingCylinderClsn4InitEP5Actor5Fix12IiES3_jj(void* thiz, void* actor, int a, int b, unsigned int c, unsigned int d);
extern int _ZN5Actor13DistToCPlayerEv(void* thiz);
extern void func_0201267c(int a, void* p);
extern void func_ov081_0212777c(void* c, int n);
extern void func_ov081_02126758(void* c);
extern void _ZN12CylinderClsn5ClearEv(void* thiz);
extern void _ZN12CylinderClsn6UpdateEv(void* thiz);

int func_ov081_02127558(char* c)
{
    u8* pstate;
    int* pflg;
    int off;
    char* base;

    switch (*(u8*)(c + 0x3f1)) {
    case 0:
        if (_Z15ApproachLinear2Riii((int*)(c + 0x3f0), 1, 2) != 0) {
            _ZN18MovingCylinderClsn4InitEP5Actor5Fix12IiES3_jj(c + 0x1b0, c, 0x4b000, 0x73000, 0x200000, 0);
            pflg = (int*)(((int)c + 0xb0) & 0xFFFFFFFFFFFFFFFFLL);
            *pflg = *pflg & ~0x10000000;
            pstate = (u8*)(((int)c + 0x3f1) & 0xFFFFFFFFFFFFFFFFLL);
            *pstate = *pstate + 1;
        }
        break;
    case 1:
        if (_ZN5Actor13DistToCPlayerEv(c) < 0x1f4000) {
            pflg = (int*)(((int)c + 0xb0) & 0xFFFFFFFFFFFFFFFFLL);
            *pflg = *pflg | 0x10000000;
            func_0201267c(0x76, c + 0x74);
            pstate = (u8*)(((int)c + 0x3f1) & 0xFFFFFFFFFFFFFFFFLL);
            *pstate = *pstate + 1;
        }
        break;
    case 2:
        if (_Z15ApproachLinear2Riii((int*)(c + 0x3f0), 0x1f, 2) != 0) {
            _ZN18MovingCylinderClsn4InitEP5Actor5Fix12IiES3_jj(c + 0x1b0, c, 0x4b000, 0x73000, 0x200000, 0x6eff0);
            func_ov081_0212777c(c, 3);
        }
        break;
    }

    off = 0x300;
    base = c + off;
    *(s16*)(base + 0xec) += 0xc00;
    func_ov081_02126758(c);
    _ZN12CylinderClsn5ClearEv(c + 0x1b0);
    _ZN12CylinderClsn6UpdateEv(c + 0x1b0);
    return 1;
}
