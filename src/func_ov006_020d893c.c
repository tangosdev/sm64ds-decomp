/* func_ov006_020d893c — zero-init all 0x70 entries (stride 0x40, array at 0x4660):
 * 11 words, 4 halfwords, 10 bytes per entry. */

typedef unsigned int u32;
typedef unsigned short u16;
typedef unsigned char u8;

typedef struct {
    u32 w0;      /* +0x00 */
    u32 w1;
    u32 w2;
    u32 w3;
    u32 w4;
    u32 w5;
    u32 w6;
    u32 w7;
    u32 w8;
    u32 w9;
    u32 w10;     /* +0x28 */
    u16 h0;      /* +0x2c */
    u16 h1;
    u16 h2;
    u16 h3;      /* +0x32 */
    u8 b0;       /* +0x34 */
    u8 b1;
    u8 b2;
    u8 b3;
    u8 b4;       /* +0x38 */
    u8 b5;       /* +0x39 */
    u8 b6;       /* +0x3a */
    u8 b7;       /* +0x3b */
    u8 b8;       /* +0x3c */
    u8 b9;       /* +0x3d */
    char _pad[2];
} Entry; /* 0x40 */

typedef struct {
    char _pad0[0x4660];
    Entry entries[0x70];
} Work;

void func_ov006_020d893c(char* c)
{
    Work* w = (Work*)c;
    int i;
    for (i = 0; i < 0x70; i++) {
        w->entries[i].w0 = 0;
        w->entries[i].w1 = 0;
        w->entries[i].w2 = 0;
        w->entries[i].w3 = 0;
        w->entries[i].w4 = 0;
        w->entries[i].w5 = 0;
        w->entries[i].w6 = 0;
        w->entries[i].w7 = 0;
        w->entries[i].w8 = 0;
        w->entries[i].w9 = 0;
        w->entries[i].w10 = 0;
        w->entries[i].h0 = 0;
        w->entries[i].h1 = 0;
        w->entries[i].h2 = 0;
        w->entries[i].h3 = 0;
        w->entries[i].b0 = 0;
        w->entries[i].b1 = 0;
        w->entries[i].b2 = 0;
        w->entries[i].b3 = 0;
        w->entries[i].b4 = 0;
        w->entries[i].b7 = 0;
        w->entries[i].b5 = 0;
        w->entries[i].b6 = 0;
        w->entries[i].b8 = 0;
        w->entries[i].b9 = 0;
    }
}
