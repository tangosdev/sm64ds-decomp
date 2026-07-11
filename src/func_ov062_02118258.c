typedef struct Vector3 { int x, y, z; } Vector3;

#define AT(p, off) ((void*)(int)(((long long)(int)((char*)(p) + (off))) & 0xFFFFFFFFFFFFFFFFLL))

extern void* _ZN5Actor13ClosestPlayerEv(void*);
extern int Vec3_Dist(const Vector3* a, const Vector3* b);
extern short Vec3_HorzAngle(const Vector3* a, const Vector3* b);

void func_ov062_02118258(char* c, int lim)
{
    *(void**)(c + 0x3b4) = _ZN5Actor13ClosestPlayerEv(c);

    if (*(void**)(c + 0x3b4) == 0 || Vec3_Dist((Vector3*)(c + 0x5c), (Vector3*)(c + 0x39c)) > lim) {
        *(short*)(c + 0x3c0) = Vec3_HorzAngle((Vector3*)(c + 0x5c), (Vector3*)(c + 0x39c));
        *(int*)(c + 0x3b8) = 0x61a8000;
    } else {
        Vector3 v;
        int* src = (int*)AT(*(char**)(c + 0x3b4), 0x5c);
        v.x = src[0];
        v.y = src[1];
        v.z = src[2];
        if (Vec3_Dist((Vector3*)(c + 0x39c), &v) > lim) {
            *(int*)(c + 0x3b8) = 0x4e20000;
            return;
        }
        *(int*)(c + 0x3b8) = Vec3_Dist((Vector3*)(c + 0x5c), &v);
        *(short*)(c + 0x3c0) = Vec3_HorzAngle((Vector3*)(c + 0x5c), &v);
    }
}
