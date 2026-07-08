//cpp
typedef unsigned int u32;
typedef int s32;
extern "C" {
extern unsigned char TOP_SCREEN_RELATED[];
extern int data_ov006_0213fe78[];
extern int GAME_SPEED_RELATED[];
extern void func_ov006_0210a534(void);
extern s32 func_ov004_020ad674(void);
extern u32 LoadCompressedFileAt(int fileID, void *target);
extern int LoadFile(int handle);
extern void Deallocate(void *ptr);
extern void func_ov006_020c0aa8(void *p);
extern int func_ov006_020c1a88(void *p);
extern void func_ov004_020b04d0(int v);
extern void func_ov004_020b682c(void);
extern int func_ov004_020ad8b8(void);
int func_ov006_0212551c(void *self);
}
extern "C" void _ZN3GXS11LoadOBJPlttEPKvjj(void const *a, unsigned int b, unsigned int c);
struct Obj {
    virtual void v0();virtual void v1();virtual void v2();virtual void v3();
    virtual void v4();virtual void v5();virtual void v6();virtual void v7();
    virtual void v8();virtual void v9();virtual void v10();virtual void v11();
    virtual void v12();virtual void v13();virtual void v14();virtual void v15();
    virtual void v16();virtual void v17();virtual void v18(int x);
    char pad[0x10000];
};
int func_ov006_0212551c(void *self) {
    Obj *o = (Obj *)self;
    char *c = (char *)self;
    int fh;
    TOP_SCREEN_RELATED[0] = 0x11;
    func_ov006_0210a534();
    LoadCompressedFileAt(data_ov006_0213fe78[func_ov004_020ad674()], (void *)0x6600000);
    fh = LoadFile(0xb3);
    _ZN3GXS11LoadOBJPlttEPKvjj((void *)fh, 0, 0x100);
    Deallocate((void *)fh);
    func_ov006_020c0aa8(c + 0x4660);
    if (func_ov006_020c1a88(c + 0x4f38) == 0)
        return 0;
    func_ov004_020b04d0(0x20);
    func_ov004_020b682c();
    GAME_SPEED_RELATED[0] = 1;
    *(int *)(c + 0xa8) = func_ov004_020ad8b8();
    *(int *)(c + 0xac) = *(int *)(c + 0xa8);
    o->v18(3);
    return 1;
}
