extern void func_ov004_020b2444(int a1, int a2, int num, int a4, int a5, int sel, int idx);
extern int func_ov004_020adbc0(void);
extern void func_ov004_020b2220(int a1, int a2, int a3, int a4, int a5, int a6, int a7);

typedef struct {
    int a;
    int b;
    unsigned short c;
    unsigned char p0[3];
    unsigned char flag;
    unsigned char p1[2];
} Elem;

typedef struct {
    unsigned char _pad[0x473c];
    Elem arr[5];
    unsigned char _pad2[0x4ee8 - 0x473c - 5*16];
    unsigned char go;
} Obj;

void func_ov006_020e1554(Obj *o)
{
    int i;
    for (i = 0; i < 5; i++) {
        if (o->arr[i].flag != 0) {
            func_ov004_020b2444(o->arr[i].a >> 12, o->arr[i].b >> 12, o->arr[i].c, -1, -1, 0, 0);
        }
    }
    if (o->go != 0) {
        int r = func_ov004_020adbc0();
        func_ov004_020b2220(0x80, 0x60, r, 1, 0, 0x800, 0);
    }
}
