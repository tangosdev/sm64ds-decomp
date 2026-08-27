extern void func_ov006_020d68a8(char *o, int a1);
extern void func_ov006_020d6e8c(char* self, int idx);
extern void func_ov006_020d6d7c(char *this, int idx);
extern short data_02082214[];

/* Entity slot: 0x40-byte records based at self+0x4660, indexed by idx. */
struct Ent {
    int x;                  /* 0x00 */
    int y;                  /* 0x04 */
    char pad08[8];
    int spd;                /* 0x10 */
    char pad14[8];
    int px;                 /* 0x1c */
    int py;                 /* 0x20 */
    char pad24[8];
    unsigned short ang;     /* 0x2c */
    unsigned short f2e;     /* 0x2e */
    char pad30[2];
    unsigned short h;       /* 0x32 */
    unsigned char b34;      /* 0x34 */
    unsigned char b35;      /* 0x35 */
    char pad36[1];
    unsigned char b37;      /* 0x37 */
    char pad38[2];
    unsigned char b3a;      /* 0x3a */
    char pad3b[2];
    unsigned char b3d;      /* 0x3d */
    char pad3e[2];
};

struct S32 { int v; char pad[0x3c]; };
struct S16 { unsigned short v; char pad[0x3e]; };

void func_ov006_020d816c(char *self, int idx)
{
    int t;

    if (((struct Ent *)(self + 0x4660))[idx].h != 0) {
        if (*(unsigned char *)(self + 0x62fb) == 0) {
            ((struct S16 *)(self + 0x4692))[idx].v--;
        }
        if ((short)((struct Ent *)(self + 0x4660))[idx].h < 0) {
            ((struct Ent *)(self + 0x4660))[idx].h = 0;
        }
        if (((struct Ent *)(self + 0x4660))[idx].h <= 0x40) {
            ((struct Ent *)(self + 0x4660))[idx].b34 = 2;
        }
    } else {
        ((struct Ent *)(self + 0x4660))[idx].h = 0;
        ((struct Ent *)(self + 0x4660))[idx].b37 = 4;
        ((struct Ent *)(self + 0x4660))[idx].b34 = 3;
        ((struct Ent *)(self + 0x4660))[idx].b35 = 0;
        ((struct Ent *)(self + 0x4660))[idx].f2e = 0;
        ((struct Ent *)(self + 0x4660))[idx].b3a = 0;
        ((struct Ent *)(self + 0x4660))[idx].b3d = 1;
        func_ov006_020d68a8(self, idx);
        return;
    }

    if (*(unsigned char *)(self + 0x62fb) != 0) {
        return;
    }

    t = data_02082214[((int)((struct Ent *)(self + 0x4660))[idx].ang >> 4) * 2 + 1];
    ((struct S32 *)(self + 0x4660))[idx].v += (int)(((long long)t * ((struct Ent *)(self + 0x4660))[idx].spd + 0x800) >> 12);
    t = data_02082214[((int)((struct Ent *)(self + 0x4660))[idx].ang >> 4) * 2];
    ((struct S32 *)(self + 0x4664))[idx].v += (int)(((long long)t * ((struct Ent *)(self + 0x4660))[idx].spd + 0x800) >> 12);
    func_ov006_020d6e8c(self, idx);
    func_ov006_020d6d7c(self, idx);
    ((struct Ent *)(self + 0x4660))[idx].px = ((struct Ent *)(self + 0x4660))[idx].x;
    ((struct Ent *)(self + 0x4660))[idx].py = ((struct Ent *)(self + 0x4660))[idx].y;
}
