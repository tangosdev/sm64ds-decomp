void func_ov006_0210ab08(char *c, int idx);
void func_ov006_0210ab08(char *c, int idx){
    int d = (*(int*)(c + idx*4 + 0x4fe4) >> 12) / 80;
    int i;
    char *dst = c + idx*3;
    char *src = c + idx*5;
    for (i = 0; i < 3; i++) {
        *(unsigned char*)(dst + 0x5031) = *(unsigned char*)(src + d + 0x501c);
        d = (d + 1) % *(unsigned char*)(c + 0x503a);
        dst += 1;
    }
}
