struct S {
    int x;       /* +0x0 */
    int y;       /* +0x4 */
    int z;       /* +0x8 */
    short w;     /* +0xc */
    unsigned char inuse;  /* +0xe */
    unsigned char f;      /* +0xf */
    unsigned char g;      /* +0x10 */
    unsigned char pad;    /* +0x11 */
    unsigned char pad2[2];/* +0x12 -> stride 0x14 */
};

extern struct S data_ov006_02142044[];

void func_ov006_020ea81c(int r0, int r1) {
    int i;
    for (i = 0; i < 0x10; i++) {
        if (data_ov006_02142044[i].inuse == 0) {
            data_ov006_02142044[i].inuse = 1;
            data_ov006_02142044[i].f = 1;
            data_ov006_02142044[i].w = 0x18;
            if (r0 < 0x14000) r0 = 0x14000;
            else if (r0 > 0xf0000) r0 = 0xf0000;
            data_ov006_02142044[i].x = r0;
            data_ov006_02142044[i].y = r1;
            data_ov006_02142044[i].z = 0x800;
            data_ov006_02142044[i].g = 0;
            return;
        }
    }
}
