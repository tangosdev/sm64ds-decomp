extern int func_0203fc74(void);
extern void func_02040638(void);
extern int func_0203fbc4(void);
extern int Wireless_Reset(void);
extern int func_0203fb5c(void);

extern int data_020a0f5c;
extern unsigned short data_020a0f24;
extern int data_020a0f94;

struct S { unsigned short f0; unsigned short f2; };

void func_020402a0(struct S *self)
{
    int x;
    if (self->f2 != 0) goto set_flag;
    x = data_020a0f5c;
    data_020a0f24 = 0;
    switch (x) {
    case 0:
        func_0203fc74();
        return;
    case 3:
        func_02040638();
        if (func_0203fbc4() != 0) return;
        data_020a0f5c = 0;
        Wireless_Reset();
        return;
    case 4:
        if (func_0203fb5c() != 0) return;
        data_020a0f5c = 0;
        Wireless_Reset();
        return;
    }
    return;
set_flag:
    data_020a0f94 = 1;
}
