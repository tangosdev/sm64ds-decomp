struct Out { int a; int b; };
void *func_02071a50(void *p, struct Out *out);

void func_0207180c(unsigned char *p) {
    struct Out local;
    unsigned char b = *p;
    p += 2;
    p = func_02071a50(p, &local);
    if (b & 0x40) {
        func_02071a50(p, &local);
    }
}
