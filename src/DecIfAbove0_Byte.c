extern unsigned char DecIfAbove0_Byte(unsigned char* p) {
    if (*p != 0) *p = *p - 1;
    return *p;
}
