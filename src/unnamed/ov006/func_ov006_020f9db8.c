struct A {
    char pad0[0xc];
    int f0c;
    int f10;
    int f14;
    int f18;
    int f1c;
    int f20;
    char pad24[6];
    short f2a;
    char pad2c;
    unsigned char f2d;
};

extern void Vec2_Sub(int* o, int* a, int* b);
extern void func_0203d630(int *p, int m);

extern short data_ov006_0213d6f4[];
extern struct A *data_ov006_02142578;
extern short data_ov006_0214255c[];

void func_ov006_020f9db8(struct A *o, int a)
{
    int v[3];

    if (a >= 0x14)
        return;

    if (o->f2a >= 0x14) {
        o->f0c = (0x70 - ((data_ov006_0213d6f4[0] >> 2) << 1)) << 12;
        o->f10 = -0x30000;
        data_ov006_02142578 = o;
        o->f2d = 1;
    } else if (o->f2d == 0) {
        o->f0c = (0x70 - ((data_ov006_0213d6f4[0] >> 2) << 1)) << 12;
        o->f10 = -0x30000;
        o->f2d = 4;
        data_ov006_0214255c[0]++;
    } else {
        o->f2d = 4;
        data_ov006_0214255c[0]++;
    }

    o->f14 = ((a % 5) * 32 + 0x30) << 12;
    o->f18 = ((a / 5) * 0x30) << 12;
    o->f2a = (short)a;

    Vec2_Sub(v, &o->f14, &o->f0c);

    o->f1c = v[0];
    o->f20 = v[1];
    func_0203d630(&o->f1c, 0x124);

    if (o->f1c < 0)
        o->f1c = -o->f1c;
    if (o->f20 < 0)
        o->f20 = -o->f20;
}
