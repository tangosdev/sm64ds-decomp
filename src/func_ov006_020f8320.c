extern void func_0203d6d0(int* o, int* a, int* b);
extern void func_0203d630(int *p, int m);

extern int data_ov006_0213d56c;
extern int data_ov006_02142504;
extern int data_ov006_021424f4;

struct Obj {
    char pad_00[0xc];
    int f_0c;
    int f_10;
    int f_14;
    int f_18;
    int f_1c;
    int f_20;
    int f_24;
    char pad_28[2];
    short f_2a;
    unsigned char f_2c;
    unsigned char f_2d;
};

void func_ov006_020f8320(struct Obj* o, int m)
{
    int tmp[3];

    if (m >= 0x14) {
        return;
    }

    if (o->f_2a >= 0x14) {
        o->f_0c = (0x70 - ((data_ov006_0213d56c >> 2) << 1)) << 12;
        o->f_10 = -0x30000;
        *(struct Obj**)&data_ov006_02142504 = o;
        o->f_2d = 1;
    } else if (o->f_2d == 0) {
        o->f_0c = (0x70 - ((data_ov006_0213d56c >> 2) << 1)) << 12;
        o->f_10 = -0x30000;
        o->f_2d = 4;
        data_ov006_021424f4++;
    } else {
        o->f_2d = 4;
        data_ov006_021424f4++;
    }

    o->f_14 = ((m % 5) * 32 + 0x30) << 12;
    o->f_18 = ((m / 5) * 0x30) << 12;
    o->f_2a = (short)m;

    func_0203d6d0(tmp, &o->f_14, &o->f_0c);
    o->f_1c = tmp[0];
    o->f_20 = tmp[1];
    func_0203d630(&o->f_1c, 0x124);

    if (o->f_1c < 0) {
        o->f_1c = -o->f_1c;
    }
    if (o->f_20 < 0) {
        o->f_20 = -o->f_20;
    }
}
