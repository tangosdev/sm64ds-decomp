extern unsigned short func_0203da80(unsigned short x);
extern int func_0203d950(int i);
extern void func_02030790(void);
extern int data_0209fc68;
extern unsigned char data_0209fc58;
extern unsigned char data_0209fc54;
extern void (*data_020927ec[])(void);

/* The per-frame session check func_020197b8 runs. In states 1..4 a link
   error (data_020a0f1c bits 0x9d, func_0203da80) ends the session
   (func_02030790). It sets disconnect bit 0x10 for error 4, 0x20 for
   error 0x80, else all four player bits, plus 0x20 before state 4. In
   states 3 and 4, losing the parent (slot 0) or the data_0209fc54 flag
   d_s_entry_methods.cpp raises ends it too: player bit 0, 0x20 before
   state 4, 0x40 for the flag. Then the current state's handler from
   data_020927ec runs. Always 1. */
int func_02030e4c(void)
{
    void (*fn)(void);
    if (data_0209fc68 != 0 && data_0209fc68 < 5 && func_0203da80(0x9d)) {
        if (func_0203da80(4)) {
            data_0209fc58 |= 0x10;
        } else if (func_0203da80(0x80)) {
            data_0209fc58 |= 0x20;
        } else {
            data_0209fc58 |= 0xf;
            if (data_0209fc68 < 4)
                data_0209fc58 |= 0x20;
        }
        func_02030790();
    } else if (data_0209fc68 >= 3 && data_0209fc68 < 5) {
        if (func_0203d950(0) == 0 || data_0209fc54 != 0) {
            data_0209fc58 |= 1;
            if (data_0209fc68 < 4)
                data_0209fc58 |= 0x20;
            if (data_0209fc54 != 0)
                data_0209fc58 |= 0x40;
            func_02030790();
        }
    }
    fn = data_020927ec[data_0209fc68];
    if (fn != 0)
        fn();
    return 1;
}
