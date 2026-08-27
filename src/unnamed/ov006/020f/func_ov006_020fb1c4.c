void func_ov006_020fb1c4(char *c, int i){
    short *e = (short*)(c + 0x5bc4 + i*0x1c);
    if(*(unsigned short*)e != 0){
        *e = *(unsigned short*)e - 1;
        if(*e < 0) *e = 0;
        return;
    }
    *(unsigned char*)(c+i*0x1c+0x5000+0xbc8)=2;
    *(unsigned char*)(c+0x5bca+i*0x1c) = *(unsigned char*)(c+0x5bca+i*0x1c) - 1;
}
