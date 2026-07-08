struct Arg { void *m[3]; };

extern int func_ov002_020b676c(unsigned char *self, struct Arg *a, short arg2);
extern short data_ov015_02114794;
extern struct Arg data_ov015_021147a4;

int func_ov015_02112c98(unsigned char *self)
{
    return func_ov002_020b676c(self, &data_ov015_021147a4, data_ov015_02114794);
}
