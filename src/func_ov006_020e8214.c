typedef unsigned char u8;
extern int _ZNK9Animation12WillHitFrameEi(void* anim, int frame);

void func_ov006_020e8214(char* c)
{
    u8 state = *(u8*)(c + 0x5553);
    if (state == 0) {
        u8* p = (u8*)(((long long)(int)(c + 0x5553)) & 0xFFFFFFFFFFFFFFFFLL);
        *(u8*)(c + 0x51f2) = 0x1e;
        (*p)++;
        return;
    }
    if (state != 1)
        return;
    if (_ZNK9Animation12WillHitFrameEi(c + 0x5034, 0)) {
        u8* p = (u8*)(((long long)(int)(c + 0x5554)) & 0xFFFFFFFFFFFFFFFFLL);
        (*p)++;
    }
    if (*(u8*)(c + 0x5554) < 1)
        return;
    {
        u8* p = (u8*)(((long long)(int)(c + 0x5553)) & 0xFFFFFFFFFFFFFFFFLL);
        (*p)++;
    }
    *(u8*)(c + 0x51f2) = 0x1f;
}
