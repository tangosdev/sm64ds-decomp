//cpp
typedef int s32;

struct Vector3 {
    s32 x, y, z;
    Vector3() {}
    Vector3(s32 a, s32 b, s32 c) : x(a), y(b), z(c) {}
    Vector3(const Vector3& o) : x(o.x), y(o.y), z(o.z) {}
};

extern "C" {
char* _ZN5Actor10FindWithIDEj(unsigned int id);
s32 Vec3_HorzDist(const Vector3* a, const Vector3* b);
void func_ov002_020e81e0(char* a0);
void func_ov002_020e7e24(char* a0);
void func_ov002_020e7d08(char* a0);
}
extern "C" void func_ov002_020e947c(char* a0, Vector3 v, int a2);

extern "C" void func_ov002_020ea90c(char* self)
{
    if (*(s32*)(self + 0xa8) <= -0x20000) {
        char* other = _ZN5Actor10FindWithIDEj(*(unsigned int*)(self + 0x434));
        if (other == 0 || Vec3_HorzDist((Vector3*)(self + 0x5c), (Vector3*)(other + 0x5c)) == 0) {
            *(s32*)(self + 0xa8) = 0x18000;
            *(s32*)(self + 0x440) = 3;
        } else {
            Vector3* pp = (Vector3*)(((long long)(int)(other + 0x5c)) & 0xFFFFFFFFFFFFFFFFLL);
            Vector3 v;
            int yv;
            v.x = pp->x;
            yv = pp->y;
            *(volatile s32*)&v.y = yv;
            v.z = pp->z;
            v.y = yv + 0xc8000;
            func_ov002_020e947c(self, v, 0x190000);
            *(s32*)(self + 0x440) = 2;
        }
    }
    func_ov002_020e81e0(self);
    func_ov002_020e7e24(self);
    func_ov002_020e7d08(self);
}
