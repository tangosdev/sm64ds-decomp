typedef struct { int a, b, c, d; } Item16;

extern Item16 data_ov030_02115e0c[];
extern void func_ov030_02114170(void *self);

void func_ov030_021141a8(char *self, int idx)
{
    *(Item16 **)(self + 0x3a4) = &data_ov030_02115e0c[idx];
    func_ov030_02114170(self);
}
