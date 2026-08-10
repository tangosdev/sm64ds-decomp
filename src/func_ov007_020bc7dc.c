extern int _ZN4cstd3divEii(int a, int b);
extern void func_ov007_020c9110(void* p);
extern void _ZN15ModelComponents11UpdateBonesEP8BCA_Filei(void* model, void* bca, int frame);
extern void func_0204531c(void* model, int x);

void func_ov007_020bc7dc(char* c)
{
    void* model = *(void**)c;
    void* p = *(void**)(c + 0x88);
    void* entry = *(void**)(*(char**)(c + 0x14) + *(short*)((char*)p + 0xc) * 4);
    int quot = _ZN4cstd3divEii(*(short*)(c + 0x8c) << 12, *(short*)(c + 0x8e));

    if (*(int*)(c + 0x98) != 0)
        func_ov007_020c9110(*(void**)(c + 0x88));

    p = *(void**)(c + 0x88);
    if (*(short*)((char*)p + 0xc) != -1)
        _ZN15ModelComponents11UpdateBonesEP8BCA_Filei(model, entry, *(unsigned short*)((char*)p + 0x10));

    func_0204531c(model, quot);

    if (*(int*)(c + 0x98) == 0)
        return;

    *(short*)(c + 0x8c) -= 1;
    if (*(short*)(c + 0x8c) <= 0)
        *(short*)(c + 0x8c) = 0;
}
