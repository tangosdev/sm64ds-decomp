/* storage 0x0239CA98 runtime 0x027CEA10 size 0x40 main. Two BL relocs, no pool. */

extern void fn_02395CB8(void);
extern int fn_02396914(void);

int fn_0239CA98(int a1, int a2) {
    *(unsigned short *)(a2 + 2) = 1;
    if (*(unsigned short *)(a1 + 16) > 1)
        return 5;
    fn_02395CB8();
    if (*(unsigned short *)(a1 + 16) == 1)
        fn_02396914();
    return 0;
}
