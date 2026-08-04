extern void func_ov006_020efdf0(char* c, int idx);
extern unsigned char data_0209d45c;

void func_ov006_020effb8(char* c, int idx) {
    *(int*)(c + 0x47ec + idx * 0x14) += 0x2000;
    func_ov006_020efdf0(c, idx);
    if ((*(int*)(c + 0x47ec + idx * 0x14) >> 12) < 0xa0) return;
    *(unsigned char*)(c + 0x47f5 + idx * 0x14) += 1;
    data_0209d45c &= ~4;
}
