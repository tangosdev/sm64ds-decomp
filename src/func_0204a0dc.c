typedef unsigned int u32;
typedef unsigned short u16;

extern void _ZN2GX12BeginLoadTexEv(void);
extern u32 _ZN2GX7LoadTexEPKvjj(const void *p, u32 a, u32 b);
extern void _ZN2GX10EndLoadTexEv(void);

typedef struct TexData {
    int f0;
    u32 f4lo : 4;
    u32 f4hi : 28;
    int f8;
} TexData;

typedef struct Elem {
    TexData *tex;
    u32 result;
    char pad[0x14 - 8];
} Elem;

typedef struct Obj {
    char pad20[0x20];
    Elem *list;
    char pad24[0x26 - 0x24];
    u16 count;
} Obj;

int func_0204a0dc(Obj *thiz, u32 (*cb)(const void *, u32))
{
    int i;
    _ZN2GX12BeginLoadTexEv();
    for (i = 0; i < thiz->count; i++) {
        Elem *e = (Elem *)((char *)thiz->list + i * 0x14);
        TexData *t = e->tex;
        u32 flag = (t->f4lo == 5) ? 1 : 0;
        u32 r = cb((const void *)t->f8, flag);
        _ZN2GX7LoadTexEPKvjj((const void *)((char *)e->tex + 0x20), r, (u32)t->f8);
        e->result = r;
    }
    _ZN2GX10EndLoadTexEv();
    return 1;
}
