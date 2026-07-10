typedef unsigned char u8;
typedef unsigned short u16;

typedef struct Obj {
    int w0, w4, w8, wC, w10; /* 0x00 */
    int w14;                 /* 0x14 */
    int w18;                 /* 0x18 */
    int pad1C[7];            /* 0x1c */
    int w38;                 /* 0x38 */
    int pad3C[3];            /* 0x3c */
    int w48;                 /* 0x48 */
    int pad4C[5];            /* 0x4c */
    int w60;                 /* 0x60 */
    int w64;                 /* 0x64 */
    u16 h68;                 /* 0x68 */
    u8 b6a;                  /* 0x6a */
    u8 b6b;                  /* 0x6b */
    u8 b6c;                  /* 0x6c */
} Obj;

typedef struct Wrap {
    int w0;
    u8 *data; /* 0x04 */
} Wrap;

extern u8 *func_02071a50(u8 *src, int *dst);

void func_02071864(Obj *t, Wrap *w)
{
    u8 *data = w->data;
    unsigned int flags = data[0];
    int f40 = flags & 0x40;
    int f80;
    u8 *p;
    t->b6b = (f40 != 0);
    t->b6a = ((flags & 0x20) != 0);
    f80 = flags & 0x80;
    t->b6c = (f80 != 0);
    t->h68 = data[1] << 4;
    p = data + 2;
    *(u16 *)(int)(((long long)(int)((int)t + 0x68)) & 0xFFFFFFFFFFFFFFFFLL) |= 0x4000;
    p = func_02071a50(p, &t->w60);
    if (f40) {
        p = func_02071a50(p, &t->w64);
    }
    if (f40) {
        if (f80) {
            t->w18 = t->w38;
        } else {
            t->w18 = t->w48;
        }
        return;
    }
    t->w18 = t->w14;
}
