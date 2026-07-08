extern int func_ov004_020b6324(int);

int func_ov006_02125248(void *this) {
    int x = *(int *)((char *)this + 0xb4);
    int v = x / 5;
    if ((unsigned int)v > 3) {
        v = 3;
    }
    return func_ov004_020b6324(v + 1);
}
