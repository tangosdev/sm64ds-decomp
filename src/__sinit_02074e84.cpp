//cpp
extern char data_0208ee14;
extern char data_02092188;
extern char data_0209f388;
extern void *volatile data_0209f3c4;
extern char data_0209f43c;
extern "C" void _ZN7ClipperC1Ev(void *thiz);
extern "C" void _ZN7ClipperD1Ev(void);
extern "C" void func_020731dc(void *a, void (*dtor)(void), void *node);

extern "C" void __sinit_02074e84(void);
void __sinit_02074e84(void) {
    data_0209f3c4 = &data_0208ee14;
    data_0209f3c4 = &data_02092188;
    _ZN7ClipperC1Ev(&data_0209f43c);
    func_020731dc(&data_0209f43c, _ZN7ClipperD1Ev, &data_0209f388);
}
