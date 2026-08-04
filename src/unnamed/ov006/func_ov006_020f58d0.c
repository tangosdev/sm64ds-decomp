void func_ov006_020f58d0(char *c, int i){
    short *e = (short*)(c + 0x5396 + i*0x14);
    if(*(unsigned short*)e != 0){
        *e = *(unsigned short*)e - 1;
        if(*e < 0) *e = 0;
        return;
    }
    *(int*)(c+i*0x14+0x5000+0x388)=0xc000;
    *(int*)(c+i*0x14+0x5000+0x38c)=0xc000;
    *(unsigned char*)(c+i*0x14+0x5000+0x399)=1;
    *(unsigned char*)(c+i*0x14+0x5000+0x39a)=1;
    *(int*)(c+i*0x14+0x5000+0x390)=0xc00;
}
