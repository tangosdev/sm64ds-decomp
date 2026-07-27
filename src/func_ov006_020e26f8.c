extern unsigned char data_020a0e40;
typedef struct TblEnt { unsigned char v; unsigned char pad[3]; } TblEnt;
extern TblEnt data_020a0de8[];
extern TblEnt data_020a0dea[];
extern TblEnt data_020a0deb[];

typedef struct Entry { int a; int b; int pad[3]; int m; int n; int pad2[3]; unsigned char f28; unsigned char pad3[3]; } Entry;
typedef struct Work { unsigned char pad[0x4660]; Entry e[48]; unsigned char pad2[0x10]; int gx; int gy; } Work;

void func_ov006_020e26f8(Work *w, int i)
{
    unsigned char idx = data_020a0e40;
    if (data_020a0de8[idx].v) {
        int t, mm, nn;
        w->e[i].a = w->e[i].m + (data_020a0dea[idx].v << 12);
        t = w->e[i].a >> 12;
        if (t < 0xe) w->e[i].a = 0xe000;
        if (t > 0xf2) w->e[i].a = 0xf2000;
        mm = (w->e[i].a >> 12) - data_020a0dea[data_020a0e40].v;
        nn = (w->e[i].b >> 12) - data_020a0deb[idx].v;
        w->e[i].m = mm << 12;
        w->e[i].n = nn << 12;
    } else {
        int dx, dy;
        w->e[i].f28 = 0;
        dx = (w->gx - w->e[i].a) >> 12;
        dy = (w->gy - w->e[i].b) >> 12;
        if (dx < -0x2e) return;
        if (dx > 0x2e) return;
        if (dy < -0x14) return;
        if (dy <= 0x14) w->gy = w->e[i].b + 0x15000;
    }
}
