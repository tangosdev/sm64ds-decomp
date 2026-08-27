void func_ov006_020f67a0(char *c){
    int found = 0;
    int i;
    char *p;
    for(i = 0, p = c; i < 0x14; i++, p += 0x18){
        if(*(unsigned char*)(p + 0x51bb) != 0){
            if(*(unsigned char*)(p + 0x51bc) == 2)
                *(unsigned char*)(p + 0x51bc) = 1;
        }
    }
    for(i = 0, p = c; i < 0x14; i++, p += 0x18){
        if(*(unsigned char*)(p + 0x51bb) != 0){
            if(*(unsigned char*)(p + 0x51bc) != 1){ found++; break; }
        }
    }
    if(found == 0)
        *(int*)(c+0x53d8) = 8;
}
