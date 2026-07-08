//cpp
extern char data_0208ee14;
extern char data_02092188;
extern char data_0209f388;
extern void *volatile STAGE_SCENE_RELATED;
extern char GLOBAL_CLIPPER;
extern "C" void _ZN7ClipperC1Ev(void *thiz);
extern "C" void _ZN7ClipperD1Ev(void);
extern "C" void func_020731dc(void *a, void (*dtor)(void), void *node);

extern "C" void __sinit_02074e84(void);
void __sinit_02074e84(void) {
    STAGE_SCENE_RELATED = &data_0208ee14;
    STAGE_SCENE_RELATED = &data_02092188;
    _ZN7ClipperC1Ev(&GLOBAL_CLIPPER);
    func_020731dc(&GLOBAL_CLIPPER, _ZN7ClipperD1Ev, &data_0209f388);
}
