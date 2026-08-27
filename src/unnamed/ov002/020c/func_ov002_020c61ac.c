extern int _ZN6Player7IsStateERNS_5StateE(void *thiz, void *st);
extern void _ZN6Player11ChangeStateERNS_5StateE(void *thiz, void *st);
extern void func_ov002_020de968(void *c);
extern void func_ov002_020bdd2c(void *c);
extern int func_ov002_020c607c(void *c, int a, int b, int *out);
extern void _ZN9dBgCh_GndC1Ev(void *p);
extern void _ZN5dBgCh19StartDetectingWaterEv(void *p);
extern void _ZN9dBgCh_Gnd12SetObjAndPosERK7Vector3P8dActor_c(void *p, void *v, void *actor);
extern int _ZN9dBgCh_Gnd10DetectClsnEv(void *p);
extern void _ZN9dBgCh_GndD1Ev(void *p);

extern char data_ov002_021102ec;
extern char data_ov002_021106dc;
extern char data_ov002_021106f4;
extern char data_ov002_021102bc;
extern char data_ov002_0210ffec;
extern char data_ov002_0211001c;
extern char data_ov002_021101b4;

int func_ov002_020c61ac(char *c) {
    int outA;
    int outB;
    int v[3];
    int obj[0x14];
    int saved;
    int a;
    int b;

    if ((*(unsigned char *)(c + 0x6e9) & 5) == 0)
        return 0;
    if (_ZN6Player7IsStateERNS_5StateE(c, &data_ov002_021102ec) ||
        _ZN6Player7IsStateERNS_5StateE(c, &data_ov002_021106dc) ||
        _ZN6Player7IsStateERNS_5StateE(c, &data_ov002_021106f4) ||
        _ZN6Player7IsStateERNS_5StateE(c, &data_ov002_021102bc) ||
        _ZN6Player7IsStateERNS_5StateE(c, &data_ov002_0210ffec) ||
        _ZN6Player7IsStateERNS_5StateE(c, &data_ov002_0211001c) ||
        *(unsigned char *)(c + 0x706) != 0)
        return 0;
    if ((*(unsigned char *)(c + 0x6e9) & 5) == 5) {
        if (*(unsigned char *)(c + 0x6fd) != 0 || *(unsigned char *)(c + 0x703) != 0) {
            int d;
            int yv, zv, xv;
            zv = *(int *)(c + 0x64);
            d = 0xc8000;
            if (*(unsigned char *)(c + 0x6fd) != 0)
                d = 0x96000;
            yv = *(int *)(c + 0x60) + d;
            xv = *(int *)(c + 0x5c);
            v[0] = xv;
            v[1] = yv;
            v[2] = zv;
            _ZN9dBgCh_GndC1Ev(obj);
            _ZN5dBgCh19StartDetectingWaterEv(obj);
            _ZN9dBgCh_Gnd12SetObjAndPosERK7Vector3P8dActor_c(obj, v, c);
            if (_ZN9dBgCh_Gnd10DetectClsnEv(obj)) {
                *(int *)(c + 0x644) = *(int *)((char *)obj + 0x44);
                *(int *)(c + 0x60) = *(int *)(c + 0x644);
            }
            if (*(unsigned char *)(c + 0x6fd) != 0) {
                func_ov002_020de968(c);
                _ZN6Player11ChangeStateERNS_5StateE(c, &data_ov002_021101b4);
            } else {
                func_ov002_020bdd2c(c);
                _ZN6Player11ChangeStateERNS_5StateE(c, &data_ov002_021101b4);
                _ZN9dBgCh_GndD1Ev(obj);
                return 1;
            }
            _ZN9dBgCh_GndD1Ev(obj);
        }
    }
    a = func_ov002_020c607c(c, *(int *)(c + 0x60) - 0x20000, *(int *)(c + 0x60) + 0xa0000, &outA);
    saved = *(int *)(c + 0x36c);
    if (outA == 0x80000000)
        return 0;
    b = func_ov002_020c607c(c, *(int *)(c + 0x60) + 0x1e000, *(int *)(c + 0x60) - 0x28000, &outB);
    if (outB == 0x80000000)
        return 0;
    if (saved == *(int *)(c + 0x36c))
        return 0;
    if (outA + 0x20000 < outB)
        return 0;
    {
        int t = outA - outB;
        if (t < 0)
            t = -t;
        if (t >= 0xa0000)
            return 0;
    }
    if ((a | b) == 0)
        return 0;
    *(int *)(c + 0x60) = outB;
    *(unsigned char *)(c + 0x6f2) = 0;
    _ZN6Player11ChangeStateERNS_5StateE(c, &data_ov002_021102ec);
    if (a != 0) {
        *(unsigned char *)(c + 0x6e5) = 1;
        *(unsigned char *)(c + 0x6e6) = (unsigned char)a;
    } else if (b != 0) {
        *(unsigned char *)(c + 0x6e5) = 2;
        *(unsigned char *)(c + 0x6e6) = (unsigned char)b;
    }
    return 1;
}
