typedef unsigned char u8;
typedef short s16;
struct Arg { void *m[3]; };

extern int func_ov002_020b676c(u8 *self, struct Arg *arg, s16 value);
extern s16 data_ov047_02112320;
extern s16 data_ov047_02112324;
extern struct Arg data_ov047_02112334;

int _ZN10RickshawBs13InitResourcesEv(u8 *self)
{
    s16 value = data_ov047_02112320;

    if (((*(int *)(self + 8)) & 0xff) == 1) {
        value = data_ov047_02112324;
    }

    return func_ov002_020b676c(self, &data_ov047_02112334, value);
}
