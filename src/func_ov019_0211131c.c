// NONMATCHING: base materialization / addressing (div=3). Logic verified correct vs ROM; not
// byte-matchable from C at mwccarm 1.2/sp2p3 (see notes/matching-style.md).
// Counts as decompiled, not matched.
struct Vec3 { int x, y, z; };
extern void UpdateAngle(short* a, short b, int n, short s);
extern int PathPtr_NumNodes(void* p);
extern void PathPtr_GetNode(void* p, struct Vec3* v, unsigned int i);
extern short Vec3_HorzAngle(struct Vec3* a, struct Vec3* b);
int func_ov019_0211131c(char* c) {
    UpdateAngle((short*)(c+0x94), *(short*)(c+0x38c), 2, 0x600);
    int n = *(int*)(c+0x36c);
    if (n >= PathPtr_NumNodes(c+0x364) - 2) {
        *(short*)(c+0x8e) = *(short*)(c+0x94);
    } else {
        struct Vec3 v;
        PathPtr_GetNode(c+0x364, &v, n+1);
        *(short*)(c+0x8e) = Vec3_HorzAngle((struct Vec3*)(c+0x5c), &v);
    }
    int* pv = (int*)(c+0x380);
    *pv = *pv - *(int*)(c+0x98);
    return *(int*)(c+0x380) < 0 ? 1 : 0;
}
