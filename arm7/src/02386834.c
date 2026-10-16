extern int fn_02383024(int, int, int);

int fn_02386834(int a1) {
    int result;
    int channel = 7;
    int zero = 0;
    do {
        result = fn_02383024(channel, a1, zero);
    } while (result < 0);
    return result;
}
