extern void func_ov006_020c9098(char* o);
extern void func_ov006_020c8ecc(char* o);
extern int data_ov006_021405bc;
extern int data_ov006_021405b4;
extern int data_ov006_021405b0;
extern int data_ov006_021405ac;
extern char* data_ov006_02140554;

void func_ov006_020ca840(void) {
    int i, n, off;
    n = data_ov006_021405bc;
    data_ov006_021405b0 = 0x100;
    data_ov006_021405b4 = -0x4800;
    data_ov006_021405ac = 1;

    i = 0;
    if (n > 0) {
        off = 0;
        do {
            func_ov006_020c9098(data_ov006_02140554 + off);
            func_ov006_020c8ecc(data_ov006_02140554 + off);
            n = data_ov006_021405bc;
            i++;
            off += 0xdc;
        } while (i < n);
    }
}
