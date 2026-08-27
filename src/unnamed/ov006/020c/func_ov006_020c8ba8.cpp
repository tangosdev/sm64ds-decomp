//cpp
int ApproachLinear(int&, int, int);
int ApproachLinear2(short&, short, short);

extern "C" void func_ov006_020c8ba8(char* c)
{
    if (*(short*)(c+0x18) == 0) return;
    *(short*)(((int)c + 0x1c)) += 0x200;
    *(int*)(c+0x10) = (int)(((long long)*(int*)(c+0x10) * *(int*)(c+0x14) + 0x800) >> 12);
    *(int*)(((int)c + 0xc)) -= 0x40;
    ApproachLinear(*(int*)(c+0x14), 0x800, 0x10);
    if (ApproachLinear2(*(short*)(c+0x1e), 0, 1) != 0) {
        if (*(short*)(c+0x1a) != 2)
            *(short*)(c+0x18) = 0;
    } else {
        *(int*)(c) += *(int*)(c+0x8);
        *(int*)(((int)c + 0x4)) += *(int*)(c+0xc);
    }
}
