//cpp
struct Vec3 { int x, y, z; };
extern "C" {
extern void func_0203cd80(struct Vec3 *v, int a);
extern void func_0203ccd4(struct Vec3 *v, short angle);
extern void func_02016a14(void *p, int a);
extern void func_02016a04(void *p, int a);
extern void func_ov006_020c35a8(char *c);
extern int data_ov006_0213aee8[12];
void func_ov006_020c3bf4(void *self);
}

struct M48 { int w[12]; };

struct Obj {
    virtual void f0() = 0;
    virtual void f1() = 0;
    virtual void f2() = 0;
    virtual void f3() = 0;
    virtual void f4() = 0;
    virtual void f5(int a) = 0;
};

void func_ov006_020c3bf4(void *self)
{
    char *c = (char*)self;
    struct Vec3 v;
    unsigned int packed;
    int i;

    v.x = 0;
    v.y = 0;
    v.z = 0xfffff008;
    func_0203cd80(&v, 0);
    func_0203ccd4(&v, 0x600);

    packed = (((short)v.x >> 3) & 0x3ff)
           | ((((short)v.y >> 3) & 0x3ff) << 10)
           | ((((short)v.z >> 3) & 0x3ff) << 20);
    *(unsigned int*)0x40004c8 = packed;

    func_02016a14(c + 0xd18, 0x2bff);
    func_02016a04(c + 0xd18, 0x1211);

    *(struct M48*)(c + 0xd34) = *(struct M48*)data_ov006_0213aee8;

    ((Obj*)(c + 0xd18))->f5(0);

    i = 0;
    {
        char *p = c + 8;
        for (; i < 0x16; i++) {
            if (*(int*)(c + 0x48) != 0)
                func_ov006_020c35a8(p);
            c += 0x98;
            p += 0x98;
        }
    }
}
