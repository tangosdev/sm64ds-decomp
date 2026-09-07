//cpp
#pragma opt_common_subs off
typedef short s16;
typedef unsigned short u16;
typedef long long s64;

extern "C" {
int func_ov004_020b8f18(void *c);
int GetGameLanguage(void);
int func_02053200(int x);
int func_ov004_020aea78(void *a, void *b, void *c, void *d);
void func_ov004_020aea30(void *self, int a1, int a2, int a3);
}

extern int *data_ov004_020bc020[];
extern int *data_ov004_020bbfbc[];
extern int *data_ov004_020bbfd0[];
extern s16 data_02082214[];
extern int data_ov006_021346bc;

extern "C" void func_ov004_020ae858(char *self)
{
    int extra[3];
    s16 *tbl[3];
    int buf[4];
    int zero1;
    int zero2;
    int zero0;
    int i;

    if (func_ov004_020b8f18((void *)(self + 0xf4)) != 0)
        return;

    extra[0] = *data_ov004_020bc020[GetGameLanguage()];
    extra[1] = *data_ov004_020bbfbc[GetGameLanguage()];
    extra[2] = *data_ov004_020bbfd0[GetGameLanguage()];

    tbl[0] = (s16 *)(self + 0x4634);
    tbl[1] = (s16 *)(self + 0x4638);
    tbl[2] = (s16 *)(self + 0x463c);

    i = 0;
    zero1 = 0;
    zero2 = 0;
    zero0 = 0;

    for (; i < 3; i++) {
        if (i == *(s16 *)(self + 0x4646)) {
            unsigned int field = *(unsigned int *)(self + 0x4640);
            unsigned int shifted = (field << 0xf) >> 0x10;
            int idx0 = (int)shifted >> 4;
            int s0 = data_02082214[idx0 * 2];
            char *cursor = (char *)extra[i];
            int arg0;
            int result;
            s16 *pos;
            u16 v;
            int *pbuf = buf;

            arg0 = (int)((((s64)s0 << 11) + 0x800) >> 12) + 0x1000;
            result = func_02053200(arg0);

            {
                volatile int *vp = pbuf;
                vp[0] = 0;
                vp[1] = 0;
                vp[2] = 0;
                vp[3] = 0;
            }
            buf[0] = result;
            buf[3] = result;

            pos = tbl[i];
            do {
                func_ov004_020aea78((void *)cursor, (void *)(int)pos[0], (void *)(int)pos[1], (void *)pbuf);
                v = *(u16 *)(cursor + 6);
                cursor += 8;
            } while (v != 0xffff);

            pos = tbl[i];
            func_ov004_020aea30((void *)data_ov006_021346bc, pos[0], pos[1], zero0);
        } else {
            s16 *pos = tbl[i];
            char *ep = (char *)extra[i];
            func_ov004_020aea30((void *)ep, pos[0], pos[1], zero1);
            func_ov004_020aea30((void *)data_ov006_021346bc, pos[0], pos[1], zero2);
        }
    }
}
