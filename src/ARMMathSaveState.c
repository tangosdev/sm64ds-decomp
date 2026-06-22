typedef unsigned int u32;
typedef unsigned short u16;

struct Pair { u32 a, b; };

struct ARMMathState {
    struct Pair p0;   /* 0x00 */
    struct Pair p8;   /* 0x08 */
    struct Pair p10;  /* 0x10 */
    u16 h18;          /* 0x18 */
    u16 h1a;          /* 0x1a */
};

void ARMMathSaveState(struct ARMMathState *s)
{
    s->p0 = *(struct Pair*)0x4000290;
    s->p8 = *(struct Pair*)0x4000298;
    s->h18 = (u16)(*(volatile u16*)0x4000280 & 3);
    s->p10 = *(struct Pair*)0x40002b8;
    s->h1a = (u16)(*(volatile u16*)0x40002b0 & 1);
}
