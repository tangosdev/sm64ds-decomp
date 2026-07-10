typedef short s16;
typedef struct { int x, y, z; } Vec3;

extern int Vec3_HorzDist(Vec3* a, Vec3* b);
extern s16 Vec3_HorzAngle(Vec3* a, Vec3* b);
extern int _ZN5Actor14GetSubtractionEss(void* self, s16 a, s16 b);

#pragma opt_strength_reduction off
void func_ov079_02123f34(char* self)
{
    s16 minAngle;
    int minDist, minDist2;
    int dist;
    int i;
    s16 angle;
    Vec3 v;

    *(s16*)(self + 0x3b8) = *(s16*)(self + 0x94);
    minAngle = *(s16*)(self + 0x94);
    minDist2 = 0x7fffffff;
    minDist = 0x7fffffff;
    for (i = 0; i < 4; i++) {
        char* obj = *(char**)(self + 0x39c + i * 4);
        Vec3* p;
        if (obj == 0)
            continue;
        p = (Vec3*)(((long long)(int)(obj + 0x5c)) & 0xFFFFFFFFFFFFFFFFLL);
        v.x = p->x;
        v.y = p->y;
        v.z = p->z;
        dist = Vec3_HorzDist((Vec3*)(self + 0x5c), &v);
        angle = Vec3_HorzAngle((Vec3*)(self + 0x5c), &v);
        if (dist < minDist) {
            *(int*)(self + 0x3ec) = i;
            minDist = dist;
            minAngle = angle;
        }
        if (_ZN5Actor14GetSubtractionEss(self, angle, *(s16*)(self + 0x94)) < 0x2000) {
            if (dist < minDist2) {
                minDist2 = dist;
                *(int*)(self + 0x3f0) = i;
            }
        }
    }
    *(int*)(self + 0x3f4) = minDist;
    *(int*)(self + 0x3f8) = minDist2;
    *(s16*)(self + 0x3b8) = minAngle;
}
