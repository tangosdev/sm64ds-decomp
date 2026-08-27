extern void func_ov004_020afdd0(void* a0, int a1, int a2, int a3, int a4);

extern void* data_ov006_02136b80[];

struct E {
    int x;                  /* 0x00 */
    int y;                  /* 0x04 */
    char pad0[0x0f];        /* 0x08 */
    unsigned char idx;      /* 0x17 */
    char pad1;              /* 0x18 */
    unsigned char flag;     /* 0x19 */
    char pad2[6];           /* 0x1a */
};

struct Obj {
    char pad[0x5620];
    struct E arr[2];
};

void func_ov006_020ff47c(struct Obj* o) {
    int i;
    for (i = 0; i < 2; i++) {
        if (o->arr[i].flag) {
            func_ov004_020afdd0(data_ov006_02136b80[o->arr[i].idx],
                                o->arr[i].x >> 12, o->arr[i].y >> 12, -1, 2);
        }
    }
}
