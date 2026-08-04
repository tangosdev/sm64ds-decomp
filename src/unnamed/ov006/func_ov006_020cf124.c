#pragma opt_strength_reduction off

typedef struct { int x, y, z; } Vec3;

extern void SubVec3(Vec3* a, Vec3* b, Vec3* c);
extern void CrossVec3(const Vec3* a, const Vec3* b, Vec3* out);
extern void NormalizeVec3(int* v, int* out);

void func_ov006_020cf124(char* self)
{
    Vec3 ej;
    Vec3 ei;
    char* p;
    Vec3* n;
    int* out;
    int i;
    int j;
    int mask = 0x3ff;

    p = self + 0x5c;
    n = (Vec3*)(self + 0x1dc);
    out = (int*)(self + 0x2dc);

    for (i = 0; i < 4; i++) {
        for (j = 0; j < 4; j++) {
            int *q;
            int z = 0;

            q = (int*)&ej; q[0] = z; q[1] = z; q[2] = z;
            q = (int*)&ei; q[0] = z; q[1] = z; q[2] = z;

            if (j == 0)
                SubVec3((Vec3*)p, (Vec3*)(p + 0xc), &ej);
            else if (j == 3)
                SubVec3((Vec3*)(p - 0xc), (Vec3*)p, &ej);
            else
                SubVec3((Vec3*)(p - 0xc), (Vec3*)(p + 0xc), &ej);

            if (i == 0)
                SubVec3((Vec3*)p, (Vec3*)(p + 0x30), &ei);
            else if (i == 3)
                SubVec3((Vec3*)(p - 0x30), (Vec3*)p, &ei);
            else
                SubVec3((Vec3*)(p - 0x30), (Vec3*)(p + 0x30), &ei);

            CrossVec3(&ej, &ei, n);
            NormalizeVec3((int*)n, (int*)n);

            n->x = (int)(((long long)n->x * 0xff8 + 0x800) >> 12);
            n->y = (int)(((long long)n->y * 0xff8 + 0x800) >> 12);
            n->z = (int)(((long long)n->z * 0xff8 + 0x800) >> 12);

            *out = ((n->x >> 3) & mask)
                 | (((n->y >> 3) & mask) << 10)
                 | (((n->z >> 3) & mask) << 20);

            n++;
            out++;
            p += 0xc;
        }
    }
}
