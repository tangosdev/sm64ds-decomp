//cpp
extern "C" {
struct Vector3 { int x, y, z; };
extern void _ZN5Sound9PlayBank0EjRK7Vector3(unsigned int n, const Vector3& v);
extern void _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(void* thiz, void* f, int a, int b, unsigned int e);
extern void _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(unsigned int n, int a, int b, int c);
extern int data_ov077_02127b28[];
}

struct Base {
    virtual void v0();
    virtual void v1();
    virtual void v2();
    virtual void v3();
    virtual void v4();
    virtual void v5();
    virtual void v6();
    virtual void v7();
    virtual void v8();
    virtual void v9();
    virtual void v10();
    virtual void v11();
    virtual void v12();
    virtual void v13();
    virtual void v14();
    virtual void v15();
    virtual void v16();
    virtual void v17();
    virtual void v18();
    virtual void v19();
    virtual void v20();
    virtual void v21();
    virtual void v22();
    virtual void v23();
    virtual void v24();
    virtual void v25();
    virtual void v26();
    virtual void v27();
    virtual void v28();
    virtual int m();   // slot 0x74/4 = 29
};

extern "C" int func_ov077_02124038(char* c)
{
    _ZN5Sound9PlayBank0EjRK7Vector3(9, *(Vector3*)(c + 0x74));
    *(int*)(c + 0x9c) = -0x2000;
    *(int*)(c + 0xa0) = -0x3c000;
    *(int*)(c + 0x98) = 0xa000;
    *(int*)(c + 0xa8) = 0x28000;
    *(unsigned char*)(c + 0x41c) = 0x2d;
    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(c + 0xd4, *(void**)((char*)data_ov077_02127b28 + 4), 0, 0x1000, 0);
    *(int*)(c + 0x130) = 0x4000;
    Base* b = (Base*)c;
    int r1 = b->m();
    _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(0x43, *(int*)(c + 0x5c), *(int*)(c + 0x60) + r1, *(int*)(c + 0x64));
    int r2 = b->m();
    _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(0x44, *(int*)(c + 0x5c), *(int*)(c + 0x60) + r2, *(int*)(c + 0x64));
    *(int*)(c + 0x3f4) = 4;
    return 1;
}
