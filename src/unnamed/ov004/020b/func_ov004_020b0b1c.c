extern unsigned char data_020a0e40;
extern unsigned char data_020a0de8[][4];
extern unsigned char data_020a0de9[][4];
extern unsigned char data_020a0dea[][4];
extern unsigned char data_020a0deb[][4];

struct Vec2 {
    short x;
    short y;
};

struct Obj004 {
    char pad0[0x10];
    struct Vec2 pos;  /* 0x10 */
    char pad14[0xc];
    int f20;          /* 0x20 */
    char pad24[0xe];
    unsigned short f32;/* 0x32 */
    char pad34[0x100];
};

extern struct Obj004 data_ov004_020bf648[3];
extern int data_ov004_020bc150;
extern int data_ov004_020beb68;

extern void func_ov004_020b422c(struct Obj004 *o);
extern int func_ov004_020b40c0(struct Obj004 *o);

int func_ov004_020b0b1c(int arg) {
    int idx = data_020a0e40;
    int flag = 0;
    int i;

    if (data_020a0de8[idx][0] != 0) {
        if (data_020a0de9[idx][0] != 0) flag = 1;
    }

    if (flag) {
        struct Obj004 *o = data_ov004_020bf648;
        int cx = data_020a0dea[idx][0];
        int cy = data_020a0deb[idx][0];
        for (i = 0; i < 3; i++, o++) {
            if (arg == o->f20 && o->f32 != 0) {
                volatile struct Vec2 pos;
                int dx, dy;
                short ty;
                pos.x = o->pos.x;
                ty = o->pos.y;
                dx = pos.x - cx;
                pos.y = ty;
                dy = pos.y - cy;
                if (dx <= 0x40 && dx >= -0x40 && dy <= 0x13 && dy >= -0x13) {
                    if (data_ov004_020bc150 != 0) {
                        func_ov004_020b422c(o);
                        if ((unsigned int)(arg - 1) <= 1) {
                            *(int *)(*(int *)&data_ov004_020beb68 + 0x4000 + 0x630) = 1;
                        }
                    }
                }
            }
        }
    } else {
        int j;
        struct Obj004 *o = data_ov004_020bf648;
        for (j = 0; j < 3; j++, o++) {
            if (arg == o->f20) {
                if (func_ov004_020b40c0(o) != 0) {
                    if (arg == 1) {
                    } else if (arg == 2) {
                    } else {
                        data_ov004_020bc150 = 1;
                    }
                    return 1;
                }
            }
        }
    }
    return 0;
}
