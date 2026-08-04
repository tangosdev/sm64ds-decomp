void func_ov006_020c1604(char* c, int unused, short a2, int a3);

void func_ov006_020f6bf0(char* c){
    *(short*)(c+0x53e6) = 0;
    *(short*)(c+0x53e8) = 0;
    if (*(unsigned char*)(c+0x540a) == 1)
        func_ov006_020c1604(c+0x4f38, 4, 3, (int)(c+0x53e6));
    else
        func_ov006_020c1604(c+0x4f38, 4, 4, (int)(c+0x53e6));
    *(short*)(c+0x511e) = 1;
    *(int*)(c+0x53d8) = 1;
    if (*(unsigned char*)(c+0xc4) == 0) {
        *(unsigned char*)(c+0xc3) = 1;
        *(unsigned char*)(c+0xc4) = 1;
        *(short*)(c+0xc0) = 0;
    }
}
