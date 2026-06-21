//cpp
struct SharedFilePtr { void Release(); };
struct Model { ~Model(); };
extern int data_ov002_0210bae4;
extern "C" void func_020072c0(void);
extern "C" void func_0207328c(void *p, int a, int b, void (*fn)(void));

struct Obj {
    void *vtable;
};

extern "C" void *func_ov002_020f6a00(Obj *self) {
    *(void**)self = &data_ov002_0210bae4;
    SharedFilePtr *sfp = *(SharedFilePtr**)((char*)self + 0x5c);
    if (sfp != 0) sfp->Release();
    ((Model*)self)->~Model();
    func_0207328c((char*)self + 0x50, 1, 0xc, func_020072c0);
    return self;
}
