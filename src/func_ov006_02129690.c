typedef signed char s8;
typedef unsigned char u8;
typedef int s32;

extern void func_ov004_020af948(void* a, int b, int c, void* m);
extern void func_ov004_020afcf8(void* a, int b, int c, void* m);
extern void* data_ov006_02139c6c[];

typedef struct {
    s32 a;        /* 0x00 */
    s32 b;        /* 0x04 */
    u8 pad[0x16]; /* 0x08 */
    u8 f1e;       /* 0x1e */
    u8 f1f;       /* 0x1f */
    u8 f20;       /* 0x20 */
    u8 pad2[3];   /* 0x21 */
} Elem;

typedef struct {
    u8 head[0xab6c];
    s32 fixed;                  /* 0xab6c */
    u8 mid[0xbe94 - 0xab70];
    Elem arr[50];               /* 0xbe94 */
} Obj;

void func_ov006_02129690(Obj* a)
{
    int i;
    for (i = 0; i < 50; i++) {
        if (a->arr[i].f1e != 0) {
            int bb = a->arr[i].a >> 12;
            int cc = (a->arr[i].b - a->fixed) >> 12;
            func_ov004_020af948(data_ov006_02139c6c[a->arr[i].f1f], bb, cc, 0);
            func_ov004_020afcf8(data_ov006_02139c6c[a->arr[i].f20], bb, cc, 0);
        }
    }
}
