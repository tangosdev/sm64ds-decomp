typedef unsigned short u16;
typedef unsigned int u32;

extern void _ZN4CP1527FlushAndInvalidateDataCacheEjj(u32 addr, u32 size);
extern void func_02056554(const void* src, int offset, int count);
extern void func_02056494(const void* src, int offset, int count);
extern void func_020563d4(const void* src, u32 offset, u32 count);
extern void func_02056314(void* dst, u32 offset, u32 len);
extern void func_020564f4(const void* src, int offset, int count);
extern void func_02056434(const void* src, int offset, int count);
extern void func_02056374(const void* src, u32 offset, u32 count);
extern void func_020562b4(const void* src, u32 offset, u32 count);

struct S {
    int type;    /* 0x0 */
    int pad4;
    int pad8;
    int flag_c;  /* 0xc */
    int buf;     /* 0x10 */
    int pad14;
    int arg18;   /* 0x18 */
    u16 w;       /* 0x1c */
    u16 h;       /* 0x1e */
    int x0;      /* 0x20 */
    int y0;      /* 0x24 */
    int x1;      /* 0x28 */
    int y1;      /* 0x2c */
};

void func_ov007_020c076c(struct S *s, int flags)
{
    int x = (s->x1 + s->x0) >> 12;
    int y = (s->y1 + s->y0) >> 12;
    int size;

    if (s->buf == 0)
        return;

    if (s->flag_c != 0) {
        size = s->w * s->h * 2;
        _ZN4CP1527FlushAndInvalidateDataCacheEjj((u32)s->buf, size);
        if (flags & 1) {
            switch (s->type) {
            case 0: func_02056554((const void*)s->buf, s->arg18, size); break;
            case 1: func_02056494((const void*)s->buf, s->arg18, size); break;
            case 2: func_020563d4((const void*)s->buf, s->arg18, size); break;
            case 3: func_02056314((void*)s->buf, s->arg18, size); break;
            }
        }
        if (flags & 2) {
            switch (s->type) {
            case 0: func_020564f4((const void*)s->buf, s->arg18, size); break;
            case 1: func_02056434((const void*)s->buf, s->arg18, size); break;
            case 2: func_02056374((const void*)s->buf, s->arg18, size); break;
            case 3: func_020562b4((const void*)s->buf, s->arg18, size); break;
            }
        }
        s->flag_c = 0;
    }

    if (flags & 1) {
        switch (s->type) {
        case 0: *(volatile u32*)0x4000010 = (x & 0x1ff) | ((y << 16) & 0x1ff0000); break;
        case 1: *(volatile u32*)0x4000014 = (x & 0x1ff) | ((y << 16) & 0x1ff0000); break;
        case 2: *(volatile u32*)0x4000018 = (x & 0x1ff) | ((y << 16) & 0x1ff0000); break;
        case 3: *(volatile u32*)0x400001c = (x & 0x1ff) | ((y << 16) & 0x1ff0000); break;
        }
    }

    if (flags & 2) {
        switch (s->type) {
        case 0: *(volatile u32*)0x4001010 = (x & 0x1ff) | ((y << 16) & 0x1ff0000); break;
        case 1: *(volatile u32*)0x4001014 = (x & 0x1ff) | ((y << 16) & 0x1ff0000); break;
        case 2: *(volatile u32*)0x4001018 = (x & 0x1ff) | ((y << 16) & 0x1ff0000); break;
        case 3: *(volatile u32*)0x400101c = (x & 0x1ff) | ((y << 16) & 0x1ff0000); break;
        }
    }
}
