typedef struct { int a, b, c, d; } Item16;

extern Item16 data_ov070_021236ec[];
extern void func_ov070_0212200c(void *self);

void func_ov070_02122044(char *self, int idx)
{
    *(Item16 **)(self + 0x31c) = &data_ov070_021236ec[idx];
    func_ov070_0212200c(self);
}
