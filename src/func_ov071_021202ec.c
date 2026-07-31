typedef struct { int a, b, c, d; } Item16;

extern Item16 data_ov071_02122fa8[];
extern void func_ov071_021202b4(void *self);

void func_ov071_021202ec(char *self, int idx)
{
    *(Item16 **)(self + 0x380) = &data_ov071_02122fa8[idx];
    func_ov071_021202b4(self);
}
