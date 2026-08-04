extern void func_ov004_020b2444(int a, int b, int c, int d, int e, int f, int g);
void func_ov006_02100140(char *c){
    int i;
    for(i=0;i<0x10;i++){
        if(*(unsigned char*)(c+0x5000+0x4b5) != 0){
            func_ov004_020b2444(
                *(int*)(c+0x5000+0x4a0) >> 12,
                *(int*)(c+0x5000+0x4a4) >> 12,
                *(unsigned char*)(c+0x5000+0x4b6),
                -1, -1, 0, 0);
        }
        c += 0x18;
    }
}
