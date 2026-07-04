extern void func_ov002_020b16c4(char* c, char* p);
extern void func_ov002_020b1674(char* c, char* p);
extern void func_ov002_020b1884(char* c, char* p);

void func_ov002_020b2a34(char* c, char* p)
{
    int state = *(int*)(c + 0x3a0);
    if (state == 1) {
        *(int*)(((long long)(int)(c + 0x60)) & 0xFFFFFFFFFFFFFFFFLL) += 0x50000;
        func_ov002_020b16c4(c, p);
        return;
    }
    if (state == 2) {
        func_ov002_020b1674(c, p);
        return;
    }
    func_ov002_020b1884(c, p);
}
