extern void func_ov004_020af948(void* a, int b, int c, void* m);
extern void* data_ov006_02133a3c[];

typedef struct {
    int x;                  /* 0x00 */
    int y;                  /* 0x04 */
    char _0[0x13];          /* 0x08..0x1a */
    unsigned char type;     /* 0x1b */
    unsigned char enable;   /* 0x1c */
    char _1[3];             /* -> size 0x20 */
} Slot;

typedef struct {
    char _0[0x52bc];
    Slot slots[20];
} Outer;

void func_ov006_020e8354(Outer* a) {
    int i;
    for (i = 0; i < 20; i++) {
        if (a->slots[i].enable) {
            func_ov004_020af948(data_ov006_02133a3c[a->slots[i].type],
                                a->slots[i].x >> 12, a->slots[i].y >> 12, 0);
        }
    }
}
