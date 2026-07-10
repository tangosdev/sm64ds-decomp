extern int _ZNK7PathPtr8NumNodesEv(void* p);
extern void _ZNK7PathPtr7GetNodeER7Vector3j(void* p, void* out, unsigned int i);
extern int Vec3_Dist(void* a, void* b);
extern short Vec3_HorzAngle(void* a, void* b);
extern int _Z14ApproachLinearRsss(short* p, short t, short s);
extern void _ZN5Actor9UpdatePosEP12CylinderClsn(void* self, void* clsn);
extern int func_ov019_0211140c(int* self, void* clsn);
extern void func_0201267c(int a, void* p);
extern int func_ov019_021122dc(void* c, int s);

#define LB(off) (*(unsigned char *)(((int)c + (off)) & 0xFFFFFFFFFFFFFFFF))

#pragma opt_common_subs off
int func_ov019_021117a8(char* c) {
    int node[3];
    switch (*(unsigned char*)(c+0x38f)) {
    case 0: {
        int n = _ZNK7PathPtr8NumNodesEv(c+0x364);
        _ZNK7PathPtr7GetNodeER7Vector3j(c+0x364, node, n - 1);
        *(int*)(c+0x380) = Vec3_Dist(c+0x5c, node);
        *(short *)((char *)(((int)c + 0x300) & 0xFFFFFFFFFFFFFFFF) + 0x8c) =
            Vec3_HorzAngle(c+0x5c, node);
        _Z14ApproachLinearRsss(
            (short*)(c+0x8e),
            *(short *)((char *)(((int)c + 0x300) & 0xFFFFFFFFFFFFFFFF) + 0x8c),
            0x200);
        *(short*)(c+0x94) = *(short*)(c+0x8e);
        if (*(int*)(c+0x380) < *(int*)(c+0x98)) {
            *(int*)(c+0x98) = *(int*)(c+0x380);
            LB(0x38f) = LB(0x38f) + 1;
        }
        _ZN5Actor9UpdatePosEP12CylinderClsn(c, c+0x174);
        func_ov019_0211140c((int*)c, c+0x1a8);
        {
            unsigned int s = ((unsigned int)*(int*)(c+0x12c) << 4) >> 0x10;
            if (s == 9 || s == 0x15) {
                func_0201267c(0xde, c+0x74);
            }
        }
        break;
    }
    case 1: {
        short ang = Vec3_HorzAngle(c+0x5c, *(char**)(c+0x378) + 0x5c);
        if (_Z14ApproachLinearRsss((short*)(c+0x8e), ang, 0x514) != 0) {
            func_ov019_021122dc(c, 5);
        }
        break;
    }
    }
    {
        unsigned int s = ((unsigned int)*(int*)(c+0x12c) << 4) >> 0x10;
        if (s == 9 || s == 0x15) {
            func_0201267c(0xf3, c+0x74);
        }
    }
    return 1;
}
