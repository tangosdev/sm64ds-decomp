//cpp
extern char data_0208ee14;
extern char data_02092188;
extern char data_0209f388;
extern void *volatile data_0209f3c4;
extern char data_0209f43c;
/* local extern: data_0209f43c is defined elsewhere as raw storage, so this
   static initializer constructs it by name and registers the destructor by
   address, which C++ cannot take. */
extern "C" void *_ZN8dClipperC1Ev(void *object);
extern "C" void *_ZN8dClipperD1Ev(void *object);
extern "C" void func_020731dc(void *object, void *destructor, void *node);

extern "C" void __sinit_02074e84(void);
void __sinit_02074e84(void) {
    data_0209f3c4 = &data_0208ee14;
    data_0209f3c4 = &data_02092188;
    _ZN8dClipperC1Ev(&data_0209f43c);
    func_020731dc(&data_0209f43c, (void *)_ZN8dClipperD1Ev, &data_0209f388);
}
