typedef struct { int a, b, c, d; } Item16;

extern Item16 data_ov071_02123088[];
extern void func_ov071_021215fc(void *self);

void func_ov071_02121634(char *self, int idx)
{
    *(Item16 **)(self + 0x1e4) = &data_ov071_02123088[idx];
    func_ov071_021215fc(self);
}
