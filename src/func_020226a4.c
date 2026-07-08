extern void *_ZTVN8Particle8CallbackE;
extern void *_ZTVN8Particle14SimpleCallbackE;
void func_020226a4(char *p)
{
    *(void **)p = &_ZTVN8Particle8CallbackE;
    *(void **)p = &_ZTVN8Particle14SimpleCallbackE;
    *(short *)(p + 4) = 0;
}
