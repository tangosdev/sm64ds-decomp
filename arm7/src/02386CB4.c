/* storage 0x02386CB4 runtime 0x037FEB4C size 0x50 wram. Pool +0x50: 03808744. */

extern int fn_02382AC4(void);
extern int fn_02382AD8(int);
extern void fn_02381600(int, unsigned int, int);
extern void fn_02383B54(void);

int fn_02386CB4(int unused, unsigned int value) {
    int tok;

    tok = fn_02382AC4();
    if (value >= 0x2000000)
        fn_02381600(0x03808744, value, 0);
    else if (value == 0)
        fn_02383B54();
    return fn_02382AD8(tok);
}
