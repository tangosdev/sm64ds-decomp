extern void func_ov074_02120808(char* c);

void func_ov074_0211ffac(char* c) {
    func_ov074_02120808(c);
    *(unsigned short *)(c + 0x500 + 0xfc) = 0;
}
