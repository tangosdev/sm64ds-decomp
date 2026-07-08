typedef unsigned char u8;
extern void func_0203da2c(int v);
extern void func_0203da4c(void);
extern u8 IS_PLAYER_ACTIVE[];
extern char data_0209fc64[];
extern u8 NUM_PLAYERS_2[];
extern u8 data_0209fc58[];
extern int DP_STATE;
extern int data_0209fc60;
extern u8 data_0209fc54;
extern u8 data_0209f1d8;

void func_02030aa4(int arg)
{
    int i;
    func_0203da2c(arg);
    func_0203da4c();
    for (i = 0; i < 4; i++) {
        IS_PLAYER_ACTIVE[i] = 0;
        data_0209fc64[i] = -1;
    }
    NUM_PLAYERS_2[0] = 0;
    data_0209fc58[0] = 0;
    if (arg == 3)
        DP_STATE = 1;
    else
        DP_STATE = 3;
    data_0209fc60 = 0;
    data_0209fc54 = 0;
    data_0209f1d8 = 1;
}
