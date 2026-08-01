//cpp
struct Actor {
    int GetSubtraction(short a, short b);
};
extern "C" void func_0201267c(int a0, void *a1);

extern "C" int func_ov063_0211a8a4(char *thiz)
{
    Actor *a = (Actor*)thiz;
    int r6 = a->GetSubtraction(*(short*)(thiz + 0x5b0), *(short*)(thiz + 0x94));
    int r0 = a->GetSubtraction(*(short*)(thiz + 0x94),
                               *(short*)(*(char**)(thiz + 0x484) + 0x8e));
    int ret = 0;
    *(int*)(thiz + 0xa8) = 0;
    if (r6 > 0x1568 || r0 < 0x6b58) {
        if (*(unsigned char*)(thiz + 0x5c8) == 0x28) {
            *(unsigned char*)(thiz + 0x5c9) = 0xff;
            if (*(unsigned char*)(thiz + 0x5cf) != 0xf)
                func_0201267c(0xf8, thiz + 0x74);
            *(unsigned short*)(thiz + 0x5c0) = 0x1e;
        }
        if (*(unsigned char*)(thiz + 0x5c8) > 0xb4)
            ret = 1;
    } else {
        if (*(unsigned char*)(thiz + 0x5c8) == 0xff)
            *(unsigned char*)(thiz + 0x5c9) = 0x28;
    }
    return ret;
}
