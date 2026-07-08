//cpp
extern void *_ZTVN8Particle8CallbackE;
extern void *_ZTVN8Particle14SimpleCallbackE;
extern "C" void _ZN8Particle14SimpleCallbackC2Ev(char *p);
extern "C" void _ZN8Particle14SimpleCallbackC2Ev(char *p)
{
    *(void **)p = &_ZTVN8Particle8CallbackE;
    *(void **)p = &_ZTVN8Particle14SimpleCallbackE;
    *(short *)(p + 4) = 0;
}
