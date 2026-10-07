typedef struct FaderNode {
    void *volatile vtbl;
    int field4;
    int field8;
    short fieldC;
} FaderNode;

extern FaderNode data_0209f5e8;
extern FaderNode data_0209f5d0;
extern int _ZTV8dFader_c[];
extern int _ZTV15dFdBrightness_c[];
extern int _ZTV10dFdColor_c[];
extern void *_ZN10dFdColor_cD1Ev(void *self);
extern int data_0209f5dc[];
extern void *_ZN15dFdBrightness_cD1Ev(void *self);
extern int data_0209f5c4[];
extern void func_020731dc(void *node, void *dtor, void *extra);

void __sinit_02074edc(void)
{
    data_0209f5e8.vtbl = _ZTV8dFader_c;
    data_0209f5e8.vtbl = _ZTV15dFdBrightness_c;
    data_0209f5e8.field4 = 0x1000;
    data_0209f5e8.field8 = 0;
    data_0209f5e8.vtbl = _ZTV10dFdColor_c;
    data_0209f5e8.fieldC = 0;
    func_020731dc(&data_0209f5e8, _ZN10dFdColor_cD1Ev, data_0209f5dc);

    data_0209f5d0.vtbl = _ZTV8dFader_c;
    data_0209f5d0.vtbl = _ZTV15dFdBrightness_c;
    data_0209f5d0.field4 = 0x1000;
    data_0209f5d0.field8 = 0;
    func_020731dc(&data_0209f5d0, _ZN15dFdBrightness_cD1Ev, data_0209f5c4);
}
