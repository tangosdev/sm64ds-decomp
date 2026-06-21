//cpp
extern "C" {

void* _ZN9ActorBasenwEj(unsigned int sz);
void _ZN5ActorC2Ev(void* self);
void _ZN5ModelD1Ev();
void _ZN5ModelC1Ev();
void func_020733a8(void* base, unsigned int count, unsigned int sz,
                   void (*ctor)(), void (*dtor)());

extern void* data_ov002_0210ac00;

void* func_ov002_020ec32c()
{
    char* self = (char*)_ZN9ActorBasenwEj(0x264);
    if (self) {
        _ZN5ActorC2Ev(self);
        *(void**)self = &data_ov002_0210ac00;
        func_020733a8(self + 0xd4, 5, 0x50, _ZN5ModelC1Ev, _ZN5ModelD1Ev);
    }
    return self;
}

}
