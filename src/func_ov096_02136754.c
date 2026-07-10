typedef unsigned short u16;
typedef short s16;
typedef long long s64;
extern s16 data_02082214[];
extern void func_ov096_02136928(char* c, int n);
extern void func_ov096_02135948(char* c);
extern void _ZN12CylinderClsn5ClearEv(char* p);
extern void _ZN12CylinderClsn6UpdateEv(char* p);

int func_ov096_02136754(char* self)
{
    char *other = *(char**)(self + 0x394);
    if (!other) {
        func_ov096_02136928(self, 1);
    } else {

        char *p = self + 0x60;
        int sp, t, a0, z, lim, b;
        s16 *tbl, c, sn;
        u16 ang;

        *(int*)(self+0x5c) = *(int*)(other+0x5c);
        other = *(char**)(self+0x394);
        t = *(int*)(other+0x64);
        sp = 0xe000;
        *(int*)(self+0x64) = t;

        t = *(int*)(self+0xa8) - 0x2000;
        a0 = *(int*)(self+0xa0);
        if (t >= a0) a0 = t;
        *(int*)(self+0xa8) = a0;
        *(int*)p += *(int*)(self+0xa8);
        other = *(char**)(self+0x394);
        t = *(int*)(self+0x60);
        lim = *(int*)(other+0x60) + 0x6e000;
        if (t < lim) {
            *(int*)(self+0x60) = lim;
            *(int*)(self+0xa8) = 0;
        }
        other = *(char**)(self+0x394);
        tbl = data_02082214;
        c = *(s16*)(other+0x8e);
        z = 0;
        *(s16*)(self+0x8e) = c;
        other = *(char**)(self+0x394);
        other = other + 0x300;
        c = *(s16*)(other+0xaa);
        b = 0x800;
        c = (s16)(c + 0x13000);
        *(s16*)(self + 0x3aa) = c;
        ang = *(u16*)(self + 0x3aa);
        c = tbl[(ang>>4)*2];
        *(int*)(self+0x378) = (int)(((s64)c * sp + b) >> 12);
        *(int*)(self+0x37c) = z;
        ang = *(u16*)(self + 0x3aa);
        sn = tbl[(ang>>4)*2+1];
        *(int*)(self+0x380) = (int)(((s64)sn * sp + b) >> 12);

    }
    func_ov096_02135948(self);
    _ZN12CylinderClsn5ClearEv(self + 0x14c);
    _ZN12CylinderClsn6UpdateEv(self + 0x14c);
    return 1;
}
