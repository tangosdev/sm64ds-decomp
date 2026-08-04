extern unsigned short data_ov006_0213d344[];
void func_ov006_020f5de0(char *c, int i){
    int o = i * 0x18;
    unsigned short cnt;
    *(unsigned short*)(c + 0x51b6 + o) += 1;
    cnt = *(unsigned short*)(c + 0x51b6 + o);
    if(cnt < (data_ov006_0213d344[*(unsigned char*)(c + 0x5000 + o + 0x1bd)] & 0xff))
        return;
    *(unsigned short*)(c + 0x51b6 + o) = 0;
    *(unsigned char*)(c + 0x51bd + o) -= 1;
    if(*(unsigned char*)(c + 0x51bd + o) == 0)
        *(unsigned char*)(c + 0x5000 + o + 0x1bc) = 2;
}
