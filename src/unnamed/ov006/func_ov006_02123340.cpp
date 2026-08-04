//cpp
struct Vector3_16f;
struct Fix12 { int v; };

struct Particle { };
namespace Particle2 {}

extern "C" void func_ov006_02120c40(void);
extern "C" void func_ov006_020eef90(void);
extern "C" void func_ov006_02122ab8(void);
extern "C" void func_ov006_02123938(void* self);
extern "C" void func_ov004_020adb1c(int self);

struct PSys {
    static void* NewUnkCallback818(unsigned int a, unsigned int b, int c, int d, int e, void* f);
    static void* FromUniqueID(unsigned int id);
};

extern void ApproachLinear(int& x, int a, int b);

extern int data_ov006_02140830;

struct Obj;
typedef void (Obj::*PMF)();

struct Obj {
    char pad[0x5004];
    PMF cb;     /* 0x5004 */
};

extern "C" int func_ov006_02123340(Obj* self)
{
    int saved = data_ov006_02140830;
    func_ov006_02120c40();
    func_ov006_020eef90();
    func_ov006_02122ab8();
    char* c = (char*)self;
    *(void**)(c + 0x7ac4) = PSys::NewUnkCallback818(
        *(unsigned int*)(c + 0x7ac4), 0xf0, 0x280000, 0x700000, -0x580000, 0);
    void* p = PSys::FromUniqueID(*(unsigned int*)(c + 0x7ac4));
    if (p != 0) {
        *(char*)((char*)p + 0x58) = (char)(*(int*)(c + 0x7ac8) >> 12);
        ApproachLinear(*(int*)(c + 0x7ac8), 0x14000, 0x200);
    }
    (self->*(self->cb))();
    func_ov006_02123938(self);
    if (saved != data_ov006_02140830) {
        func_ov004_020adb1c(data_ov006_02140830);
    }
    return 1;
}
