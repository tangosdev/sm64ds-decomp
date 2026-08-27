/* func_0203faa8 at 0x0203faa8 (arm9), size 0xb4
 * Compiler mwccarm 1.2/sp2p3, flags:
 * -O4,p -enum int -lang c99 -char signed -interworking -proc arm946e -gccext,on -msgstyle gcc
 * goto-pinned early-return: the ne-path must be textually first; plain ==/!= return
 * spellings canonicalize to moveq-first (see notes/mwccarm-codegen.md) */
typedef struct {
    char pad[0x14];
    unsigned short field_14;
} StructA;

extern StructA *data_020a0f44;
extern unsigned int data_020a0f58;
extern unsigned int data_020a0f64;
extern unsigned int data_020a0f68;
extern unsigned int data_020a0f60;

extern void func_0203fdac(void);

extern int func_020625fc(void *fn, unsigned int a, unsigned short b, unsigned int c,
                         unsigned short s0, unsigned short s1,
                         int z0, int z1, int z2, int z3, int one);

int func_0203faa8(void)
{
    unsigned short flag = (unsigned short)(data_020a0f44->field_14 == 0);
    unsigned short s0 = (unsigned short)data_020a0f58;

    int r = func_020625fc(
        (void *)func_0203fdac,
        data_020a0f68,
        (unsigned short)data_020a0f64,
        data_020a0f60,
        s0, flag,
        0, 0, 0, 0, 1);

    if (r == 2)
        goto yes;
    return 0;
yes:
    return 1;
}
