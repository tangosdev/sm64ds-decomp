struct Ent_ov006 {
    int x;
    int y;
    int f08;
    int f0c;
    int f10;
    int f14;
    short f18;
    short f1a;
    short f1c;
    short f1e;
};

extern int data_ov006_021404c4;
extern struct Ent_ov006 data_ov006_021404d8[3];

void func_ov006_020c8c78(int x, int y)
{
    int i;
    int v;

    if (data_ov006_021404c4 >= 3) {
        return;
    }
    for (i = 0; i < 3; i++) {
        if (data_ov006_021404d8[i].f18 != 0) {
            continue;
        }
        data_ov006_021404d8[i].f1a = data_ov006_021404c4;
        data_ov006_021404d8[i].x = x << 12;
        v = data_ov006_021404d8[i].x;
        if (v < 0x20000) {
            v = 0x20000;
        } else if (v > 0xE0000) {
            v = 0xE0000;
        }
        data_ov006_021404d8[i].x = v;
        data_ov006_021404d8[i].y = y << 12;
        data_ov006_021404d8[i].f08 = 0;
        data_ov006_021404d8[i].f0c = -0x400;
        data_ov006_021404d8[i].f18 = 1;
        data_ov006_021404d8[i].f1c = 0;
        data_ov006_021404d8[i].f14 = 0x1000;
        data_ov006_021404d8[i].f10 = 0x30000;
        data_ov006_021404d8[i].f1e = 0x5A;
        data_ov006_021404c4++;
        return;
    }
}
