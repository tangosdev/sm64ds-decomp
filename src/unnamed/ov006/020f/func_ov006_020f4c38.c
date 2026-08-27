void func_ov006_020c1604(char* c, int unused, short a2, int a3);

void func_ov006_020f4c38(char* c){
    *(short*)(c+0x5326) = 0;
    *(short*)(c+0x5328) = 0;
    if (*(unsigned char*)(c+0x533c) >= 2)
        func_ov006_020c1604(c+0x4f38, 4, 3, (int)(c+0x5326));
    else
        func_ov006_020c1604(c+0x4f38, 4, 2, (int)(c+0x5326));
    *(short*)(c+0x511e) = 1;
    *(int*)(c+0x5318) = 1;
    if (*(unsigned char*)(c+0xc4) == 0) {
        *(unsigned char*)(c+0xc3) = 1;
        *(unsigned char*)(c+0xc4) = 1;
        *(short*)(c+0xc0) = 0;
    }
}
