extern unsigned char NUM_PLAYERS_2[];
extern signed char data_0209fc64[];
extern int data_ov075_0211b554[][4];
int func_ov075_02115290(int dummy, int col) {
    int cnt = NUM_PLAYERS_2[0];
    int row;
    if ((unsigned int)cnt <= 1) {
        row = 0;
    } else {
        row = cnt - 1;
        col = data_0209fc64[col];
        if (col < 0) col = 3;
    }
    return data_ov075_0211b554[row][col];
}
