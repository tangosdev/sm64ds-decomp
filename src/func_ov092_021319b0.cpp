//cpp
extern "C" {
extern void* _ZN5Actor10FindWithIDEj(unsigned int id);
extern void func_ov092_02131878(void* a, void* b, int n);
extern int _ZN5Actor24BumpedUnderneathByPlayerER6Player(void* thiz, void* p);

void func_ov092_021319b0(char* c)
{
    unsigned int id = *(unsigned int*)(c + 0x50c);
    if (id == 0) return;
    char* o = (char*)_ZN5Actor10FindWithIDEj(id);
    if (o == 0) return;
    int b = (*(unsigned short*)(o + 0xc) == 0xbf);
    if (b == 0) return;
    int f = *(int*)(c + 0x508);
    if (f & 0x40) {
        func_ov092_02131878(c, o, 0);
    } else if (f & 0x380) {
        func_ov092_02131878(c, o, 1);
    } else if (_ZN5Actor24BumpedUnderneathByPlayerER6Player(c, o) != 0) {
        if (*(int*)(o + 0x60) > *(int*)(c + 0x60) - 0x64000)
            func_ov092_02131878(c, o, 2);
    }
    *(void**)(c + 0x320) = o;
}
}
