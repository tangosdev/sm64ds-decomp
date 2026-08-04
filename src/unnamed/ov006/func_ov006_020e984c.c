void func_ov006_020e95a4(char *c);
void func_ov006_020e984c(char *c) {
    func_ov006_020e95a4(c);
    *(short*)(c + 0x5548) = 0;
    *(short*)(c + 0x554a) = 0;
    *(unsigned char*)(c + 0x554e) = 0;
    *(unsigned char*)(c + 0x554f) = 0;
    *(unsigned char*)(c + 0x5550) = 0;
    *(unsigned char*)(c + 0x5552) = 0xff;
    *(unsigned char*)(c + 0x5553) = 0;
    *(short*)(c + 0x554c) = 0;
    *(unsigned char*)(c + 0x5554) = 0;
    *(unsigned char*)(c + 0x5555) = 1;
}
