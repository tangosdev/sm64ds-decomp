extern unsigned short DecIfAbove0_Short(unsigned short* p) {
    if (*p != 0) *p = *p - 1;
    return *p;
}
