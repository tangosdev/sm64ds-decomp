// NONMATCHING: base materialization / addressing (div=8). Logic verified correct vs ROM; not
// byte-matchable from C at mwccarm 1.2/sp2p3 (see notes/matching-style.md).
// Counts as decompiled, not matched.
typedef struct { int x, y, z; } Vec3;
extern void AddVec3(Vec3* out, const Vec3* a, const Vec3* b);
extern int Vec3_Dist(const Vec3* a, const Vec3* b);
extern int _ZNK7PathPtr7GetNodeER7Vector3j(void* p, Vec3* v, unsigned int i);
extern void func_ov091_02131cb0(void* c, const Vec3* v0, const Vec3* v1);
extern void func_02010da4(void* c);

int func_ov091_02131db8(char* c) {
    int r4;
    AddVec3((Vec3*)(c+0x5c), (const Vec3*)(c+0xa4), (const Vec3*)(c+0x5c));
    if (Vec3_Dist((const Vec3*)(c+0x5c), (const Vec3*)(c+0x338)) < (*(int*)(c+0x98) >> 1)) {
        r4 = 1;
        *(int*)(c+0x32c) = *(int*)(c+0x338);
        *(int*)(c+0x330) = *(int*)(c+0x33c);
        *(int*)(c+0x334) = *(int*)(c+0x340);
        *(int*)(c+0x5c) = *(int*)(c+0x32c);
        *(int*)(c+0x60) = *(int*)(c+0x330);
        *(int*)(c+0x64) = *(int*)(c+0x334);
        if (*(int*)(c+0x320) == 1) {
            int* p328 = (int*)(c+0x328);
            *p328 = *p328 + 1;
            if (*p328 >= *(int*)(c+0x324)) {
                *p328 = *(int*)(c+0x324) - 2;
                r4 = -1;
            }
        } else {
            int* p328 = (int*)(c+0x328);
            *p328 = *p328 - 1;
            if (*p328 < 0) {
                *p328 = r4;
                r4 = -1;
            }
        }
        _ZNK7PathPtr7GetNodeER7Vector3j((void*)(c+0x344), (Vec3*)(c+0x338), (unsigned int)*(int*)(c+0x328));
        {
            Vec3 a, b;
            a.x = *(int*)(c+0x338);
            a.y = *(int*)(c+0x33c);
            a.z = *(int*)(c+0x340);
            b.x = *(int*)(c+0x32c);
            b.y = *(int*)(c+0x330);
            b.z = *(int*)(c+0x334);
            func_ov091_02131cb0(c, &a, &b);
        }
        return r4;
    }
    func_02010da4(c);
    return 0;
}
