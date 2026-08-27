extern unsigned char data_0209fc50[];
extern signed char data_0209fc64[];
extern int data_ov075_0211b554[][4];
int func_ov075_02115290(int dummy, int col) {
    int cnt = data_0209fc50[0];
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
