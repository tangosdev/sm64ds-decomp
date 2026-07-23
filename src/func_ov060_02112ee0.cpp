//cpp
extern "C" {
extern int _ZN6Player9StartTalkER9ActorBaseb(void *pl, void *a, int b);
extern int _ZN6Player12GetTalkStateEv(void *pl);
extern unsigned char NumStars(void);
extern int _ZN6Player11ShowMessageER9ActorBasejPK7Vector3jj(
    void *pl, void *a, unsigned m, void *v, unsigned d, unsigned e);
extern void _ZN7Message11PrepareTalkEv(void);
extern void _ZN5Sound22StopLoadedMusic_Layer1Ej(unsigned a);
extern void func_ov060_021135fc(void *c);
extern void func_ov060_02111cc0(void *c, int a, int b, int d);
extern void *_ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(
    unsigned a, unsigned b, int x, int y, int z, void *v, void *cb);
extern void func_ov060_02113260(void *c);
}

#define LAUND(p) ((void *)((((long long)(int)(p)) & 0xFFFFFFFFFFFFFFFFLL)))

extern "C" int func_ov060_02112ee0(void *cc)
{
    char *c = (char *)cc;
    int ret = 0;
    unsigned short mode =
        *(unsigned short *)((char *)LAUND(c + 0x300) + 0xfe);
    volatile int v[4];

    if (mode <= 1) {
        if (mode == 0) {
            unsigned short *op = (unsigned short *)LAUND(c + 0x3fe);
            *op = *op + 1;
            *(unsigned char *)(c + 0x424) = 0;
        }

        switch (*(unsigned char *)(c + 0x424)) {
        case 0:
            if (_ZN6Player9StartTalkER9ActorBaseb(
                    *(void **)(c + 0x3a0), c, 1)) {
                unsigned char *p =
                    (unsigned char *)LAUND(c + 0x424);
                *p = *p + 1;
            }
            break;

        case 1:
            if (_ZN6Player12GetTalkStateEv(*(void **)(c + 0x3a0)) == 0) {
                unsigned m = (NumStars() != 0x96) ? 0xd1 : 0xd2;
                if (_ZN6Player11ShowMessageER9ActorBasejPK7Vector3jj(
                        *(void **)(c + 0x3a0), c, m, c + 0x5c, 0, 2)) {
                    unsigned char *p =
                        (unsigned char *)LAUND(c + 0x424);
                    *p = *p + 1;
                    _ZN7Message11PrepareTalkEv();
                }
            }
            break;

        case 2:
            if (_ZN6Player12GetTalkStateEv(*(void **)(c + 0x3a0)) == -1) {
                _ZN5Sound22StopLoadedMusic_Layer1Ej(0x3c);
                func_ov060_021135fc(c);

                unsigned short *op =
                    (unsigned short *)LAUND(c + 0x3fe);
                unsigned char *ip =
                    (unsigned char *)LAUND(c + 0x424);
                *op = *op + 1;
                int t = *ip + 1;
                *ip = t;
                func_ov060_02111cc0(c, 4, 0x40000000, t);
            }
            break;
        }
    } else {
        if (*(unsigned char *)(c + 0x41c) > 4) {
            *(unsigned char *)LAUND(c + 0x41c) -= 4;
            v[0] = *(int *)(c + 0x5c);
            v[1] = *(int *)(c + 0x60);
            v[2] = *(int *)(c + 0x64);
            v[1] = *(int *)(c + 0x60) + 0x32000;

            *(void **)(c + 0x448) =
                _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(
                    *(volatile unsigned *)(c + 0x448), 0x99,
                    v[0], v[1], *(int *)(c + 0x64), 0, 0);
        } else {
            func_ov060_02113260(c);
            ret = 1;
        }
    }

    return ret;
}
