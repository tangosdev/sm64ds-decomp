//cpp
extern "C" int data_ov006_02140428;
extern "C" void _Z14ApproachLinearRiii(int &r, int b, int c);
extern "C" void _ZN5Sound12PlayBank2_2DEj(unsigned int);
extern "C" void func_ov006_020c8c78(int a0, int a1);
extern "C" void func_ov006_020c8768(char *p);

extern "C" void func_ov006_020c78ec(char *thiz)
{
    *(short *)(((int)thiz + 0x32) & 0xFFFFFFFFFFFFFFFFLL) -= 1;
    if (*(short*)(thiz + 0x32) == 0) {
        _Z14ApproachLinearRiii((&data_ov006_02140428)[0], 0, 1);
        _ZN5Sound12PlayBank2_2DEj(0x130);
        func_ov006_020c8c78(*(short*)(thiz + 0x36), 0xc0);
        func_ov006_020c8768(thiz);
        return;
    }
    *(int*)(thiz + 0x20) =
        (int)(((long long)*(int*)(thiz + 0x20) * 0xc00 + 0x800) >> 12);
    if (*(int*)(thiz + 0x14) < -0x6c000) {
        *(int*)(thiz + 0x14) = -0x6c000;
        return;
    }
    if (*(int*)(thiz + 0x14) > 0x6c000)
        *(int*)(thiz + 0x14) = 0x6c000;
}
