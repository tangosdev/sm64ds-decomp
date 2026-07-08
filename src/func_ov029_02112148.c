struct Arg { void *m[3]; };

extern int func_ov002_020b676c(unsigned char *self, struct Arg *a, short arg2);
extern short data_ov029_02113fc4;
extern struct Arg data_ov029_02113fd4;

int func_ov029_02112148(unsigned char *self)
{
    return func_ov002_020b676c(self, &data_ov029_02113fd4, data_ov029_02113fc4);
}
