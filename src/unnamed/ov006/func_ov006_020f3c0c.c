extern int *func_020beb68;
void func_ov006_020f3c0c(int this) {
    int n;
    int i;
    if (func_020beb68 != 0)
        n = *(int*)((char*)func_020beb68 + 0xa8);
    else
        n = 0;
    if (n >= 3) n = 3;
    *(unsigned char*)(this + 0x5000 + 0x33b) = (unsigned char)n;
    i = 0;
    if (n <= 0) return;
    do {
        *(unsigned char*)(this + i*0x14 + 0x5000 + 0x2d8) = 1;
        *(unsigned char*)(this + i*0x14 + 0x5000 + 0x2da) = 0;
        *(short*)(this + i*0x14 + 0x5200 + 0xd6) = i*0x10;
        i++;
    } while (i < n);
}
