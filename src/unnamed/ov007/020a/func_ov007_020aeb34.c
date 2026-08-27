typedef long long s64;
void func_ov007_020aeb34(char *c, int t){
    short *m = *(short**)(c+0x24);
    int ip = m[3] << 12;
    if(t < 0x1000){
        if(t <= 0) ip = 0;
        else ip = (int)(((s64)t * ip) >> 12);
    }
    int r3 = m[4] << 12;
    if(t < 0x1000){
        if(t <= 0) r3 = 0;
        else r3 = (int)(((s64)t * r3) >> 12);
    }
    *(int*)(c+0x8) = ip + (m[1] << 12);
    *(int*)(c+0xc) = r3 + (m[2] << 12);
}
