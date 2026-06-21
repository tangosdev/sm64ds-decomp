//cpp
struct SharedFilePtr;
struct Model;
extern void* data_ov002_0210bae4;
extern void func_020072c0();
extern "C" {

void _ZN13SharedFilePtr7ReleaseEv(SharedFilePtr* self);
void _ZN5ModelD2Ev(Model* self);
void func_0207328c(void* p, int a, int b, void* fn);
void _ZN6Memory16operator_delete2EPv(void* p);

void* func_ov002_020f69a8(Model* self)
{
    char* p = (char*)self;
    *(void**)p = (void*)&data_ov002_0210bae4;
    SharedFilePtr* sfp = *(SharedFilePtr**)(p + 0x5c);
    if (sfp)
        _ZN13SharedFilePtr7ReleaseEv(sfp);
    _ZN5ModelD2Ev(self);
    func_0207328c(p + 0x50, 1, 0xc, (void*)func_020072c0);
    _ZN6Memory16operator_delete2EPv(self);
    return self;
}

}
