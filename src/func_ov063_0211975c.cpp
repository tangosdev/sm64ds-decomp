//cpp
typedef unsigned char u8;
typedef signed char s8;
typedef unsigned short u16;
typedef short s16;

extern "C" int func_ov063_0211a3d0(void* p);
extern "C" void func_0201267c(int a, char* b);
extern "C" {
extern char* _ZN8CapEnemy15RespawnIfHasCapEv(void* self);
extern void _ZN5Actor24KillAndTrackInDeathTableEv(void* self);
}

extern "C" void func_ov063_0211975c(char* self) {
    if (!func_ov063_0211a3d0(self)) return;
    if (*(int*)(self + 0x5a4) != 0) {
        _ZN5Actor24KillAndTrackInDeathTableEv(self);
        func_0201267c(0xd5, self + 0x74);
        if ((*(u8*)(self + 0x113) & 0xf) >= 6) return;
        *(int*)(self + 0x5c) = *(int*)(self + 0x51c);
        *(int*)(self + 0x60) = *(int*)(self + 0x520);
        *(int*)(self + 0x64) = *(int*)(self + 0x524);
        *(s8*)(self + 0xcc) = *(s8*)(self + 0x5d0);
        *(s16*)(self + 0x92) = *(s16*)(self + 0x570);
        *(s16*)(self + 0x94) = *(s16*)(self + 0x572);
        *(s16*)(self + 0x96) = *(s16*)(self + 0x574);
        s16* src = (s16*)(((int)self + 0x92) & 0xFFFFFFFFFFFFFFFF);
        *(s16*)(self + 0x8c) = src[0];
        *(s16*)(self + 0x8e) = src[1];
        *(s16*)(self + 0x90) = src[2];
        char* r = (char*)_ZN8CapEnemy15RespawnIfHasCapEv(self);
        if (r == 0) return;
        {
            u16* p = (u16*)(((int)r + 0x5d4) & 0xFFFFFFFFFFFFFFFF);
            *p &= ~2;
        }
    } else {
        *(u8*)(self + 0x5cc) = 5;
        *(u8*)(self + 0x5ce) = 0;
        {
            int* q = (int*)(((int)self + 0x19c) & 0xFFFFFFFFFFFFFFFF);
            *q |= 1;
        }
        {
            u16* p = (u16*)(((int)self + 0x5d4) & 0xFFFFFFFFFFFFFFFF);
            *p &= ~8;
        }
    }
}
