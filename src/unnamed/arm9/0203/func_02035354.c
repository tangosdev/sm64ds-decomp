int func_0203543c(unsigned char *p) {
    return p[4] & 8;
}

void func_02035428(char *self) {
    *(unsigned char *)(self + 4) |= 0x20;
}

void func_02035414(char *self) {
    *(unsigned char *)(self + 4) &= ~0x20;
}

int func_02035408(unsigned char *p) {
    return p[4] & 32;
}

void func_020353f4(char *self) {
    *(unsigned char *)(self + 4) |= 0x40;
}

void func_020353e0(char *self) {
    *(unsigned char *)(self + 4) &= ~0x40;
}

int func_020353d4(unsigned char *p) {
    return p[4] & 64;
}

void func_020353b0(char *c, int *p) {
    if (p == 0) {
        *(int *)(c + 0xc) = 0;
        *(int *)(c + 8) = -1;
    } else {
        *(int **)(c + 0xc) = p;
        *(int *)(c + 8) = p[1];
    }
}

void func_02035394(unsigned char *dst, unsigned char *src) {
    *(unsigned char *)(dst + 4) = *(unsigned char *)(src + 4);
    *(int *)(dst + 8) = *(int *)(src + 8);
    *(int *)(dst + 0xc) = *(int *)(src + 0xc);
}

typedef struct A {
    int f0;
    int f4;
    int f8;
    struct A* fc;
} A;

int func_02035354(A* a, A* b){
    if (b == 0 || a->fc == 0) return 0;
    {
        int t = b->f4;
        if (a->fc == b && a->f8 == t) return 1;
    }
    return 0;
}
