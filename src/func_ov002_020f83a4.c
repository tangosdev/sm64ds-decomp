typedef struct Obj Obj;
struct Obj {
    char pad0[8];
    unsigned int unk8;
    char pad_c[0x74];
    int unk80;
    int unk84;
    int unk88;
    char pad_8c[0x50];
    void *unkDC;
    void *unkE0;
};

extern void *_Znwj(int sz);
extern void func_ov002_020f6a50(void *p);
extern void _ZN9ModelAnimC2Ev(void *p);
extern void _ZN5ModelC2Ev(void *p);
extern int func_ov002_020f6618(void *self, void *mdl, int nAnims, void *anims, int a5, int texByte, void *texs, int tsData);
extern int func_ov002_020f6960(void *self, void *fp, int n);

extern unsigned char data_0209f2d8;
extern char data_ov002_0210b600;
extern char data_ov002_0210b604;
extern char data_ov002_0210b608;
extern char data_ov002_0210b60c;
extern char data_ov002_0210b610;
extern char data_ov002_0210bae4;
extern char data_ov002_0210bcc4;
extern char data_ov002_0210bce8;
extern char data_ov002_0210bcf0;
extern char data_ov002_0210bd24;
extern char data_ov002_0211094c;
extern char data_ov002_02110b50;
extern char data_ov002_02110b70;
extern char data_ov002_02110b78;
extern char data_ov002_02110b98;
extern char data_ov002_02110c18;
extern char data_ov085_0213074c;
extern char func_02113c20;

int func_ov002_020f83a4(Obj *arg0) {
    void *p;
    int t;

    if (arg0->unk8 == 0x12) {
        p = _Znwj(0x84);
        if (p) {
            func_ov002_020f6a50((char *)p + 0x64);
            _ZN9ModelAnimC2Ev(p);
            *(void **)p = &data_ov002_0210bcc4;
            *(void **)((char *)p + 0x50) = &data_ov002_0210bce8;
        }
        arg0->unkE0 = p;
        if (arg0->unkE0 == 0) return 0;
        if (func_ov002_020f6618(arg0->unkE0, &data_ov085_0213074c, 1, &data_ov002_0210b60c, 1, 1, &data_ov002_0210b608, -1) == 0) return 0;
    } else if (arg0->unk8 == 0x13) {
        p = _Znwj(0x84);
        if (p) {
            func_ov002_020f6a50((char *)p + 0x64);
            _ZN9ModelAnimC2Ev(p);
            *(void **)p = &data_ov002_0210bcc4;
            *(void **)((char *)p + 0x50) = &data_ov002_0210bce8;
        }
        arg0->unkE0 = p;
        if (arg0->unkE0 == 0) return 0;
        t = (*(volatile unsigned char *)&data_0209f2d8 == 2);
        if (t == 0) {
            if (func_ov002_020f6618(arg0->unkE0, &data_ov002_02110b98, 1, &data_ov002_0210b610, 1, 1, &data_ov002_0210b600, 0x16) == 0) return 0;
        } else {
            if (func_ov002_020f6618(arg0->unkE0, &data_ov002_02110c18, 0xD, &data_ov002_0210bcf0, 1, 0xD, &data_ov002_0210bd24, 0x16) == 0) return 0;
        }
    } else if (arg0->unk8 >= 0x14 && arg0->unk8 <= 0x16) {
        p = _Znwj(0x60);
        if (p) {
            func_ov002_020f6a50((char *)p + 0x50);
            _ZN5ModelC2Ev(p);
            *(void **)p = &data_ov002_0210bae4;
        }
        arg0->unkDC = p;
        if (arg0->unkDC == 0) return 0;
        if (func_ov002_020f6960(arg0->unkDC, &data_ov002_02110b70, -1) == 0) return 0;
    } else if (arg0->unk8 == 0x17) {
        p = _Znwj(0x60);
        if (p) {
            func_ov002_020f6a50((char *)p + 0x50);
            _ZN5ModelC2Ev(p);
            *(void **)p = &data_ov002_0210bae4;
        }
        arg0->unkDC = p;
        if (arg0->unkDC == 0) return 0;
        if (func_ov002_020f6960(arg0->unkDC, &data_ov002_02110b50, 0x19) == 0) return 0;
    } else if (arg0->unk8 == 0x18) {
        p = _Znwj(0x60);
        if (p) {
            func_ov002_020f6a50((char *)p + 0x50);
            _ZN5ModelC2Ev(p);
            *(void **)p = &data_ov002_0210bae4;
        }
        arg0->unkDC = p;
        if (arg0->unkDC == 0) return 0;
        if (func_ov002_020f6960(arg0->unkDC, &data_ov002_0211094c, -1) == 0) return 0;
    } else if (arg0->unk8 == 0x19) {
        p = _Znwj(0x60);
        if (p) {
            func_ov002_020f6a50((char *)p + 0x50);
            _ZN5ModelC2Ev(p);
            *(void **)p = &data_ov002_0210bae4;
        }
        arg0->unkDC = p;
        if (arg0->unkDC == 0) return 0;
        if (func_ov002_020f6960(arg0->unkDC, &data_ov002_02110b78, 0x13) == 0) return 0;
    } else if (arg0->unk8 >= 0x1A && arg0->unk8 <= 0x2D) {
        p = _Znwj(0x84);
        if (p) {
            func_ov002_020f6a50((char *)p + 0x64);
            _ZN9ModelAnimC2Ev(p);
            *(void **)p = &data_ov002_0210bcc4;
            *(void **)((char *)p + 0x50) = &data_ov002_0210bce8;
        }
        arg0->unkE0 = p;
        if (arg0->unkE0 == 0) return 0;
        if (func_ov002_020f6618(arg0->unkE0, &func_02113c20, 1, &data_ov002_0210b604, 0, 0, 0, -1) == 0) return 0;
    }
    arg0->unk80 = 0x1000;
    arg0->unk84 = 0x1000;
    arg0->unk88 = 0x1000;
    return 1;
}
