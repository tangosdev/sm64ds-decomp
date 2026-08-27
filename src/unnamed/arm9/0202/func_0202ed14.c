struct Obj {
    char pad_e[0xe];
    unsigned char b_e;      /* 0xe */
    unsigned char b_f;      /* 0xf */
    int w_10;               /* 0x10 */
    int w_14;               /* 0x14 */
    int w_18;               /* 0x18 */
    int w_1c;               /* 0x1c */
    int w_20;               /* 0x20 */
    int w_24;               /* 0x24 */
    unsigned char b_28;     /* 0x28 */
};

void func_0202ed14(struct Obj *o)
{
    o->b_f = 0;
    o->b_e = 1;
    o->w_10 = 0;
    o->w_1c = 0;
    o->w_20 = 0;
    o->w_24 = 0;
    o->w_14 = 0;
    o->w_18 = o->w_14;
    o->b_28 = 0;
}
