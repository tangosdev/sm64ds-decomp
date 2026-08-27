struct Camera {
    char pad60[0x60];
    int f60;              // 0x60
    char pad_to6d8[0x6d8 - 0x64];
    unsigned char f6d8;   // 0x6d8
    char pad_to6e5[0x6e5 - 0x6d9];
    unsigned char f6e5;   // 0x6e5
};

extern struct Camera *data_0209f318;
extern int data_0209f32c;

extern void func_0200d768(struct Camera *thiz, unsigned char playerID);
extern void func_0200d72c(struct Camera *thiz, unsigned char playerID);

/* The 0x6e5 read-modify-write goes through a materialized base
   (ldr rN,[pc]; add rN,c,rN) because 0x6e5 is not an ARM rotated immediate;
   the (long long)-mask launder is what stops mwcc folding it to [c,#0x6e5]. */
#define LAUNDER_U8(p) \
    ((unsigned char *)(int)(p))

void func_ov002_020ceb7c(struct Camera *c)
{
    struct Camera *g = data_0209f318;

    if (c->f6e5 & 2) {
        if (data_0209f32c - 0x50000 - c->f60 >= 0x64000)
            return;
        func_0200d768(g, c->f6d8);
        *LAUNDER_U8((char *)c + 0x6e5) ^= 2;
    } else {
        if (data_0209f32c - 0x50000 - c->f60 <= 0x12c000)
            return;
        func_0200d72c(g, c->f6d8);
        *LAUNDER_U8((char *)c + 0x6e5) |= 2;
    }
}
