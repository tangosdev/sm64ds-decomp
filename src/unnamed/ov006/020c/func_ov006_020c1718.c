extern int data_ov006_0213ac58[];
int func_ov006_020c1718(int* r0) {
    int* g = data_ov006_0213ac58;
    int w0 = r0[0];
    int ip = 1;
    if (w0 == g[0]) {
        if (r0[1] == g[1] || w0 == 0) ip = 0;
    }
    return ip == 0;
}
