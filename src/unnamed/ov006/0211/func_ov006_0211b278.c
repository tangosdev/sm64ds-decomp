void func_ov006_0211b278(char *c, int i){
    int o = i * 0x24;
    *(int*)(c + 0x51b0 + o) += *(int*)(c + 0x51b8 + o);
    *(int*)(c + 0x51b4 + o) += *(int*)(c + 0x51bc + o);
    *(int*)(c + 0x51bc + o) += 0x200;
    if(*(int*)(c + 0x51bc + o) > 0){
        *(int*)(c + 0x51bc + o) = 0;
        *(unsigned char*)(c + 0x51d1 + o) = 2;
    }
}
