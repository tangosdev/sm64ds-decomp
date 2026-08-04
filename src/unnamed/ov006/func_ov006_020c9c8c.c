//cpp
typedef int Fix12;
struct BCA_File;
struct ModelAnim {
    void SetAnim(BCA_File *f, int a, Fix12 b, unsigned int c);
};
struct Pair { int a, b; };

struct Obj {
    virtual void v0();
    virtual void v1();
    virtual void v2();
    virtual void v3();
    virtual void slot4();
};

extern "C" {
extern int data_ov006_021405b0;
extern int data_ov006_02140574;
extern BCA_File *data_ov006_0214059c;
extern Pair data_ov006_0213b194;
extern void func_ov006_020e6e3c(int a, int b);
extern void func_ov006_020c91ac(char *c);
}

extern "C" void func_ov006_020c9c8c(char *c)
{
    Obj *o = (Obj *)c;
    int a, b;
    *(int *)(c + 0x48) = data_ov006_021405b0;
    *(int *)(c + 0x40) = (int)(((long long)data_ov006_02140574 * 0xc00 + 0x800) >> 12);
    *(int *)(c + 0x4c) = *(int *)(c + 0x40);
    o->slot4();
    *(int *)(c + 0x64) = 0;
    *(int *)(c + 0x60) = 0;
    ((ModelAnim *)(c + 0x78))->SetAnim(data_ov006_0214059c, 0x40000000, 0x800, 0);
    *(int *)(c + 0xd0) = 0;
    func_ov006_020e6e3c(0x110, *(int *)(c + 0x24));
    func_ov006_020e6e3c(0x1b5, *(int *)(c + 0x24));
    *(int *)(c + 0x68) = 0;
    a = data_ov006_0213b194.a;
    b = data_ov006_0213b194.b;
    *(int *)(c + 0x70) = b ? a : a;
    *(int *)(c + 0x74) = b;
    func_ov006_020c91ac(c);
}
