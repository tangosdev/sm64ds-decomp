extern void* _ZN9ActorBasenwEj(unsigned int);
extern void _ZN9ActorBaseC1Ev(void*);
extern void func_020733a8(void*, int, int, void*, void*);

struct P2 { int a, b; };
extern char data_0208e4b8[];
extern char _ZTV5Stage[];
extern char data_ov075_0211d304[];
extern struct P2 data_02086b58;
extern void func_ov075_02115bac(void);
extern void func_ov075_0211a838(void);
extern void func_ov075_02115bc8(void);
extern void func_ov075_0211a834(void);

void* func_ov075_0211a740(void)
{
    char* p = (char*)_ZN9ActorBasenwEj(0x288);
    if (p) {
        struct P2 v;
        _ZN9ActorBaseC1Ev(p);
        *(char**)p = data_0208e4b8;
        *(char**)p = _ZTV5Stage;
        {
            unsigned char* f = (unsigned char*)(((int)p + 0x13) & 0xFFFFFFFFFFFFFFFF);
            *f |= 1;
            *f |= 4;
        }
        *(char**)p = data_ov075_0211d304;
        v = data_02086b58;
        *(struct P2*)(p + 0x54) = v;
        *(struct P2*)(p + 0x5c) = v;
        *(struct P2*)(p + 0x64) = v;
        func_020733a8(p + 0x70, 9, 0x24, func_ov075_0211a838, func_ov075_02115bac);
        func_020733a8(p + 0x1b4, 4, 0x2c, func_ov075_0211a834, func_ov075_02115bc8);
    }
    return p;
}
