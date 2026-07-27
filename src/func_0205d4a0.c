typedef struct UnkStruct_0205d4a0 {
    char pad0[0x10];
    int unk10;
    char pad14[0x18];
    int unk2c;
    int unk30;
    short unk34;
    short unk36;
} UnkStruct_0205d4a0;

extern void func_0205cdf4(UnkStruct_0205d4a0 *self, int a);

#pragma long_calls on
void func_0205d4a0(UnkStruct_0205d4a0 *self, int a, int b)
{
    if (self->unk10 != 5) {
        self->unk34 = 0;
        self->unk36 = 0;
    }
    self->unk2c = a;
    self->unk30 = b;
    func_0205cdf4(self, 5);
}
