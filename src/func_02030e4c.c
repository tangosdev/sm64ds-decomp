extern unsigned short func_0203da80(unsigned short x);
extern int func_0203d950(int i);
extern void func_02030790(void);
extern int DP_STATE;
extern unsigned char data_0209fc58;
extern unsigned char data_0209fc54;
extern void (*data_020927ec[])(void);

int func_02030e4c(void)
{
    void (*fn)(void);
    if (DP_STATE != 0 && DP_STATE < 5) {
        if (!func_0203da80(0x9d))
            goto second;
        if (func_0203da80(4)) {
            data_0209fc58 |= 0x10;
        } else if (func_0203da80(0x80)) {
            data_0209fc58 |= 0x20;
        } else {
            data_0209fc58 |= 0xf;
            if (DP_STATE < 4)
                data_0209fc58 |= 0x20;
        }
        func_02030790();
    } else {
    second:
        if (DP_STATE >= 3 && DP_STATE < 5) {
            if (func_0203d950(0) != 0 && data_0209fc54 == 0)
                goto end;
            data_0209fc58 |= 1;
            if (DP_STATE < 4)
                data_0209fc58 |= 0x20;
            if (data_0209fc54 != 0)
                data_0209fc58 |= 0x40;
            func_02030790();
        }
    }
end:
    fn = data_020927ec[DP_STATE];
    if (fn != 0)
        fn();
    return 1;
}
