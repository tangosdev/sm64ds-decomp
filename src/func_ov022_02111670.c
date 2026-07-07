struct Arg { void *m[3]; };

extern int func_ov002_020b676c(unsigned char *self, struct Arg *a, short arg2);
extern struct Arg data_ov022_02113da4;

void func_ov022_02111670(unsigned char *c)
{
    func_ov002_020b676c(c, &data_ov022_02113da4, 0x100);
}