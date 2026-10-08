extern unsigned int data_020a6488;

int func_0205af78(unsigned int x) {
    unsigned int g = data_020a6488;
    if (x > g) {
        return (x - g) >= 0x80000000u;
    } else {
        return (g - x) < 0x80000000u;
    }
}
