struct Inner83 {
    char pad[0x34];
    short h34;
    short pad36;
    short h38;
    short h3a;
};

void func_0204f6f8(struct Inner83 **o, short a, short b) {
    if (*o == 0) return;
    (*o)->h34 = 2;
    (*o)->h38 = a;
    (*o)->h3a = b;
}
