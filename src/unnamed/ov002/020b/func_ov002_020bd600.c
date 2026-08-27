typedef short s16;
typedef int s32;
struct Obj { char pad[0x5c]; s32 x; s32 y; s32 z; };
extern s16 ReadUnalignedShort(unsigned char *p);
s32 func_ov002_020bd600(char *self, unsigned char *data)
{
    s32 x, y, z;
    z = ReadUnalignedShort(data + 4) << 12;
    y = ReadUnalignedShort(data + 2) << 12;
    x = ReadUnalignedShort(data) << 12;
    *(s32*)(self + 0x5c) = x;
    *(s32*)(self + 0x60) = y;
    *(s32*)(self + 0x64) = z;
    *(s16*)(self + 0x8e) = ReadUnalignedShort(data + 6);
    *(s16*)(self + 0x94) = *(s16*)(self + 0x8e);
    return 1;
}
