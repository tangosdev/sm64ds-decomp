//cpp
typedef unsigned char u8;
typedef signed char s8;
typedef unsigned short u16;
typedef short s16;
typedef unsigned int u32;
typedef int s32;

extern "C" {
extern void _ZN5Actor24KillAndTrackInDeathTableEv(void *a);
extern void func_0201267c(u32 id, void *p);
extern void *_ZN8CapEnemy15RespawnIfHasCapEv(void *a);
extern int _ZN6Player9StartTalkER9ActorBaseb(void *p, void *a, int b);
extern void *_ZN5Actor10FindWithIDEj(u32 id);
extern void _ZN6Player11ShowMessageER9ActorBasejPK7Vector3jj(void *p, void *a, u32 msg, void *v, u32 a4, u32 a5);
extern void func_ov063_02116244(char *c);
extern u8 data_0209d660;
extern int _ZN5Sound15PlaySecretSoundEP5ActorPt(void *a, void *p);
extern void _ZN9ActorBase18MarkForDestructionEv(void *a);
}

extern "C" void func_ov063_0211934c(char *c)
{
    void *r4;
    u8 st;

    r4 = *(void **)(c + 0x488);
    if (r4 == 0 || *(u8 *)(c + 0x5cf) != 2) {
        _ZN5Actor24KillAndTrackInDeathTableEv(c);
        func_0201267c(0xd5, c + 0x74);
        if (((*(u8 *)(c + 0x113)) & 0xf) >= 6)
            return;
        
        {
            void *cap;
            *(s32 *)(c + 0x5c) = *(s32 *)(c + 0x51c);
            *(s32 *)(c + 0x60) = *(s32 *)(c + 0x520);
            *(s32 *)(c + 0x64) = *(s32 *)(c + 0x524);
            *(s8 *)(c + 0xcc) = *(s8 *)(c + 0x5d0);
            *(s16 *)(c + 0x92) = *(s16 *)(c + 0x570);
            *(s16 *)(c + 0x94) = *(s16 *)(c + 0x572);
            *(s16 *)(c + 0x96) = *(s16 *)(c + 0x574);
            s16 *src = (s16 *)(((int)c + 0x92));
            *(s16 *)(c + 0x8c) = src[0];
            *(s16 *)(c + 0x8e) = src[1];
            *(s16 *)(c + 0x90) = src[2];
            
            cap = _ZN8CapEnemy15RespawnIfHasCapEv(c);
            if (cap == 0)
                return;
            {
                u16 *p = (u16 *)(((int)cap + 0x5d4));
                *p &= ~2;
            }
        }
        return;
    }

    st = *(u8 *)(c + 0x5ce);
    switch (st) {
    case 0:
        if (_ZN6Player9StartTalkER9ActorBaseb(r4, c, 1) == 0)
            return;
        {
            u8 *q = (u8 *)(((long long)(int)(c + 0x5ce)));
            *q = *q + 1;
        }
        return;

    case 1:
    {
        void *found;
        *(void **)(c + 0x48c) = _ZN5Actor10FindWithIDEj(*(u32 *)(c + 0x490));
        found = *(void **)(c + 0x48c);
        if (found != 0) {
            s32 *cnt = (s32 *)(((long long)(int)((char *)found + 0x180)));
            *cnt = *cnt + 1;
        }
        found = *(void **)(c + 0x48c);
        if (found != 0 && *(s32 *)((char *)found + 0x180) == 5) {
            _ZN6Player11ShowMessageER9ActorBasejPK7Vector3jj(r4, c, 0xb5, c + 0x5c, 0, 2);
        } else {
            _ZN6Player11ShowMessageER9ActorBasejPK7Vector3jj(r4, c, 0xb4, c + 0x5c, 0, 2);
        }
        *(void **)(c + 0x48c) = 0;
        {
            u8 *q = (u8 *)(((long long)(int)(c + 0x5ce)));
            *q = *q + 1;
        }
        func_0201267c(0xf8, c + 0x74);
        return;
    }

    case 2:
        if (data_0209d660 != 0)
            return;
        func_0201267c(0xd5, c + 0x74);
        if (*(u32 *)(c + 0x490) != 0) {
            void *found;
            *(void **)(c + 0x48c) = _ZN5Actor10FindWithIDEj(*(u32 *)(c + 0x490));
            found = *(void **)(c + 0x48c);
            if (found != 0 && *(s32 *)((char *)found + 0x180) == 5) {
                func_ov063_02116244((char *)found);
                {
                    u16 *p = (u16 *)(((long long)(int)(c + 0x5c6)));
                    *p = *p + 1;
                }
            }
        }
        {
            u8 *q = (u8 *)(((long long)(int)(c + 0x5ce)));
            *q = *q + 1;
        }
        return;

    case 3:
        if (*(u16 *)(c + 0x500 + 0xc6) != 0) {
            if (_ZN5Sound15PlaySecretSoundEP5ActorPt(
                    c, (u16 *)(((long long)(int)(c + 0x5c6)))) == 0)
                return;
            _ZN9ActorBase18MarkForDestructionEv(c);
            if (((*(u8 *)(c + 0x113)) & 0xf) >= 6)
                return;
            
        {
            void *cap2;
            *(s32 *)(c + 0x5c) = *(s32 *)(c + 0x51c);
            *(s32 *)(c + 0x60) = *(s32 *)(c + 0x520);
            *(s32 *)(c + 0x64) = *(s32 *)(c + 0x524);
            *(s8 *)(c + 0xcc) = *(s8 *)(c + 0x5d0);
            *(s16 *)(c + 0x92) = *(s16 *)(c + 0x570);
            *(s16 *)(c + 0x94) = *(s16 *)(c + 0x572);
            *(s16 *)(c + 0x96) = *(s16 *)(c + 0x574);
            s16 *src = (s16 *)(((int)c + 0x92));
            *(s16 *)(c + 0x8c) = src[0];
            *(s16 *)(c + 0x8e) = src[1];
            *(s16 *)(c + 0x90) = src[2];
            
            cap2 = _ZN8CapEnemy15RespawnIfHasCapEv(c);
            if (cap2 == 0)
                return;
            {
                u16 *p = (u16 *)(((int)cap2 + 0x5d4));
                *p &= ~2;
            }
        }
            return;
        }
        _ZN9ActorBase18MarkForDestructionEv(c);
        if (((*(u8 *)(c + 0x113)) & 0xf) >= 6)
            return;
        
        {
            void *cap3;
            *(s32 *)(c + 0x5c) = *(s32 *)(c + 0x51c);
            *(s32 *)(c + 0x60) = *(s32 *)(c + 0x520);
            *(s32 *)(c + 0x64) = *(s32 *)(c + 0x524);
            *(s8 *)(c + 0xcc) = *(s8 *)(c + 0x5d0);
            *(s16 *)(c + 0x92) = *(s16 *)(c + 0x570);
            *(s16 *)(c + 0x94) = *(s16 *)(c + 0x572);
            *(s16 *)(c + 0x96) = *(s16 *)(c + 0x574);
            s16 *src = (s16 *)(((int)c + 0x92));
            *(s16 *)(c + 0x8c) = src[0];
            *(s16 *)(c + 0x8e) = src[1];
            *(s16 *)(c + 0x90) = src[2];
            
            cap3 = _ZN8CapEnemy15RespawnIfHasCapEv(c);
            if (cap3 != 0) {
                u16 *p = (u16 *)(((int)cap3 + 0x5d4));
                *p &= ~2;
            }
        }
        return;
    }
}
