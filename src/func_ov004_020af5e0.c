typedef unsigned short u16;
typedef unsigned int u32;

typedef struct Item {
    u32 w0;
    u32 val : 10;
    u32 rest : 22;
} Item;

#pragma opt_strength_reduction off

int func_ov004_020af5e0(Item* src, Item* dst, int add)
{
    int n = 1;
    int i = 0;
    Item* q = dst;
    do {
        u32 w;
        u32* p;
        dst[i].w0 = src->w0;
        *(u32*)((char*)&dst[i] + 4) = *(u32*)((char*)src + 4);
        w = (add + src->val) & 0x3ff;
        p = (u32*)(((long long)(int)((char*)q + 4)) & 0xFFFFFFFFFFFFFFFFLL);
        *p = (*p & ~0x3ff) | ((u32)(((long long)(int)w) & 0xFFFFFFFFFFFFFFFFLL) & 0x3ff);
        if (*(u16*)((char*)src + 6) == 0xffff)
            break;
        if (i == 0x1f) {
            *(u16*)((char*)&dst[i] + 6) = 0xffff;
            break;
        }
        i++;
        src++;
        n++;
        q++;
    } while (i < 0x20);
    return n;
}
