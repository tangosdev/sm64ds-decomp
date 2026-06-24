typedef int Fix12i;
typedef long long s64;
struct Vec3 { Fix12i x, y, z; };

extern void Vec3_Sub(struct Vec3* out, struct Vec3* a, struct Vec3* b);
extern int LenVec3(struct Vec3* v);
extern int _ZN4cstd4fdivEii(int a, int b);
extern void Vec3_Add(struct Vec3* out, struct Vec3* a, struct Vec3* b);

void func_ov014_02111fe0(char* c){
    struct Vec3 v;
    struct Vec3 out;
    int len, lim;
    Vec3_Sub(&v, (struct Vec3*)(c + 0x5c), (struct Vec3*)(c + 0x5ec));
    len = LenVec3(&v);
    lim = *(int*)(c + 0x5f8) * 7 + 0xc8000;
    if (len <= lim) return;
    v.x = (int)(((s64)v.x * _ZN4cstd4fdivEii(lim, len) + 0x800) >> 12);
    v.y = (int)(((s64)v.y * _ZN4cstd4fdivEii(lim, len) + 0x800) >> 12);
    v.z = (int)(((s64)v.z * _ZN4cstd4fdivEii(lim, len) + 0x800) >> 12);
    Vec3_Add(&out, (struct Vec3*)(c + 0x5ec), &v);
    *(int*)(c + 0x5c) = out.x;
    *(int*)(c + 0x60) = out.y;
    *(int*)(c + 0x64) = out.z;
    if (*(int*)(c + 0x610) != 2) return;
    if (*(int*)(c + 0x5f8) == 0x64000) {
        *(int*)(c + 0x98) = 0;
        *(int*)(c + 0xa8) = 0;
    }
}
