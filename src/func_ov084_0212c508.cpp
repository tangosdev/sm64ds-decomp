//cpp
typedef short s16;
typedef unsigned short u16;
typedef signed char s8;
typedef unsigned char u8;
struct Vector3 { int x, y, z; };
extern "C" s16 Vec3_HorzAngle(const struct Vector3 *v0, const struct Vector3 *v1);
extern "C" int func_ov084_0212cac0(char *c);
extern "C" int IsCannonOpenInCurLevel(void);
extern "C" void func_ov084_0212c960(char *c, int i);
extern "C" int _ZN6Player12GetTalkStateEv(char *c);
extern "C" int _Z14ApproachLinearRsss(s16 *a, s16 b, s16 c);
extern "C" int func_ov084_0212caa8(char *c);
extern "C" s8 NumRedCoins(void);
extern "C" void func_ov084_0212c9f0(char *c, int arg1, unsigned int arg2);
extern "C" int ObjectMessageIDToActualMessageID(int id);
extern "C" int func_ov084_0212ca60(char *c);
extern "C" void func_02012790(int a);
extern int data_0209caa0[];
extern u8 data_0209f288;
extern s8 data_0209f2f8;
extern u8 data_0209d660;
extern u8 data_0209d6bc;
extern u16 data_0209d6d4;
extern u8 data_0209f284;

extern "C" void func_ov084_0212c508(char *self)
{
    char *r5 = *(char **)(self + 0x194);
    int r4 = 0;
    int t = *(int *)(self + 8);
    s16 r7;
    struct Vector3 pv;
    int r1;
    u16 r4b;
    if (t != 0xffff)
        r4 = (u16)t;
    {
        int *p = (int *)(unsigned)((unsigned long long)(unsigned)(r5 + 0x5c));
        int a = p[0];
        pv.x = a;
        int b = p[1];
        pv.y = b;
        int c = p[2];
        pv.z = c;
    }
    r7 = Vec3_HorzAngle((struct Vector3 *)(self + 0x5c), &pv);

    if (func_ov084_0212cac0(self) != 0) {
        if (IsCannonOpenInCurLevel() == 0) {
            func_ov084_0212c960(self, 2);
            return;
        }
    }

    switch (_ZN6Player12GetTalkStateEv(r5)) {
    case 0:
        if (_Z14ApproachLinearRsss((s16 *)(self + 0x8e), r7, 0x800) != 0) {
            if (func_ov084_0212caa8(self) != 0) {
                if ((data_0209caa0[2] & 0x8000) == 0) {
                    data_0209caa0[2] |= 0x8000;
                    r1 = 0x15b;
                } else if (NumRedCoins() <= 3) {
                    r1 = 0x15c;
                } else if (NumRedCoins() <= 6) {
                    r1 = 0x15d;
                } else if (NumRedCoins() == 7) {
                    r1 = 0x15e;
                } else {
                    r1 = 0x15f;
                }
                func_ov084_0212c9f0(self, r1, 0);
                data_0209f288 = 1;
            } else if (func_ov084_0212cac0(self) != 0 && IsCannonOpenInCurLevel() != 0) {
                if (*(u8 *)(self + 0x1ea) == 0) {
                    if (data_0209f2f8 == 6)
                        r1 = 0x8f;
                    else
                        r1 = 0x14a;
                } else {
                    if (data_0209f2f8 == 6)
                        r1 = 0x90;
                    else
                        r1 = 0x14b;
                }
                *(u8 *)(self + 0x1ea) = 1;
                func_ov084_0212c9f0(self, r1, 0);
            } else {
                r1 = ObjectMessageIDToActualMessageID((int)(s16)r4);
                r1 = r1 + *(int *)(r5 + 8);
                func_ov084_0212c9f0(self, (u16)r1, 0);
            }
        }
        break;
    case 1:
        break;
    default:
        func_ov084_0212c960(self, 0);
        break;
    }

    if (data_0209d660 == 0)
        return;

    if (data_0209d6bc == 9) {
        if (func_ov084_0212caa8(self) != 0)
            data_0209f288 = 0;
    }

    {
        u8 d6bc = data_0209d6bc;
        if (*(u16 *)(self + 0x1ec) != d6bc) {
            if (d6bc == 3)
                goto do_inc;
            if (d6bc != 9)
                goto skip_inc;
        do_inc:
            {
                u8 *p = (u8 *)(((long long)(int)(self + 0x1eb)) & 0xFFFFFFFFFFFFFFFFLL);
                *p = (u8)(*p + 1);
            }
        skip_inc:
            ;
        }
    }

    r4b = data_0209d6d4;
    if (r4b == 0x15c || r4b == 0x15e || func_ov084_0212ca60(self) != 0) {
        if (*(u8 *)(self + 0x1eb) == 0)
            data_0209f284 = 1;
        else
            data_0209f284 = 0;
    }

    if (r4b == 0x15b || r4b == 0x15d) {
        if (*(u8 *)(self + 0x1eb) == 1)
            data_0209f284 = 1;
        else
            data_0209f284 = 0;
    }

    if (r4b == 0x8a) {
        if (*(u8 *)(self + 0x1eb) == 1)
            data_0209f284 = 1;
        if (data_0209d6bc == 9)
            data_0209f284 = 0;
    }

    if (*(u8 *)(self + 0x1ee) != data_0209f284 && data_0209f284 != 0)
        func_02012790(0x24);

    *(u8 *)(self + 0x1ee) = data_0209f284;
    *(u16 *)(self + 0x1ec) = data_0209d6bc;
}
