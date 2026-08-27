extern unsigned char data_0209f25c;
extern short *data_0209f348;

int GetMinimapScale(int idx) {
    if (idx >= 0) {
        if (idx < (int)data_0209f25c) return data_0209f348[idx];
    }
    return 0xa00;
}
