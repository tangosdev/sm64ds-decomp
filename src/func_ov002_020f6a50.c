extern void func_020733a8(void* obj, int a, int b, void* cb1, void* cb2);
extern void _ZN7Vector3D1Ev(void);
extern void func_0203d384(void);

void* func_ov002_020f6a50(char* c)
{
    func_020733a8(c, 1, 0xc, (void*)func_0203d384, (void*)_ZN7Vector3D1Ev);
    *(int*)c = 0;
    *(int*)(c+4) = 0;
    *(int*)(c+8) = 0;
    return c;
}
