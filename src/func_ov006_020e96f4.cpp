//cpp
extern "C" void _ZN3G2x13SetBlendAlphaEPVttttt(
    volatile unsigned short *p, unsigned short a, unsigned short b,
    unsigned short c, int d);
extern "C" void func_ov006_020e9374(void *c);

extern "C" void func_ov006_020e96f4(char *thiz)
{
    *(unsigned short*)(((int)thiz + 0x554a)) += 1;
    if (*(unsigned short*)(thiz + 0x554a) >= 4) {
        *(unsigned short*)(thiz + 0x554a) = 0;
        *(unsigned char*)(((int)thiz + 0x554e)) += 1;
        _ZN3G2x13SetBlendAlphaEPVttttt(
            (volatile unsigned short*)0x4001050, 0, 4,
            *(unsigned char*)(thiz + 0x554e), 0x10 - *(unsigned char*)(thiz + 0x554e));
    }
    func_ov006_020e9374(thiz);
    if (*(unsigned char*)(thiz + 0x554e) < 0x10)
        return;
    *(unsigned char*)(thiz + 0x554e) = 0;
    *(unsigned short*)(thiz + 0x554a) = 0;
    *(int*)(thiz + 0x5540) = 2;
    *(unsigned short*)(thiz + 0x5548) = 0;
}
