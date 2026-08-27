extern unsigned short data_ov006_0212ef4c[];
void func_ov006_0211bbe0(char *c, int idx);
void func_ov006_0211bbe0(char *c, int idx){
    int o = idx * 0x14;
    char *cb = c + 0x50f0;
    char *p = c + o;
    unsigned char b;
    *(unsigned short*)(cb + o) = (unsigned short)(*(unsigned short*)(cb + o) + 1);
    b = ((unsigned char *)p)[0x50f7];
    if (*(unsigned short*)(cb + o) < data_ov006_0212ef4c[b]) return;
    *(unsigned short*)(cb + o) = 0;
    ((unsigned char *)(c + 0x50f7))[o] = (unsigned char)(((unsigned char *)(c + 0x50f7))[o] - 1);
    if (((unsigned char *)(c + 0x50f7))[o] == 0) ((unsigned char *)p)[0x50f4] = 0;
}
