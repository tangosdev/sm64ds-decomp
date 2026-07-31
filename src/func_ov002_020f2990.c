#include "types.h"
struct OamAttr;
extern int GetOwnerLanguage(void);
extern void _ZN3OAM6RenderEbP7OamAttriiii5Fix12IiES3_ii(
    int a, struct OamAttr* attr, int x, int y, int e, int f, int g, int h, int i, int j);

extern struct OamAttr* data_ov002_0210b988[];

void func_ov002_020f2990(char* c)
{
    int x, y;
    int idx;

    if (*(u8*)(c + 0x1d9) == 0) return;

    x = *(int*)(c + 0x1cc) >> 0xc;
    y = *(int*)(c + 0x1d0) >> 0xc;
    idx = 0;
    if (GetOwnerLanguage() == 3) idx = 1;
    else if (GetOwnerLanguage() == 2 || GetOwnerLanguage() == 5) idx = 2;
    else if (GetOwnerLanguage() == 4) idx = 3;

    _ZN3OAM6RenderEbP7OamAttriiii5Fix12IiES3_ii(
        0, data_ov002_0210b988[idx], x, y, -1, 1, 0x1000, 0x1000, 0, -1);
}
