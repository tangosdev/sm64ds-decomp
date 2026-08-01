#include "types.h"
typedef struct { s16 res; s16 pad; int name; } E8;

int func_020471ac(const void* tbl, const char* name);
int func_02047144(const void* tbl, const char* name);
int func_02046590(const void* tbl, const char* name);

struct Tbl {
    u16 f0;
    u16 cnt1;
    E8* p1;
    u16 cnt2;
    E8* p2;
    int pad[3];
    u16 cnt3;
    char* p3;
};

void func_02046d50(void* arg, struct Tbl* t)
{
    int i;
    E8* e;

    e = t->p1;
    for (i = 0; i < t->cnt1; i++, e++) {
        e->res = func_020471ac(arg, (char*)e->name);
    }
    e = t->p2;
    for (i = 0; i < t->cnt2; i++, e++) {
        e->res = func_02047144(arg, (char*)e->name);
    }
    for (i = 0; i < t->cnt3; i++) {
        *(s16*)(t->p3 + i * 0xc) = func_02046590(arg, (char*)((E8*)(t->p3 + i * 0xc))->name);
    }
}
