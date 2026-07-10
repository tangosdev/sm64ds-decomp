typedef unsigned char u8;
typedef unsigned short u16;

extern int _ZN5Sound15PlaySecretSoundEP5ActorPt(void* a, u16* p);
extern int func_ov064_02116d1c(void* c);
extern void _ZN5Enemy12UpdateWMClsnER12WithMeshClsnj(void* c, void* clsn, unsigned f);
extern int _ZNK12WithMeshClsn10IsOnGroundEv(void* c);
extern void func_0200fa8c(void* c, int a);
extern void func_ov064_02116bac(void* c);

int _ZN8BigBully8BehaviorEv(char* c)
{
    u8 s = *(u8*)(c + 0x3fe);
    if (s >= 4) {
        if (*(u16*)(c + 0x3fa) != 0) {
            if (_ZN5Sound15PlaySecretSoundEP5ActorPt(c, (u16*)(c + 0x3fa)) != 0)
                *(u16*)(c + 0x3fa) = 0;
        }
        return func_ov064_02116d1c(c);
    }
    if (s == 3) {
        int t;
        int m;
        int* p;
        if (_ZN5Sound15PlaySecretSoundEP5ActorPt(c, (u16*)(c + 0x3fa)) != 0)
            *(u16*)(c + 0x3fa) = 0;
        t = *(int*)(c + 0xa8) + *(int*)(c + 0x9c);
        m = *(int*)(c + 0xa0);
        if (t >= m) m = t;
        *(int*)(c + 0xa8) = m;
        p = (int*)(((int)c + 0x60) & 0xFFFFFFFFFFFFFFFF);
        *p = *p + *(int*)(c + 0xa8);
        _ZN5Enemy12UpdateWMClsnER12WithMeshClsnj(c, c + 0x174, 0);
        if (_ZNK12WithMeshClsn10IsOnGroundEv(c + 0x174) != 0) {
            u8* q;
            func_0200fa8c(c, 0);
            q = (u8*)(((int)c + 0x3fe) & 0xFFFFFFFFFFFFFFFF);
            *q = *q + 1;
        }
        func_ov064_02116bac(c);
    }
    return 1;
}
