//cpp
typedef int Fix12;
struct Vector3_16f;
struct Callback;
struct Animation { void Advance(); };
struct System {
    static System* New(unsigned, unsigned, Fix12, Fix12, Fix12, const Vector3_16f*, Callback*);
};
extern "C" {
extern void func_ov006_020c9024(char *o);
extern void func_ov006_020c8ecc(char *o);
extern int data_ov006_0214059c;
}
System* System::New(unsigned, unsigned, Fix12, Fix12, Fix12, const Vector3_16f*, Callback*);
void Animation::Advance();

typedef void (*PMF)(void*);
struct Closure { int off; int adj; };

extern "C" void func_ov006_020cb030(char *o) {
    *(int*)(o + 0x28) = *(int*)(o + 0x1c);
    *(int*)(o + 0x2c) = *(int*)(o + 0x20);
    *(int*)(o + 0x30) = *(int*)(o + 0x24);
    if (*(int*)(o + 0x64) != 0) {
        Closure* cl = (Closure*)(o + 0x64);
        void* tobj = o + (cl->adj >> 1);
        void (*fn)();
        if (cl->adj & 1)
            fn = *(void(**)())(*(char**)tobj + cl->off);
        else
            fn = (void(*)())cl->off;
        ((void(*)(void*))fn)(tobj);
    }
    func_ov006_020c9024(o);
    Fix12 v = (Fix12)(((long long)*(int*)(o + 0x44) * 0xb4b + 0x800) >> 12);
    if (*(int*)(o + 0x38) > v) {
        if (*(int*)(o + 0xcc) == data_ov006_0214059c) {
            *(int*)(o + 0x54) = (int)System::New(
                *(int*)(o + 0x54), 0xf5,
                *(int*)(o + 0x1c) << 3, *(int*)(o + 0x20) << 3,
                *(int*)(o + 0x24) << 3, 0, 0);
        }
    }
    func_ov006_020c8ecc(o);
    ((Animation*)(o + 0xbc))->Advance();
}
