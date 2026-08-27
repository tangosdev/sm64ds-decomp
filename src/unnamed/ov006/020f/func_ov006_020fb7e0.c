typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;

extern void func_ov006_020fbb2c(char *c, int idx, unsigned short val);

/* Distinct 64-bit masks force independent RMW base materialization (add+pool).
 * Plain *(b+0x5960) for the compare/zero reloads folds to add #0x5900 + #0x60. */
#define M1(a) (a)
#define M2(a) (a)

void func_ov006_020fb7e0(char *thiz)
{
    int i;
    char *b = thiz;
    for (i = 0; i < 0x1e; i++) {
        if (*(u8 *)(b + 0x5964) != 0) {
            u16 *cnt = (u16 *)(int)M1((u32)b + 0x5960);
            int lim;
            u8 sub;
            (*cnt)++;
            if (*(u16 *)(b + 0x5960) >= 6) {
                *(u16 *)(b + 0x5960) = 0;
                (*(u8 *)(int)M2((u32)b + 0x5965))++;
                lim = 3;
                if (*(u8 *)(b + 0x5967) == 2) lim = 5;
                sub = *(u8 *)(b + 0x5968);
                if (sub != 0) {
                    if (lim - 2 == *(u8 *)(b + 0x5965)) {
                        char *q = thiz + (sub - 1) * 0xc + 0x5000;
                        *(u8 *)(q + 0xbd4) = 0;
                        *(u8 *)(q + 0xbd6) = 1;
                    }
                }
                if (*(u8 *)(b + 0x5965) >= lim) {
                    *(u8 *)(b + 0x5966) = 0;
                    *(u8 *)(b + 0x5964) = 0;
                    if (*(u16 *)(b + 0x5962) != 0) {
                        func_ov006_020fbb2c(thiz, i, *(u16 *)(b + 0x5962));
                    }
                }
            }
        }
        b = b + 0x14;
    }
}
