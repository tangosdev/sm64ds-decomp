//cpp
struct Vector3; struct Vector3_16;
struct CapEnemy { void ReleaseCap(const Vector3 &v); };
struct Actor {
    static Actor *Spawn(unsigned int a, unsigned int b, const Vector3 &v,
                        const Vector3_16 *p, int e, int f);
};
extern "C" void func_0201267c(int a, void *b);
struct V3 { int x, y, z; };
extern "C" void func_ov063_021162c8(char *self)
{
    if ((unsigned int)(*(unsigned short*)(self + 0x5d4) << 0x19) >> 0x1f) {
        *(unsigned char*)(self + 0x5cc) = 4;
    } else {
        *(unsigned char*)(self + 0x5cc) = 3;
        V3 v;
        v.x = *(int*)(self + 0x564);
        v.y = *(int*)(self + 0x568);
        v.z = *(int*)(self + 0x56c);
        ((CapEnemy*)self)->ReleaseCap(*(Vector3*)&v);
        if ((unsigned int)(*(unsigned short*)(self + 0x5d4) << 0x1e) >> 0x1f) {
            unsigned int flags = 2;
            if (*(unsigned short*)(self + 0x4a0) == 0x121) flags |= 0x10;
            Actor *a = Actor::Spawn(*(unsigned short*)(self + 0x4a0), flags,
                                    *(Vector3*)(self + 0x504), 0,
                                    *(signed char*)(self + 0x5d0), -1);
            if (a != 0) {
                *(unsigned char*)((char*)a + 0x3aa) = 0xa;
                if (*(unsigned short*)(self + 0x4a0) == 0x121)
                    *(unsigned short*)((char*)a + 0x3a8) = 0;
            }
            unsigned short *ip = (unsigned short *)(((long long)(int)(self + 0x5d4)) & 0xFFFFFFFFFFFFFFFFLL);
            *ip = (unsigned short)(*ip & ~2);
        }
    }
    func_0201267c(0x14a, self + 0x74);
}
