//cpp
typedef unsigned int u32;
typedef unsigned char u8;

struct SharedFilePtr;

extern "C" void _ZN13SharedFilePtr7ReleaseEv(SharedFilePtr *self);
extern "C" void _ZN6Player18SetNewHatCharacterEjjb(void *self, u32 a, u32 b, bool c);
extern "C" void _ZN9Animation8LoadFileER13SharedFilePtr(SharedFilePtr &f);
extern "C" void func_ov002_020e6330(void *self);
extern "C" void _ZN6Player4HealEi(void *self, int hp);
extern "C" u32 _ZNK6Player14GetBodyModelIDEjb(void *self, u32 a, bool b);
extern "C" void _ZN10ModelAnim24CopyERKS_Pcj(void *self, void *src, char *p, u32 n);
extern "C" void _ZN10ModelAnim213Func_020162C4Eji5Fix12IiEt(void *self, u32 a, int b, int c, unsigned short d);

extern SharedFilePtr *_ZN6Player9ANIM_PTRSE[];
extern u8 NEXT_HAT_CHARACTER_ARR[];
extern u8 SAVE_DATA[];

extern "C" void _ZN6Player16SetRealCharacterEj(char *self, u32 chr)
{
    u32 cur = *(u32 *)(self + 8);
    u32 base = *(u32 *)(self + 0x63c);
    u32 m1, m2;

    _ZN13SharedFilePtr7ReleaseEv(_ZN6Player9ANIM_PTRSE[base + (cur & 3)]);
    _ZN6Player18SetNewHatCharacterEjjb(self, chr, 0, 1);
    *(u32 *)(self + 8) = chr;
    *(u8 *)(self + 0x6d9) = (u8)chr;
    NEXT_HAT_CHARACTER_ARR[*(u8 *)(self + 0x6d8)] = (u8)*(u32 *)(self + 8);
    SAVE_DATA[0x41] = (u8)*(u32 *)(self + 8);
    _ZN9Animation8LoadFileER13SharedFilePtr(*_ZN6Player9ANIM_PTRSE[*(u32 *)(self + 0x63c) + (*(u32 *)(self + 8) & 3)]);
    func_ov002_020e6330(self);
    _ZN6Player4HealEi(self, 0x880);
    *(unsigned short *)(self + 0x73c) = 0;

    m1 = _ZNK6Player14GetBodyModelIDEjb(self, chr, 0);
    m2 = _ZNK6Player14GetBodyModelIDEjb(self, *(u32 *)(self + 8) & 0xff, 0);
    _ZN10ModelAnim24CopyERKS_Pcj(
        *(void **)(self + m1 * 4 + 0xdc),
        *(void **)(self + m2 * 4 + 0xdc),
        *(char **)((char *)_ZN6Player9ANIM_PTRSE[*(u32 *)(self + 0x63c) + chr] + 4),
        0);
    m1 = _ZNK6Player14GetBodyModelIDEjb(self, chr, 0);
    _ZN10ModelAnim213Func_020162C4Eji5Fix12IiEt(
        *(void **)(self + m1 * 4 + 0xdc),
        *(int *)((char *)_ZN6Player9ANIM_PTRSE[chr + 0xc4] + 4), 0, 0x1000, 0);
}
