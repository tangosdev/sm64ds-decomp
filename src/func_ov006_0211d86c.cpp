//cpp
extern "C" unsigned char TOP_SCREEN_RELATED;
extern "C" unsigned char BOTTOM_SCREEN_RELATED;
extern "C" void func_ov006_0211d7d8(char *p);
extern "C" void _ZN5Sound12PlayBank2_2DEj(unsigned int);
extern "C" void func_ov006_0211f51c(char *c);
extern "C" void func_ov006_0211d608(char *c);

extern "C" void func_ov006_0211d86c(char *thiz, int idx)
{
    char *base = thiz + (idx << 5);
    if (*(unsigned short*)(base + 0x4b00 + 0xb4) != 0) {
        *(unsigned short*)(thiz + 0x4bb4 + (idx << 5)) =
            *(unsigned short*)(thiz + 0x4bb4 + (idx << 5)) - 1;
        return;
    }
    *(unsigned char*)(base + 0x4000 + 0xbb9) = 0;
    *(unsigned char*)(base + 0x4000 + 0xbbc) = 0;
    func_ov006_0211d7d8(thiz);
    TOP_SCREEN_RELATED |= 4;
    BOTTOM_SCREEN_RELATED |= 1;
    _ZN5Sound12PlayBank2_2DEj(0x1f6);
    *(int*)(thiz + 0x4000 + 0xbe8) = 2;
    func_ov006_0211f51c(thiz);
    func_ov006_0211d608(thiz);
    _ZN5Sound12PlayBank2_2DEj(0x1f5);
}
