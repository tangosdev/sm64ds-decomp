extern char* _ZN5Actor10FindWithIDEj(unsigned int id);
extern void func_02035860(void* o, void* src);
extern void func_ov002_020e9464(char* p);

void func_ov002_020e7454(char* self) {
    char* a = _ZN5Actor10FindWithIDEj(*(unsigned int*)(self + 0x434));
    int* s;
    *(unsigned short*)((int)((unsigned long long)(unsigned)(self + 0x4a2))) &= ~0x30;
    s = (int*)((int)(a + 0x5c));
    *(int*)(self + 0x5c) = s[0];
    *(int*)(self + 0x60) = s[1];
    *(int*)(self + 0x64) = s[2];
    func_02035860(self + 0x150, self + 0x5c);
    if (*(unsigned char*)(a + 0x1d8) == 3) {
        *(int*)(self + 0x444) = *(unsigned char*)(a + 0x1da);
        *(int*)(self + 0x440) = *(int*)(self + 0x444);
        if (*(int*)(self + 0x440) != 4) return;
        func_ov002_020e9464(self);
    } else {
        unsigned short* f;
        a = (char*)((int)(a + 0x1db));
        *(unsigned char*)a &= ~1;
        *(unsigned char*)a |= 2;
        *(int*)(self + 0x440) = 9;
        *(int*)((int)(self + 0x128)) |= 1;
        f = (unsigned short*)((int)(self + 0x4a2));
        *f &= ~2;
        *f |= 8;
    }
}
