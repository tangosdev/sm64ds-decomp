//cpp
extern "C" int data_ov006_02140588;
extern "C" void func_ov006_020cd62c(int n);
extern "C" void func_ov006_020cd510(int a);
namespace Sound { void PlayBank2_2D(unsigned int); }
int ApproachLinear(int&, int, int);

extern "C" void func_ov006_02121bc8(char* self)
{
    int q = data_ov006_02140588 / 10;
    int old = *(int*)(self + 0xbc);
    func_ov006_020cd62c(*(int*)(self + 0x5d94));
    func_ov006_020cd510((q << 8) + 0x1000);
    if (old != q) {
        *(int*)(self + 0xbc) = q;
        if (*(unsigned*)(self + 0xbc) > 0x270e)
            *(int*)(self + 0xbc) = 0x270e;
        if (*(int*)(self + 0x5d98) < 0x50) {
            *(int*)(self + 0x5d9c) = 0x78;
            Sound::PlayBank2_2D(0x1b2);
        }
    }
    if (ApproachLinear(*(int*)(self + 0x5d9c), 0, 1) != 0) {
        if (*(int*)(self + 0x5d94) == *(int*)(self + 0x5d98)) {
            int t;
            *(int*)(self + 0x5d98) = (q << 3) + 0x20;
            t = *(int*)(self + 0x5d98);
            if (t > 0x50)
                t = 0x50;
            *(int*)(self + 0x5d98) = t;
        }
        ApproachLinear(*(int*)(self + 0x5d94), *(int*)(self + 0x5d98), 2);
        *(int*)(self + 0x5da0) = 0;
    } else {
        *(int*)(self + 0x5da0) = (*(int*)(self + 0x5d9c) & 2) >> 1;
    }
}
