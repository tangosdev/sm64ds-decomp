typedef unsigned short u16;
typedef unsigned char u8;
typedef unsigned int u32;

struct V3 { int x, y, z; };
struct H2 { u16 a, b; };

struct Src {
    char pad0[0xc];
    u16 h_c;    /* 0xc */
    u16 h_e;    /* 0xe */
    u16 h_10;   /* 0x10 */
    char pad12[2];
    int w_8_at_8;  /* placeholder */
};

struct Obj {
    int m0;      /* 0 */
    int m4;      /* 4 */
    int m8;      /* 8 */
    int mc;      /* 0xc */
    int m10;     /* 0x10 */
    int m14;     /* 0x14 */
    void *m18;   /* 0x18 */
    int m1c;     /* 0x1c */
    struct V3 m20;  /* 0x20 */
    int m2c;     /* 0x2c */
    int m30;     /* 0x30 */
    int m34;     /* 0x34 */
    u16 m38;     /* 0x38 */
    u16 m3a;     /* 0x3a */
    u16 m3c;     /* 0x3c */
    u16 m3e;     /* 0x3e */
    u16 m40;     /* 0x40 */
    char pad42[2];
    int m44;     /* 0x44 */
    int m48;     /* 0x48 */
    int m4c;     /* 0x4c */
    int m50;     /* 0x50 */
    u16 m54;     /* 0x54 */
    char pad56[2];
    u8 m58;      /* 0x58 */
    u8 m59;      /* 0x59 */
    char pad5a[0xe];
    int m68;     /* 0x68 */
    int m6c;     /* 0x6c */
    int m70;     /* 0x70 */
};

void func_0204ae2c(struct Obj *o, void *p18, struct V3 *src3)
{
    o->m18 = p18;
    o->m1c = 0;
    o->m20 = *src3;
    o->m34 = 0;
    o->m30 = o->m34;
    o->m2c = o->m30;
    o->m38 = 0;
    o->m3a = 0;
    {
        char *s = *(char **)(o->m18);
        *(struct H2 *)((char *)o + 0x3c) = *(struct H2 *)(s + 0xc);
        o->m40 = *(u16 *)(s + 0x10);
        o->m44 = *(int *)((char *)(*(void **)(o->m18)) + 8);
        o->m48 = *(int *)((char *)(*(void **)(o->m18)) + 0x14);
        o->m4c = *(int *)((char *)(*(void **)(o->m18)) + 0x18);
        o->m50 = *(int *)((char *)(*(void **)(o->m18)) + 0x1c);
        o->m54 = *(u16 *)((char *)(*(void **)(o->m18)) + 0x2a);
        o->m58 = *(u8 *)((char *)(*(void **)(o->m18)) + 0x30);
        o->m59 = *(u8 *)((char *)(*(void **)(o->m18)) + 0x31);
    }
    o->m4 = 0;
    o->m0 = o->m4;
    o->m10 = 0;
    o->m8 = o->m10;
    o->m14 = 0;
    o->mc = o->m14;
    o->m68 = 0;
    o->m6c = 0;
    o->m70 = 0;
}
