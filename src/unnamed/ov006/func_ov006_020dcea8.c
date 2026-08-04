struct E {
    int x;
    int y;
    char pad8[8];
    unsigned char b10;
    unsigned char b11;
    unsigned char b12;
    unsigned char b13;
    unsigned char b14;
    unsigned char b15;
    unsigned char b16;
    unsigned char b17;
};

struct Obj {
    char pad0[0x4ac0];
    struct E arr[24];
    char pad1[0x4c8];
    int a;
    int b;
};

extern void func_ov004_020afdd0(void* a0, int a1, int a2, int a3, int a4);
extern void func_ov004_020b2444(int a1, int a2, int num, int a4, int a5, int sel, int idx);
extern void* data_ov006_02133f10[];

void func_ov006_020dcea8(struct Obj* o) {
    int i;
    int v1, v2;
    if (o->a == 5 && o->b == 0) return;
    v1 = 0;
    v2 = 0;
    for (i = 0; i < 24; i++) {
        int t = o->arr[i].b10;
        int idx = (t == 2) ? 1 : v1;
        int x = o->arr[i].x >> 12;
        int y = o->arr[i].y >> 12;
        int val = (o->a == 5) ? 1 : v2;
        if (t == 2) val = 1;
        if (o->a != 5) {
            if (o->arr[i].b12 != 0) {
                func_ov004_020afdd0(data_ov006_02133f10[idx], x, y, -1, val);
            }
        } else {
            if (idx != 0 && o->arr[i].b13 != 0) {
                func_ov004_020afdd0(data_ov006_02133f10[idx], x, y, -1, val);
            }
            if (o->arr[i].b15 == 0 && o->arr[i].b13 != 0) {
                func_ov004_020b2444(x, y, o->arr[i].b13, 0, -1, 0, 0);
            }
        }
    }
}
