struct S { char pad[0x34]; int f34; };
struct G { char pad[0xc]; int fc; };
extern struct G data_020a1fc0;
extern void func_02041a00(struct S *self);

int func_02042668(struct S *self, int sel)
{
    switch (sel) {
    case 0:
        if (data_020a1fc0.fc != 2) return 5;
        if (self->f34 == 0) return 0;
        func_02041a00(self);
        return 6;
    case 1:
        return 4;
    default:
        return 8;
    }
}
