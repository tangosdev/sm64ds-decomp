typedef struct { int x, y, z; } Vec3;
extern void AddVec3(Vec3* out, const Vec3* a, const Vec3* b);
extern int Vec3_Dist(const Vec3* a, const Vec3* b);
extern int _ZNK7PathPtr7GetNodeER7Vector3j(void* p, Vec3* v, unsigned int i);
extern void func_ov091_02131cb0(void* c, const Vec3* v0, const Vec3* v1);
extern void func_02010da4(void* c);

#define LAUNDER_P(base, off) ((int *)((unsigned long long)((int)(base) + (off)) & 0xFFFFFFFFFFFFFFFFULL))

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
            int *p = LAUNDER_P(c, 0x328);
            *p = *p + 1;
            if (*(int*)(c+0x328) >= *(int*)(c+0x324)) {
                *(int*)(c+0x328) = *(int*)(c+0x324) - 2;
                r4 = -1;
            }
        } else {
            int *p = LAUNDER_P(c, 0x328);
            *p = *p - 1;
            if (*(int*)(c+0x328) < 0) {
                *(int*)(c+0x328) = r4;
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
