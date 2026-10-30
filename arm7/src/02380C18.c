/* storage 0x02380C18 runtime 0x037F8AB0 size 0x8C wram. */

extern int fn_02382AF0(void);   /* sub_37FA988 */
extern int fn_02382AC4(void);   /* sub_37FA95C */
extern void fn_02382B04(int);   /* sub_37FA99C */
extern int fn_02382AD8(int);    /* sub_37FA970 */

typedef struct {
    int unk_00;                 /* +0x0 word, cleared */
    unsigned short unk_04;      /* +0x4 halfword tag */
} P3_02380C18;

int fn_02380C18(int a1, P3_02380C18 *a2, void (*a3)(void), int a4) {
    int v8;

    if (a1 != a2->unk_04)
        return -2;
    if (a4 != 0)
        v8 = fn_02382AF0();
    else
        v8 = fn_02382AC4();
    a2->unk_04 = 0;
    if (a3 != 0)
        a3();
    a2->unk_00 = 0;
    if (a4 != 0)
        fn_02382B04(v8);
    else
        fn_02382AD8(v8);
    return 0;
}
