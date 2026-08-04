int func_ov006_020f9560(void);
extern char* data_ov006_02142578;

void func_ov006_020f9f40(char* c, int r1){
    *(short*)(c+0x2a) = (short)r1;
    if (r1 >= 0x14) {
        *(short*)(c+0x28) = 1;
    } else {
        *(short*)(c+0x28) = (short)(((r1 % 5) << 1) + 1);
    }
    *(int*)(c+0x24) = 0;
    if (r1 == 0x13) data_ov006_02142578 = c;
    *(unsigned char*)(c+0x2e) = 1;
    *(int*)(c+0x1c) = 0;
    *(int*)(c+0x20) = 0;
    *(unsigned char*)(c+0x2d) = 0;
    *(unsigned char*)(c+0x2c) = (unsigned char)func_ov006_020f9560();
    *(int*)(c+8) = 0;
    *(int*)(c+4) = *(int*)(c+8);
}
