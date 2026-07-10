//cpp
typedef unsigned int u32;
typedef unsigned char u8;
typedef unsigned short u16;

extern "C" void func_ov002_020bda48(char *self);
extern "C" int func_ov002_020bea7c(char *self);
extern "C" void _ZN6Player18TurnOffToonShadingEj(char *self, u32 a);
extern "C" u32 _ZNK6Player14GetBodyModelIDEjb(char *self, u32 a, bool b);
extern "C" void _ZN10ModelAnim24CopyERKS_Pcj(void *self, void *src, char *p, u32 n);
extern "C" void func_ov002_020bd9ec(char *self, u32 a);
extern "C" void func_ov002_020c43c4(char *self, u32 a);

extern char *data_ov002_020ff480[];

extern "C" void _ZN6Player14InitMetalWarioEv(char *self)
{
    u32 v;
    u32 id;

    func_ov002_020bda48(self);
    if (func_ov002_020bea7c(self) != 0) {
        u32 b = *(u8 *)(self + 0x6dd);
        if (b != 2)
            return;
        _ZN6Player18TurnOffToonShadingEj(self, b);
        _ZN6Player18TurnOffToonShadingEj(self, *(u8 *)(self + 0x6dc));
        *(u16 *)(self + 0x73c) = 0;
        *(u32 *)(self + 8) = *(u8 *)(self + 0x6dd);
        *(u8 *)(self + 0x71a) = 0;
    }

    *(u8 *)(self + 0x6f9) = 1;

    v = *(u32 *)(self + 8);
    id = _ZNK6Player14GetBodyModelIDEjb(self, v & 0xff, 0);
    _ZN10ModelAnim24CopyERKS_Pcj(
        *(void **)(self + id * 4 + 0xdc),
        *(void **)(self + v * 4 + 0xdc),
        *(char **)((char *)data_ov002_020ff480[*(u32 *)(self + 0x63c) + v] + 4),
        0);

    *(u16 *)(self + 0x6ae) = 0x258;
    func_ov002_020bd9ec(self, 0x34);
    func_ov002_020c43c4(self, 4);
}
