//cpp
#include "dBgCh_Lin.h"

typedef struct {
    void *tag;
    int f04, f08, f0c, f10, f14;
    unsigned short f18, f1a;
    int f1c, f20, f24;
} ClsnResultTmp;

extern "C" {
extern int _ZN6Player7IsInAirEv(void *self);
extern int _ZN6Player7IsStateERNS_5StateE(void *self, void *st);
extern void _ZN6Player11ChangeStateERNS_5StateE(void *self, void *st);
extern void _ZN9dBgCh_Lin10GetClsnPosEv(Vector3 *ret, dBgCh_Lin *self);
extern void func_02035414(void *bgch);
extern void func_02035428(void *bgch);
extern int func_02037e38(unsigned int *p);
extern unsigned _ZNK5dBgPi9GetClsnIDEv(void *self);
extern void *_ZN8dActor_c10FindWithIDEj(unsigned id);
extern void _ZN5dBgPiD1Ev(void *self);

extern char data_ov002_0211001c;
extern int data_02099368;
}

extern "C" int func_ov002_020cef84(char *self)
{
    Vector3 v1, v2, cp;
    ClsnResultTmp tmp;
    int lim;

    dBgCh_Lin line;
    if (!_ZN6Player7IsInAirEv(self) || *(unsigned char *)(self + 0x706) != 0 ||
        *(int *)(self + 0xa8) <= 0 || _ZN6Player7IsStateERNS_5StateE(self, &data_ov002_0211001c) ||
        *(unsigned char *)(self + 0x703) != 0 || *(unsigned char *)(self + 0x708) != 0 ||
        *(unsigned char *)(self + 0x709) != 0) {
        return 0;
    }

    v1.x = *(int *)(self + 0x5c);
    int y1 = *(int *)(self + 0x60);
    v1.y = y1;
    v1.z = *(int *)(self + 0x64);
    v2.x = *(int *)(self + 0x5c);
    int y2 = *(int *)(self + 0x60);
    v2.y = y2;
    v2.z = *(int *)(self + 0x64);
    v1.y = y1 + 0x64000;
    v2.y = y2 + 0x96000;
    lim = 0x8c000;
    {
        int t = *(int *)(self + 0x358) != 0;
        if (t != false) {
            if (*(int *)(self + 8) == 2) {
                v2.y += 0x32000;
                lim = 0xb4000;
            }
        }
    }
    func_02035414(&line);
    if (*(unsigned char *)(self + 0x6fb) != 0)
        func_02035428(&line);
    line.SetObjAndLine(v1, v2, (dActor_c*)self);
    if (line.DetectClsn()) {
        _ZN9dBgCh_Lin10GetClsnPosEv(&cp, &line);
        if (func_02037e38((unsigned int *)&line.surface) == 3) {
            int t2 = *(int *)(self + 0x358) != 0;
            if (t2 == false) {
                _ZN6Player11ChangeStateERNS_5StateE(self, &data_ov002_0211001c);
                *(int *)(self + 0x60) = cp.y - 0x78000;
                return 1;
            }
        }
        {
            int d = cp.y - *(int *)(self + 0x60);
            if (d < 0) d = -d;
            if (d < lim) {
                void *a;
                int *dst = &tmp.f04;
                int w0, w1;
                *(int *)(self + 0xa8) = -0x1000;
                *(unsigned char *)(self + 0x6e9) |= 8;
                w0 = *(int *)((char *)&line + 0x14);
                w1 = *(int *)((char *)&line + 0x18);
                dst[0] = w1 ? w0 : w0;
                dst[1] = w1;
                dst[2] = *(int *)((char *)&line + 0x1c);
                dst[3] = *(int *)((char *)&line + 0x20);
                dst[4] = *(int *)((char *)&line + 0x24);
                tmp.tag = &data_02099368;
                tmp.f18 = *(unsigned short *)((char *)&line + 0x28);
                tmp.f1a = *(unsigned short *)((char *)&line + 0x2a);
                tmp.f1c = *(int *)((char *)&line + 0x2c);
                tmp.f20 = *(int *)((char *)&line + 0x30);
                tmp.f24 = *(int *)((char *)&line + 0x34);
                a = _ZN8dActor_c10FindWithIDEj(_ZNK5dBgPi9GetClsnIDEv(&tmp));
                if (a)
                    (*(void (**)(void *, char *))(*(int *)a + 0x70))(a, self);
                _ZN5dBgPiD1Ev(&tmp);
            }
        }
    }
    return 0;
}
