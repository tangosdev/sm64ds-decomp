typedef unsigned char u8;

extern int _ZNK6Player14GetBodyModelIDEjb(void *self, unsigned int a, int b);
extern int _ZNK9Animation12WillHitFrameEi(void *self, int frame);

extern u8 data_ov002_02109dc4[];

void func_ov002_020d9a4c(char *c) {
    int idx = _ZNK6Player14GetBodyModelIDEjb(c, *(int*)(c + 8) & 0xff, 0);
    int anim = *(int*)(c + idx * 4 + 0xdc);
    if (_ZNK9Animation12WillHitFrameEi((char*)anim + 0x50, data_ov002_02109dc4[*(u8*)(c + 0x6e3) & 7]) != 0)
        *(u8*)(((long long)(int)(c + 0x6e5)) & 0xFFFFFFFFFFFFFFFFLL) |= 1;
}
