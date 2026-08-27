void func_ov006_020f3ba0(char *c, int i){
    short *e = (short*)(c + 0x52d6 + i*0x14);
    if(*(unsigned short*)e != 0){
        *e = *(unsigned short*)e - 1;
        if(*e < 0) *e = 0;
        return;
    }
    *(int*)(c+i*0x14+0x5000+0x2c8)=0xc000;
    *(int*)(c+i*0x14+0x5000+0x2cc)=0xc000;
    *(unsigned char*)(c+i*0x14+0x5000+0x2d9)=1;
    *(unsigned char*)(c+i*0x14+0x5000+0x2da)=1;
    *(int*)(c+i*0x14+0x5000+0x2d0)=0xc00;
}
