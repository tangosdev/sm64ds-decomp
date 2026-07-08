extern int DP_STATE;
extern unsigned char IS_PLAYER_ACTIVE[];
extern signed char data_0209fc64[];
extern signed char NUM_PLAYERS_2;
extern signed char data_0209fc58;

void func_02030994(void)
{
    signed char acc;
    int i;
    unsigned char* src;
    signed char* dst;
    if (DP_STATE == 0) return;
    acc = 0;
    src = IS_PLAYER_ACTIVE;
    dst = data_0209fc64;
    i = 0;
    do {
        unsigned char b = *src;
        i++;
        src++;
        if (b != 0) {
            *dst = acc;
            acc = (signed char)(acc + 1);
        } else {
            *dst = -1;
        }
        dst++;
    } while (i < 4);
    NUM_PLAYERS_2 = acc;
    data_0209fc58 = 0;
    DP_STATE = 4;
}
