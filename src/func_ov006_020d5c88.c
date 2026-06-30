extern void func_ov004_020af68c(void *a0, int a1, int a2, int a3, int a4);
extern void *data_ov006_02133ae0[];
extern void *data_ov006_02133a70[];

struct S {
    int x;                  /* 0x00 */
    int y;                  /* 0x04 */
    unsigned char pad[5];   /* 0x08 */
    unsigned char active;   /* 0x0d */
    unsigned char idx;      /* 0x0e */
    unsigned char pad2;     /* 0x0f */
};

struct Big {
    unsigned char pad[0x62b0];
    struct S subs[2];
};

void func_ov006_020d5c88(struct Big *b) {
    int i;
    for (i = 0; i < 2; i++) {
        if (b->subs[i].active != 0) {
            int xv = b->subs[i].x;
            int yv = b->subs[i].y;
            int idx = b->subs[i].idx;
            int a1 = xv >> 12;
            int a2 = yv >> 12;
            func_ov004_020af68c((i != 0) ? data_ov006_02133a70[idx]
                                         : data_ov006_02133ae0[idx],
                                a1, a2, -1, 1);
        }
    }
}
