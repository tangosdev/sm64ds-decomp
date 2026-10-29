extern unsigned char data_0209f1e0;
extern unsigned char data_0209b2fc;
extern unsigned char gMgSaveDirty;
extern void func_0202fd2c(void);
extern void func_0202ed48(void);
extern void func_02011c8c(void);
extern int GetSoundMode(void);
extern void SetSoundMode(int mode);
extern void func_0203bc50(void);
extern void func_0203bb14(void);
extern void ResetInput(void);
extern unsigned char data_0209f1dc;
extern unsigned char data_0209f1d8;
extern unsigned char data_020a0e44;
extern void *data_0209f1e4;

void func_02023544(void) {
    data_0209f1e0 = 0;
    func_0202fd2c();
    data_0209b2fc = 0;
    gMgSaveDirty = 0;
    func_0202ed48();
    func_02011c8c();
    SetSoundMode(GetSoundMode());
}

void func_02023498(void) {
    if (data_0209f1e0 != 0) {
        func_0203bc50();
        func_0203bb14();
        ResetInput();
        return;
    }
    if (data_0209f1dc != 0) {
        if (data_020a0e44 == 0)
            data_0209f1dc = 0;
        return;
    }
    if (data_0209f1d8 != 0)
        return;
    if (data_020a0e44 != 0) {
        data_0209f1e0 = 1;
        data_0209f1dc = 1;
    }
}

void func_0202345c(void) {
    void **o = (void **)data_0209f1e4;
    if (!o)
        return;
    void **vt = (void **)o[0];
    ((void (*)(void *))vt[2])(o);
}
