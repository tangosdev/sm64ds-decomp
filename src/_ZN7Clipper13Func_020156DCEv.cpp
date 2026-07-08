//cpp
typedef int Fix12i;
typedef unsigned int u32;
typedef unsigned short u16;

extern "C" {

void _ZN7Clipper13Func_0201559CEv(void* self);

void _ZN7Clipper13Func_020156DCEv(char* self, u32 a, u16 b, Fix12i c, Fix12i d) {
    *(u32*)(self + 0x4c) = a;
    *(u16*)(self + 0x58) = b;
    *(Fix12i*)(self + 0x50) = c;
    *(Fix12i*)(self + 0x54) = d;
    _ZN7Clipper13Func_0201559CEv(self);
}

}
