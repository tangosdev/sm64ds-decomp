//cpp
// NONMATCHING: missing logic (ROM does more) (div=62). Logic verified correct vs ROM; not
// byte-matchable from C at mwccarm 1.2/sp2p3 (see notes/matching-style.md).
// Counts as decompiled, not matched.
extern "C" {
extern void *_ZN5Actor13ClosestPlayerEv(void *o);
extern int _ZN6Player9StartTalkER9ActorBaseb(void *pl, void *a, int b);
extern void _ZN5Sound17ChangeMusicVolumeEj5Fix12IiE(unsigned a, int fx);
extern int _ZN6Player12GetTalkStateEv(void *pl);
extern int _ZN6Player11ShowMessageER9ActorBasejPK7Vector3jj(void *pl, void *a, unsigned m, void *v, unsigned d, unsigned e);
extern void func_0201277c(int a);
extern void _ZN5Sound22StopLoadedMusic_Layer3Ev(void);
extern void func_02011cfc(void);
extern char data_ov063_0211e1c8;
extern char *CAMERA;
}

extern "C" void func_ov063_02117b0c(void *cc)
{
    char *c = (char*)cc;

    switch (*(unsigned char*)(c + 0x5ce)) {
    case 0:
        *(void**)(c + 0x488) = _ZN5Actor13ClosestPlayerEv(c);
        if (!_ZN6Player9StartTalkER9ActorBaseb(*(void**)(c + 0x488), c, 1))
            return;
        *(unsigned char*)(c + 0x5ce) += 1;
        _ZN5Sound17ChangeMusicVolumeEj5Fix12IiE(0x14, 0x15666);
        break;
    case 1:
        *(unsigned char*)(c + 0x5ce) += 1;
        break;
    case 2: {
        int v[3];
        unsigned m;
        if (_ZN6Player12GetTalkStateEv(*(void**)(c + 0x488)) != 0)
            return;
        v[0] = -*(int*)(c + 0x5c);
        v[1] = *(int*)(c + 0x60) + 0xc8000;
        v[2] = *(int*)(c + 0x64);
        m = *(short*)(&data_ov063_0211e1c8 + 2 * (*(int*)(*(char**)(c + 0x488) + 8)));
        if (!_ZN6Player11ShowMessageER9ActorBasejPK7Vector3jj(*(void**)(c + 0x488), c, m, v, 0, 2))
            return;
        func_0201277c(0x151);
        *(unsigned char*)(c + 0x5ce) += 1;
        break;
    }
    case 3:
        if (_ZN6Player12GetTalkStateEv(*(void**)(c + 0x488)) != -1)
            return;
        *(unsigned char*)(c + 0x5cc) = 5;
        _ZN5Sound22StopLoadedMusic_Layer3Ev();
        _ZN5Sound17ChangeMusicVolumeEj5Fix12IiE(0x7f, 0x7222);
        func_02011cfc();
        *(unsigned short*)(c + 0x5d4) &= ~0x400;
        *(int*)(*(char**)&CAMERA + 0x154) &= ~8;
        break;
    }
}
