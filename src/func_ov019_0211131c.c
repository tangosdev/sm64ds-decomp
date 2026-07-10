struct Vec3 { int x, y, z; };
extern void _Z11UpdateAngleRssis(short* a, short b, int n, short s);
extern int _ZNK7PathPtr8NumNodesEv(void* p);
extern void _ZNK7PathPtr7GetNodeER7Vector3j(void* p, struct Vec3* v, unsigned int i);
extern short Vec3_HorzAngle(struct Vec3* a, struct Vec3* b);

int func_ov019_0211131c(char* c) {
    _Z11UpdateAngleRssis(
        (short*)(c + 0x94),
        *(short *)((char *)(((int)c + 0x300) & 0xFFFFFFFFFFFFFFFF) + 0x8c),
        2, 0x600);
    int n = *(int*)(c + 0x36c);
    if (n >= _ZNK7PathPtr8NumNodesEv(c + 0x364) - 2) {
        *(short*)(c + 0x8e) = *(short*)(c + 0x94);
    } else {
        struct Vec3 v;
        _ZNK7PathPtr7GetNodeER7Vector3j(c + 0x364, &v, n + 1);
        *(short*)(c + 0x8e) = Vec3_HorzAngle((struct Vec3*)(c + 0x5c), &v);
    }
    {
        int *pv = (int *)(((int)c + 0x380) & 0xFFFFFFFFFFFFFFFF);
        *pv = *pv - *(int*)(c + 0x98);
    }
    return *(int*)(c + 0x380) < 0 ? 1 : 0;
}
