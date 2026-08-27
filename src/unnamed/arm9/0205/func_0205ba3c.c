struct Bits96 {
    unsigned char pad[0x388];
    unsigned bits[1];
};

int func_0205ba3c(int bit, int word) {
    struct Bits96 *p = (struct Bits96 *)0x27ffc00;
    return (((1 << bit) & p->bits[word]) != 0);
}
