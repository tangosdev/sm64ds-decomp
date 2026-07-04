extern int data_ov006_0213b17c[];
int func_ov006_020ccd04(int* r0) {
    int* p = (int *)(((int)r0 + 0x64) & 0xFFFFFFFFFFFFFFFF);
    int* g = data_ov006_0213b17c;
    int ret = 1;
    if (p[0] == g[0]) {
        if (p[1] == g[1]) goto zero;
        if (r0[0x19] != 0) goto out;
    zero:
        ret = 0;
    }
out:
    return ret == 0;
}
