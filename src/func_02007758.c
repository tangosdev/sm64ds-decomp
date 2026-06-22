typedef unsigned int u32;
typedef short s16;
typedef signed char s8;
typedef unsigned char u8;

extern int SublevelToLevel(int i);
extern void func_020070e8(void *a, int b, int c, int d, int e, int f, int g);
extern s8 data_0209f2f8;
extern u8 data_0209f224;

int func_02007758(char *actor)
{
    int amt = 0x1800;
    int level = SublevelToLevel(data_0209f2f8);
    if ((level == 9 && data_0209f224 == 6) || (level == 0xd && data_0209f224 == 4))
        amt = 0x800;
    func_020070e8(actor, 0x320000, 0xcc, amt, 0x10,
                  *(s16 *)(actor + 0x19a), 8);
    return 1;
}
