extern int func_ov064_02116110(char* self, short step);
int func_ov064_021171b0(char* self){
    if(*(unsigned short*)(self+0x100) < 0xa){
        *(int*)(self+0x98) = 0;
        int r = func_ov064_02116110(self, 0x700);
        if(r != 0) return r;
        unsigned short* p = (unsigned short*)(self+0x100);
        if(*p == 9) *p = 0;
        return (int)p;
    }
    *(int*)(self+0x98) = 0xf000;
    unsigned short* p = (unsigned short*)(self+0x100);
    unsigned int v = *p;
    if(v > 0x2c){ v = 0; *p = 0; }
    return v;
}
