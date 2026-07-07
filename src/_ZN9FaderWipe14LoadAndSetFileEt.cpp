//cpp
extern "C" void _ZN5Model14LoadAndSetFileEtii(void* m, unsigned short id, int a, int b);
extern "C" void _ZN9FaderWipe14LoadAndSetFileEt(void* thiz, unsigned short id) {
    _ZN5Model14LoadAndSetFileEtii((char*)thiz + 0x10, id, 0, -1);
}