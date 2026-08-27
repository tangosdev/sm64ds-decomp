extern void func_ov027_02111d70(char* c, int x);
extern void _ZN8dActor_c9UpdatePosEP5dCc_c(char* c, void* cyl);
extern void func_0201267c(int a, char* b);
extern int data_ov027_02113c74[];
extern int data_ov027_02113c8c[];
int func_ov027_02111a28(char* c) {
    int d = *(int*)(c+0x3d4);
    if (d == 0) {
        unsigned char* p = (unsigned char*)(((int)c + 0x3d9));
        *p = *p + 1;
        if (*(unsigned char*)(c+0x3d9) >= 9) *(unsigned char*)(c+0x3d9) = 0;
        func_ov027_02111d70(c, 0);
        return 1;
    }
    {
        int v = *(int*)(c+0x98);
        if (d < v) {
            *(int*)(c+0x98) = d;
            *(int*)(c+0x3d4) = 0;
        } else {
            int* q = (int*)(((int)c + 0x3d4));
            *q = *q - v;
        }
    }
    _ZN8dActor_c9UpdatePosEP5dCc_c(c, (void*)(c+0x398));
    {
        int t = *(int*)(c+0x378);
        int w = data_ov027_02113c74[1];
        int field = w ? *(int*)(c+0x380) : *(int*)(c+0x380);
        unsigned int id = (unsigned int)(t << 4) >> 0x10;
        if (field == w) {
            if (id == 0xa || id == 0x16) {
                func_0201267c(0xf3, c+0x74);
            }
        } else if (field == data_ov027_02113c8c[1]) {
            if (id == 9 || id == 0x16) {
                func_0201267c(0xf3, c+0x74);
            }
        }
    }
    return 1;
}
