//cpp
extern "C" {
extern void func_ov004_020b04d0(int v);
extern void func_ov006_0210a534(void *c);
extern int func_ov004_020ad674(void);
extern int LoadFile(int handle);
extern void DecompressLZ16(void *src, void *dst);
extern void Deallocate(void *ptr);
extern void func_ov006_020c0aa8(void *p);
extern int func_ov006_020c1a88(void *p);
extern void func_ov004_020b682c(void);
extern int func_ov004_020ad8b8(void);
extern int func_ov004_020ad878(void);
extern int data_ov006_0213d5b4[];
extern unsigned char TOP_SCREEN_RELATED;
extern unsigned char BOTTOM_SCREEN_RELATED;
extern void *func_020beb68;
}
namespace GXS { void LoadOBJPltt(void const *, unsigned int, unsigned int); }

struct Obj {
    virtual void v0();  virtual void v1();  virtual void v2();  virtual void v3();
    virtual void v4();  virtual void v5();  virtual void v6();  virtual void v7();
    virtual void v8();  virtual void v9();  virtual void v10(); virtual void v11();
    virtual void v12(); virtual void v13(); virtual void v14(); virtual void v15();
    virtual void v16(); virtual void v17(); virtual void v18(int x);
};

extern "C" int func_ov006_020f8d08(char *c)
{
    void *a;
    void *b;

    func_ov004_020b04d0(0x20);
    func_ov006_0210a534(c);
    a = (void *)LoadFile(data_ov006_0213d5b4[func_ov004_020ad674()]);
    b = (void *)LoadFile(0xbb);
    DecompressLZ16(a, (void *)0x6600000);
    GXS::LoadOBJPltt(b, 0, 0x100);
    Deallocate(a);
    Deallocate(b);

    TOP_SCREEN_RELATED = 0x11;
    BOTTOM_SCREEN_RELATED = 0x18;
    *(unsigned short *)0x4000008 = (*(unsigned short *)0x4000008 & ~3) | 1;
    *(unsigned short *)0x400100a = *(unsigned short *)0x400100a & ~3;
    func_ov006_020c0aa8(c + 0x4660);
    if (func_ov006_020c1a88(c + 0x4f38) == 0) return 0;

    func_ov004_020b682c();
    *(int *)(c + 0xa8) = func_ov004_020ad8b8();
    *(int *)(c + 0xac) = *(int *)(c + 0xa8);
    {
        int v = func_ov004_020ad878();
        if (func_020beb68) *(int *)((char *)func_020beb68 + 0xb4) = v;
    }
    ((Obj *)c)->v18(-1);
    *(short *)(c + 0x60aa) = 0;
    return 1;
}
