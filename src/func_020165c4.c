#include "types.h"
extern u32 func_020462bc(int a);
extern void* _ZN6Memory13operator_new2Ej(u32 size);
extern void func_020462b4(int* p, int v);

int func_020165c4(char* this, int arg1)
{
    *(void**)(this + 0x6c) = _ZN6Memory13operator_new2Ej(func_020462bc(arg1));
    int* p = *(int**)(this + 0x6c);
    if (p == 0) return 0;
    func_020462b4((int*)(this + 8), (int)p);
    return 1;
}
