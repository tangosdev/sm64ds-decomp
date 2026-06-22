void func_02059fa8(int ch) {
    while (*(volatile unsigned *)(0x40000b0 + (ch * 3 + 2) * 4) & 0x80000000);
}
