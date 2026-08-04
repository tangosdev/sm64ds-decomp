extern int func_ov006_020deac8(char *p, int a, int b, int c);
extern void func_ov006_020ded84(char *p, int a, int b, int c, int e);
int func_ov006_020deb48(char *c, int a, int b, int d, signed char e) {
    int r = func_ov006_020deac8(c, a, b, d);
    if (r >= 0 && r < 0x20) {
        func_ov006_020ded84(c + r * 0x18, a, b, d, e);
        return r;
    }
    return -1;
}
