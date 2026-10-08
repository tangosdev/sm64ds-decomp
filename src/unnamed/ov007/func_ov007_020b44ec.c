extern int func_ov007_020b58e4(void* t);
extern void func_ov007_020ae834(void* t);
extern int func_ov007_020b5068(void* t);
extern void func_ov007_020b50e8(void* t);
extern void func_ov007_020b5bf0(void* t);

void func_ov007_020b44ec(void* self) {
    int* o = *(int**)self;
    void* p24 = *(void**)((char*)o + 0x24);
    if (*(unsigned short*)p24 == 0xa) return;
    void* p4 = *(void**)((char*)o + 4);
    if (*(int*)((char*)p4 + 4) == 0xff) return;
    short r = (short)func_ov007_020b58e4(self);
    p4 = *(void**)((char*)o + 4);
    *(short*)((char*)p4 + 2) = r;
    func_ov007_020ae834(o);
    p4 = *(void**)((char*)o + 4);
    switch (*(short*)p4) {
        case 0:
            func_ov007_020b5068(self);
            break;
        case 2:
            func_ov007_020b50e8(self);
            break;
        case 1:
        case 3:
        case 4:
        case 5:
        case 6:
        case 7:
            break;
    }
    if (*(int*)((char*)o + 0x20) == 0) return;
    func_ov007_020b5bf0(self);
}
