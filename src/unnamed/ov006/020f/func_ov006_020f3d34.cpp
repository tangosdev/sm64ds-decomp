//cpp
extern "C" void func_02012790(int a);
extern "C" void func_ov004_020b5dd4(void);
namespace Sound { void PlayBank2_2D(unsigned int); }

extern "C" void func_ov006_020f3d34(char *self)
{
    int a, b;
    unsigned char *ea, *eb;
    if (*(unsigned char *)(self + 0x5338) < 2) return;
    a = *(unsigned char *)(self + 0x532e);
    b = *(unsigned char *)(self + 0x532f);
    ea = (unsigned char *)(self + 0x51bc + a * 0x18);
    if (*ea != 4) return;
    eb = (unsigned char *)(self + 0x51bc + b * 0x18);
    if (*eb != 4) return;
    if (*(unsigned char *)(self + 0x51b8 + a * 0x18) == *(unsigned char *)(self + 0x51b8 + b * 0x18)) {
        *(unsigned char *)(self + 0x51bb + a * 0x18) = 0;
        *(unsigned char *)(self + 0x51ba + a * 0x18) = 0;
        *(unsigned char *)(self + 0x51bb + b * 0x18) = 0;
        *(unsigned char *)(self + 0x51ba + b * 0x18) = 0;
        func_02012790(0x26);
        Sound::PlayBank2_2D(0x13d);
        (*(unsigned char *)(((int)self + 0x5337)))++;
        *(unsigned char *)(self + 0x5338) = 0;
    } else {
        func_02012790(0xe);
        (*(unsigned char *)(((int)self + 0x533a)))++;
        func_ov004_020b5dd4();
        Sound::PlayBank2_2D(0x13e);
        if (*(unsigned char *)(self + 0x533a) < *(unsigned char *)(self + 0x533b)) {
            *ea = 5;
            *eb = 5;
            *(unsigned char *)(self + 0x5338) = 0;
        }
    }
}
