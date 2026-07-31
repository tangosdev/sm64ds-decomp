extern int data_ov006_0213b0f4[];
int func_ov006_020caf4c(int* r0) {
    int* p = (int*)(((int)r0 + 0x64));
    int* g = data_ov006_0213b0f4;
    int lr = 1;
    if (p[0] == g[0]) {
        if (p[1] == g[1] || r0[0x19] == 0) lr = 0;
    }
    return lr;
}
