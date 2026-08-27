typedef struct { int x, y, z; } Vec3;

extern void SubVec3(Vec3* a, Vec3* b, Vec3* out);
extern void CrossVec3(Vec3* a, Vec3* b, Vec3* out);
extern void NormalizeVec3(Vec3* a, Vec3* out);

void func_ov075_0211abb0(char* self)
{
    Vec3 ej;
    Vec3 ei;
    Vec3 n;
    char* p;
    int i;
    int j;
    int mask = 0x3ff;

    p = *(char**)(self + 0x80);

    for (i = 0; i < *(unsigned char*)(self + 0xa7); i++) {
        for (j = 0; j < *(unsigned char*)(self + 0xa6); j++) {
            int *q;
            int z = 0;
            q = (int*)&ej; q[0] = z; q[1] = z; q[2] = z;
            q = (int*)&ei; q[0] = z; q[1] = z; q[2] = z;
            q = (int*)&n;  q[0] = z; q[1] = z; q[2] = z;

            if (j == 0)
                SubVec3((Vec3*)p, (Vec3*)(p + 0x18), &ej);
            else if (j == *(unsigned char*)(self + 0xa6) - 1)
                SubVec3((Vec3*)(p - 0x18), (Vec3*)p, &ej);
            else
                SubVec3((Vec3*)(p - 0x18), (Vec3*)(p + 0x18), &ej);

            if (i == 0)
                SubVec3((Vec3*)p, (Vec3*)(p + *(unsigned char*)(self + 0xa7) * 0x18), &ei);
            else if (i == *(unsigned char*)(self + 0xa7) - 1)
                SubVec3((Vec3*)(p - *(unsigned char*)(self + 0xa7) * 0x18), (Vec3*)p, &ei);
            else
                SubVec3((Vec3*)(p - *(unsigned char*)(self + 0xa7) * 0x18),
                        (Vec3*)(p + *(unsigned char*)(self + 0xa7) * 0x18), &ei);

            CrossVec3(&ej, &ei, &n);
            n.z >>= 1;
            NormalizeVec3(&n, &n);
            *(int*)(p + 0x10) = (n.x >> 3 & mask)
                | ((n.y >> 3 & mask) << 10)
                | ((n.z >> 3 & mask) << 20);
            p += 0x18;
        }
    }
}
