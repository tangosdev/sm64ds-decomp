typedef unsigned char u8;
typedef unsigned short u16;

extern u8 data_ov006_0212ee30[];

struct Slot {
    int f00; int f04; int f08; int f0c; int f10; int f14;
    u16 f18; u16 f1a;
    u8 f1c; u8 f1d; u8 f1e; u8 f1f; u8 f20; u8 f21; u8 f22; u8 f23;
};

struct Mgr { u8 pad[0x51b0]; struct Slot slot[30]; };

void func_ov006_0211b654(struct Mgr* m, int n)
{
    int k;
    int i;
    u8* g = data_ov006_0212ee30;
    int* s00 = (int*)((char*)m + n * 0x14 + 0x50e8);
    int* s04 = (int*)((char*)m + n * 0x14 + 0x50ec);

    for (k = 0; k < 3; g++, k++) {
        for (i = 0; i < 30; i++) {
            if (m->slot[i].f1c == 0) {
                m->slot[i].f1c = 1;
                m->slot[i].f00 = *s00;
                int* d04 = (int*)(((int)m + i * 0x24 + 0x51b4));
                *d04 = *s04 - 0x8000;
                u16* d18 = (u16*)(((int)m + i * 0x24 + 0x51c8));
                m->slot[i].f10 = *d04;
                m->slot[i].f1e = *g;
                m->slot[i].f22 = *g;
                *d18 = 0;
                if (*(u8*)((char*)m + n * 0x14 + 0x50f8) == 7) {
                    if (k >= 3) { *d18 = 0x10; } else { *d18 = 8; }
                }
                m->slot[i].f14 = 0x1000;
                m->slot[i].f08 = 0;
                m->slot[i].f0c = 0;
                m->slot[i].f20 = 0;
                m->slot[i].f21 = 0;
                break;
            }
        }
    }
}
