//cpp
typedef struct S48 { int w[12]; } S48;

struct Obj {
    virtual void v0();
    virtual void v1();
    virtual void v2();
    virtual void v3();
    virtual void m4(int);
};

extern "C" {
void func_020167a4(void* p);
void func_ov075_02114b60(void* p);
void func_0204531c(void* self, int arg);
void _ZN5Model6RenderEPK7Vector3(void* self, void* v);
void _ZN15TextureSequence6UpdateER15ModelComponents(void* self, void* mc);
}

#define LAUNDER(x) ((char*)((long long)(int)(x)))

extern "C" void func_ov075_02114be4(char* c) {
    char *h, *comp;

    func_020167a4(c);
    func_ov075_02114b60(c);
    func_0204531c(c + 8, *(int*)(c + 0x64));

    {
        char* p = LAUNDER(c + 8);
        unsigned int i;
        h = *(char**)p;
        comp = *(char**)(p + 4);
        for (i = 0; i < *(unsigned int*)(h + 0x24); i++) {
            *(int*)(comp + 0x20) = *(int*)(c + 0x148);
            comp += 0x30;
        }
        _ZN5Model6RenderEPK7Vector3(c, 0);
        ((Obj*)(c + 0x70))->m4(*(int*)(p + 0xc) + 0x2d0);
    }

    {
        S48* src = (S48*)LAUNDER(c + 0x1c);
        *(S48*)(c + 0x8c) = *src;
    }

    {
        char* q = LAUNDER(c + 0x78);
        unsigned int i;
        h = *(char**)q;
        comp = *(char**)(q + 4);
        for (i = 0; i < *(unsigned int*)(h + 0x24); i++) {
            *(int*)(comp + 0x20) = *(int*)(c + 0x148);
            comp += 0x30;
        }
    }

    _ZN5Model6RenderEPK7Vector3(c + 0x70, 0);
    _ZN15TextureSequence6UpdateER15ModelComponents(c + 0xd4, c + 0x78);
}
