typedef unsigned int u32;
typedef unsigned short u16;
typedef unsigned char u8;
typedef signed int s32;

extern s32 GetGameLanguage(void);
extern void Hud_RenderSprite(void *fn, s32 a, s32 b, s32 r3arg, s32 stack_arg);
extern void **data_ov004_020bbfa8[];

void func_ov004_020b0d8c(void *c, s32 arg1, s32 arg2)
{
    s32 idx1 = *(s32*)((char*)c + 0x5c);
    s32 r4 = (idx1 < 0x14) ? 0x11 : 0x12;
    s32 idx2 = GetGameLanguage();
    void **table = (void**)data_ov004_020bbfa8[idx2];
    void *fn = table[r4];
    Hud_RenderSprite(fn, arg1, arg2, -1, -1);
}
