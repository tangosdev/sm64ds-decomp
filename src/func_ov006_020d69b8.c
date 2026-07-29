extern int func_020126e8(int a);
extern int func_02012468(int a, int b, int c, int d, int e, int f, int g, short h);
extern void func_02012718(void *a, int b);

extern unsigned char *data_ov006_0213bb08[];
extern unsigned char data_ov006_0213b9bc[];

struct Ent {
    int f660;
    char pad1[0x20];
    int f684;
    char pad2[6];
    unsigned short f68e;
    char pad3[4];
    unsigned char f694;
    unsigned char f695;
    unsigned char f696;
    unsigned char f697;
    unsigned char f698;
    unsigned char f699;
    char pad4[6];
};

struct Gbl {
    char pad0[0x2d0];
    int f2d0;
};

#define E (((struct Ent *)(a + 0x4000 + 0x660))[i])
#define G ((struct Gbl *)(a + 0x6000))

void func_ov006_020d69b8(char *a, int i)
{
    int type;
    int frame;
    int max;

    *(unsigned short *)(a + 0x468e + (i << 6)) += 1;
    type = E.f694;
    frame = E.f695;

    if (type != 3 && type != 0 && G->f2d0 != 3) {
        E.f684 = func_02012468(E.f684, 2, 0x1db, 4, 0, 0,
                               func_020126e8(E.f660), 0);
    }

    if (E.f68e >= data_ov006_0213bb08[type][frame]) {
        frame++;
        E.f695 = frame;
        E.f68e = 0;
        if (type != 3 && E.f697 != 2 && (type != 0 || E.f697 != 5)) {
            if (E.f698 == 1 && G->f2d0 != 3) {
                if (E.f696 != 0) {
                    func_02012718((void *)0x1d9, E.f660);
                } else {
                    func_02012718((void *)0x1da, E.f660);
                }
            }
        }
        if (type == 3 && frame == 1) {
            func_02012718((void *)0x1e1, E.f660);
        }
    }

    max = data_ov006_0213b9bc[type];
    if (frame < max) {
        return;
    }
    if (type == 3) {
        E.f698 = 0;
        E.f699 = 0;
    } else {
        max = 0;
    }
    E.f695 = max;
}
