//cpp
extern "C" {
extern int _ZN14BlendModelAnim7SetAnimER8BCA_Fileii5Fix12IiEt(void* self, void* bca, int a, int b, int fix, unsigned short t);
extern int func_ov078_02125c48(void* c, void* p);
extern int Vec3_Dist(const void* a, const void* b);
extern void _ZN6Player9DropActorEv(void* self);
extern int data_ov078_02126ee0[];
extern int data_ov078_0212708c;

struct Vec3 { int x, y, z; };

int func_ov078_02124f28(unsigned char* c)
{
    if (c[0x499] == 0) {
        int b = (*(int*)(c + 0xb0) & 0x4000) != 0;
        if (b != 0) {
            _ZN14BlendModelAnim7SetAnimER8BCA_Fileii5Fix12IiEt(
                c + 0x2cc, (void*)data_ov078_02126ee0[1], 0, 0, 0x1000, 0);
            c[0x499] = 1;
        }
    }

    {
        unsigned char* obj = *(unsigned char**)(c + 0x494);
        if (obj == 0) {
            func_ov078_02125c48(c, &data_ov078_0212708c);
            return 1;
        }
        {
            Vec3 copy = *(Vec3*)(obj + 0x5c);
            if (Vec3_Dist(c + 0x4d4, &copy) > 0x640000)
                goto drop;
            if (*(int*)(c + 0x4d8) - 0xa000 <= copy.y)
                goto flags;
        }
    }

drop:
    _ZN6Player9DropActorEv(*(void**)(c + 0x494));
    *(int*)(c + 0x494) = 0;
    func_ov078_02125c48(c, &data_ov078_0212708c);
    return 1;

flags:
    {
        int flags = *(int*)(c + 0xb0);
        int b0 = (flags & 0x400) != 0;
        if (b0 == 0) {
            int b1 = (flags & 0x2000) != 0;
            if (b1 == 0) {
                int b2 = (flags & 0x100) != 0;
                if (b2 != 0)
                    goto done;
            }
        }
        if (b0 != 0) {
            *(short*)(c + 0x94) = *(short*)(*(unsigned char**)(c + 0x494) + 0x8e);
        }
        {
            short t = *(short*)(c + 0x94);
            *(short*)(c + 0x8e) = t;
        }
        {
            int* p354 = (int*)(((long long)(int)(c + 0x354)) & 0xFFFFFFFFFFFFFFFFLL);
            *p354 = *p354 & ~2;
        }
        func_ov078_02125c48(c, &data_ov078_0212708c);
        {
            unsigned char* p = *(unsigned char**)(c + 0x494);
            if (p != 0) {
                if (*(int*)(p + 8) == 2) {
                    *(int*)(c + 0xa8) = 0x32000;
                    *(int*)(c + 0x98) = 0x1e000;
                }
            }
        }
        *(int*)(c + 0x494) = 0;
    }
done:
    return 1;
}
}
