extern unsigned char data_0209fc5c[];
extern int func_020201f0(int);
int func_ov075_02116ce4(void) {
    int i;
    for (i = 0; i < 4; i++) {
        if (data_0209fc5c[i]) {
            int r = func_020201f0(i);
            if (!r) return 0;
        }
    }
    return 1;
}
