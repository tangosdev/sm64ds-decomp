typedef unsigned char u8;

extern int func_0203d9b4(void);
extern int func_0203d950(int i);
extern void func_02020214(unsigned int flags);
extern u8 func_0202027c(int bit);
extern int func_0203da9c(void);
extern void func_02030790(void);

extern u8 IS_PLAYER_ACTIVE[];
extern u8 data_0209fc58;

void func_02030b58(void) {
    if (func_0203d9b4() != 0) {
        u8 flags = 0;
        int j;
        for (j = 0; j < 4; j++) {
            if (func_0203d950(j) != 0)
                flags = (u8)(flags | (1 << j));
        }
        func_02020214(flags);
    }
    int i;
    for (i = 0; i < 4; i++) {
        if (IS_PLAYER_ACTIVE[i] != 0) {
            if (func_0202027c(i) == 0) {
                data_0209fc58 = (u8)(data_0209fc58 | (1 << i));
                IS_PLAYER_ACTIVE[i] = 0;
            }
        }
    }
    if (IS_PLAYER_ACTIVE[0] == 0) return;
    if (IS_PLAYER_ACTIVE[func_0203da9c()] != 0) return;
    data_0209fc58 |= 0xf;
    func_02030790();
}
