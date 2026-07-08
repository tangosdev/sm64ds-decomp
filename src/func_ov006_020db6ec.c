extern int func_ov004_020b6324(int);

int func_ov006_020db6ec(void *this) {
    int x = *(int *)((char *)this + 0xb4);
    int v;
    if (x < 5) {
        v = 1;
    } else if (x < 10) {
        v = 2;
    } else if (x < 15) {
        v = 3;
    } else {
        v = 4;
    }
    return func_ov004_020b6324(v);
}
