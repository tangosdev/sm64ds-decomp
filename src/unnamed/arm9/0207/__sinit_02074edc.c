typedef struct FaderNode {
    void *volatile vtbl;
    int field4;
    int field8;
    short fieldC;
} FaderNode;

extern FaderNode data_0209f5e8;
extern FaderNode data_0209f5d0;
extern int data_0208eafc[];
extern int data_0208eacc[];
extern int data_0208eb2c[];
extern void _ZN10FaderColorD1Ev(void);
extern int data_0209f5dc[];
extern void _ZN15FaderBrightnessD1Ev(void);
extern int data_0209f5c4[];
extern void func_020731dc(void *node, void *dtor, void *extra);

void __sinit_02074edc(void)
{
    data_0209f5e8.vtbl = data_0208eafc;
    data_0209f5e8.vtbl = data_0208eacc;
    data_0209f5e8.field4 = 0x1000;
    data_0209f5e8.field8 = 0;
    data_0209f5e8.vtbl = data_0208eb2c;
    data_0209f5e8.fieldC = 0;
    func_020731dc(&data_0209f5e8, _ZN10FaderColorD1Ev, data_0209f5dc);

    data_0209f5d0.vtbl = data_0208eafc;
    data_0209f5d0.vtbl = data_0208eacc;
    data_0209f5d0.field4 = 0x1000;
    data_0209f5d0.field8 = 0;
    func_020731dc(&data_0209f5d0, _ZN15FaderBrightnessD1Ev, data_0209f5c4);
}
