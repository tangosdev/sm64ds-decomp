void func_ov006_02100734(char* c, int idx){
    char* e = c + idx * 0x40;
    if (*(unsigned char*)(e + 0x5294) == 0) return;
    *(unsigned short*)(e + 0x5290) = 0;
    *(int*)(e + 0x5270) = 0;
    *(unsigned short*)(e + 0x5292) = 0x40;
    *(unsigned char*)(e + 0x5296) = 0xc;
}
