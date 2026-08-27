//cpp
extern "C" {
extern void _Z14ApproachLinearRiii(int& x, int target, int step);
extern int RandomIntInternal(int* seed);
extern void func_ov090_02131e00(void* c, void* p);
extern int ApproachAngle(short* angle, int target, int invFactor, int maxDelta, int minDelta);
extern int data_0209e650;
extern int data_ov090_021344e4[];
extern int data_ov090_021344f4[];

int func_ov090_02131ac4(char* c)
{
    _Z14ApproachLinearRiii(*(int*)(c + 0x98), 0, 0x1000);
    if (*(int*)(c + 0x98) > 0) return 1;
    if (*(unsigned char*)(c + 0x39c) == 0) {
        *(int*)(c + 0x374) = *(int*)(c + 0x5c);
        *(int*)(c + 0x378) = *(int*)(c + 0x60);
        *(int*)(c + 0x37c) = *(int*)(c + 0x64);
        *(short*)(c + 0x39a) = (short)(((unsigned int)RandomIntInternal(&data_0209e650) >> 8) << 0xd);
        func_ov090_02131e00(c, data_ov090_021344e4);
    }
    ApproachAngle((short*)(c + 0x94), *(short*)(c + 0x39a), 1, 0x100, 0x100);
    if (*(unsigned short*)(c + 0x100) == 0)
        func_ov090_02131e00(c, data_ov090_021344f4);
    return 1;
}
}
