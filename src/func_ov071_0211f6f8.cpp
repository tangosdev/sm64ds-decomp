//cpp
extern "C" {
struct Vector3 { int x, y, z; };
extern void _ZN5Sound9PlayBank0EjRK7Vector3(unsigned int n, const Vector3& v);
extern void _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(void* thiz, void* f, int a, int b, unsigned int e);
extern void _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(unsigned int n, int a, int b, int c);
extern int data_ov071_02122f88[];
}

struct Base {
    virtual void v0();  virtual void v1();  virtual void v2();  virtual void v3();
    virtual void v4();  virtual void v5();  virtual void v6();  virtual void v7();
    virtual void v8();  virtual void v9();  virtual void v10(); virtual void v11();
    virtual void v12(); virtual void v13(); virtual void v14(); virtual void v15();
    virtual void v16(); virtual void v17(); virtual void v18(); virtual void v19();
    virtual void v20(); virtual void v21(); virtual void v22(); virtual void v23();
    virtual void v24(); virtual void v25(); virtual void v26(); virtual void v27();
    virtual void v28(); virtual int m();   // slot 0x74/4 = 29
};

extern "C" int func_ov071_0211f6f8(char* c)
{
    _ZN5Sound9PlayBank0EjRK7Vector3(9, *(Vector3*)(c + 0x74));
    *(int*)(((int)c + 0xb0) & 0xFFFFFFFFFFFFFFFF) &= ~1;
    *(int*)(c + 0x98) = 0xa000;
    *(int*)(c + 0xa8) = 0x28000;
    *(short*)(c + 0x3a8) = 0x2d;
    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(c + 0xd4, *(void**)((char*)data_ov071_02122f88 + 4), 0, 0x1000, 0);
    *(int*)(c + 0x130) = 0x4000;
    Base* b = (Base*)c;
    int r1 = b->m();
    _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(0x43, *(int*)(c + 0x5c), *(int*)(c + 0x60) + r1, *(int*)(c + 0x64));
    int r2 = b->m();
    _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(0x44, *(int*)(c + 0x5c), *(int*)(c + 0x60) + r2, *(int*)(c + 0x64));
    *(int*)(c + 0x39c) = 8;
    return 1;
}
