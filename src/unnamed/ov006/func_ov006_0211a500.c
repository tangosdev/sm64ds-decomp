struct Sub {
    int a;            /* +0x00 (this+0x51b4) */
    int pad4;         /* +0x04 */
    int b;            /* +0x08 (this+0x51bc) */
    int c;            /* +0x0c (this+0x51c0) */
    char pad10[8];    /* +0x10 */
    unsigned char f18;/* +0x18 (this+0x51cc) */
    unsigned char f19;/* +0x19 (this+0x51cd) */
    char pad1a[10];   /* pad to 0x24 */
};

typedef struct Obj {
    char pad[0x51b4];
    struct Sub arr[4];
} Obj;

void func_ov006_0211a500(Obj *this, int idx) {
    this->arr[idx].a += this->arr[idx].b;
    this->arr[idx].b -= 0x60;
    if ((this->arr[idx].c - this->arr[idx].a) >> 0xc >= 0x3c) {
        this->arr[idx].f18 = 0;
        this->arr[idx].f19 = 0;
    }
}
