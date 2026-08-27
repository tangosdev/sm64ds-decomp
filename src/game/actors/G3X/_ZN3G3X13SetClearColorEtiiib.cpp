//cpp

struct G3X {
    static void SetClearColor(unsigned short a, int b, int c, int d, bool e);
};

void G3X::SetClearColor(unsigned short a, int b, int c, int d, bool e) {
    unsigned int v = (a | (b << 16)) | (d << 24);
    if (*(int *)&e) v |= 0x8000;
    *(volatile unsigned int *)0x4000350 = v;
    *(volatile unsigned short *)0x4000354 = (unsigned short)c;
}
