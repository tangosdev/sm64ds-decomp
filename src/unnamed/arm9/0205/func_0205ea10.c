extern unsigned char data_020a80cc[];
void func_0205ea10(int mask) {
    while (*(volatile unsigned short*)(data_020a80cc + 0x36) & mask) ;
}
