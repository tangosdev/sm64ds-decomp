extern int _ZNK6Player14GetBodyModelIDEjb(void* thiz, unsigned int a, int b);
extern int _ZN6Player6IsAnimEj(void* thiz, unsigned int a);
extern void func_0201628c(int a, int b);
extern void func_020162a4(int a, int b);

int func_ov002_020e4990(char* c)
{
    int id;
    int b;
    id = _ZNK6Player14GetBodyModelIDEjb(c, *(unsigned int*)(c + 8) & 0xff, 0);
    func_0201628c(((int*)(c + 0xdc))[id], 8);
    b = *(int*)(c + 0x358) ? 1 : 0;
    if (b == 0)
        return 0;
    if (_ZN6Player6IsAnimEj(c, 0x30) || _ZN6Player6IsAnimEj(c, 0x2f) ||
        _ZN6Player6IsAnimEj(c, 0x33) || _ZN6Player6IsAnimEj(c, 0x18) ||
        _ZN6Player6IsAnimEj(c, 0x8a) || _ZN6Player6IsAnimEj(c, 0x87) ||
        _ZN6Player6IsAnimEj(c, 0x8b) || _ZN6Player6IsAnimEj(c, 0x6d) ||
        _ZN6Player6IsAnimEj(c, 0x6f) || _ZN6Player6IsAnimEj(c, 0x6c) ||
        _ZN6Player6IsAnimEj(c, 0x91) || _ZN6Player6IsAnimEj(c, 0x8e) ||
        _ZN6Player6IsAnimEj(c, 0x90) || _ZN6Player6IsAnimEj(c, 0x8f) ||
        _ZN6Player6IsAnimEj(c, 0x43) || _ZN6Player6IsAnimEj(c, 0x5b) ||
        _ZN6Player6IsAnimEj(c, 0x58) || _ZN6Player6IsAnimEj(c, 0x81) ||
        _ZN6Player6IsAnimEj(c, 0x80) || _ZN6Player6IsAnimEj(c, 0xaf) ||
        _ZN6Player6IsAnimEj(c, 0xb0))
        return 0;
    id = _ZNK6Player14GetBodyModelIDEjb(c, *(unsigned int*)(c + 8) & 0xff, 0);
    func_020162a4(((int*)(c + 0xdc))[id], 8);
    return 1;
}
