extern int data_020a0f5c;
extern int data_020a0f94;
extern int func_0203fc74(void);
extern int func_0203fc38(void);
extern void func_02040638(void);
extern int func_0203fbc4(void);
extern int Wireless_Reset(void);
extern int func_0203fb5c(void);

void func_02040408(unsigned short *arg) {
    if (arg[1] != 0) goto setflag;
    switch (data_020a0f5c) {
    case 0:
        func_0203fc74();
        return;
    case 3:
        if (func_0203fc38() == 0) return;
        func_02040638();
        if (func_0203fbc4() != 0) return;
        data_020a0f5c = 0;
        Wireless_Reset();
        return;
    case 4:
        if (func_0203fc38() != 1) return;
        if (func_0203fb5c() != 0) return;
        data_020a0f5c = 0;
        Wireless_Reset();
        return;
    }
    return;
setflag:
    data_020a0f94 = 1;
}
