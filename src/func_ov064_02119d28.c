typedef signed short s16;
typedef unsigned short u16;
typedef unsigned char u8;
typedef long long s64;

extern s16 data_02082214[];
extern void func_ov064_02119afc(char* c);
extern void _Z14ApproachLinearRiii(int* p, int a, int b);
extern void _ZN9ActorBase18MarkForDestructionEv(void* t);

int func_ov064_02119d28(char* c) {
    int spd;
    func_ov064_02119afc(c);
    if (*(u16*)(c + 0x100) < 0x1e) {
        if (*(u8*)(c + 0x380) >= 2) {
            u8 *p = (u8*)(((int)c + 0x380) & 0xFFFFFFFFFFFFFFFFLL);
            *p = *p - 1;
        }
    }
    spd = (*(int*)(c + 0x37c) == 1) ? 0x333 : 0x199;
    {
        int *q = (int*)(((int)c + 0x374) & 0xFFFFFFFFFFFFFFFFLL);
        *q = *q + 0x1000;
    }
    {
        s16 a = *(int*)(c + 0x374);
        int idx = ((u16)a >> 4) * 2;
        int fr = (int)((((s64)data_02082214[idx] << 10) + 0x800) >> 12);
        int v = fr + *(int*)(c + 0x384);
        _Z14ApproachLinearRiii((int*)(c + 0x384), 0x2000, spd);
        _Z14ApproachLinearRiii((int*)(c + 0x80), v, spd);
    }
    {
        int *q = (int*)(((int)c + 0x378) & 0xFFFFFFFFFFFFFFFFLL);
        *q = *q + 0x800;
    }
    {
        s16 b = *(int*)(c + 0x378);
        int idx2 = ((u16)b >> 4) * 2;
        s16 *w = (s16*)(((int)c + 0x92) & 0xFFFFFFFFFFFFFFFFLL);
        *w = *w + (int)((((s64)data_02082214[idx2] << 8) + 0x800) >> 12);
    }
    *(int*)(c + 0x88) = *(int*)(c + 0x80);
    *(int*)(c + 0x84) = *(int*)(c + 0x88);
    if (*(u16*)(c + 0x100) == 0 || *(u8*)(c + 0x380) <= 1)
        _ZN9ActorBase18MarkForDestructionEv(c);
    return 1;
}
