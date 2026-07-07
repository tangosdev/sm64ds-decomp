//cpp
extern "C" {
int _ZN3G3X6SetFogEbiii(int a, int b, int c, int d);
void InitialiseVramGlobals(void);
void func_ov004_020b04d0(int v);
void func_ov006_020c0134(void* self);
int func_020179b4(void* r0, void* r1, int r2);
void _ZN18TextureTransformer7PrepareER8BMD_FileR8BTA_File(void* bmd, void* bta);
void _ZN18TextureTransformer7SetFileER8BTA_Filei5Fix12IiEj(void* self, void* bta, int a, int b, unsigned int d);
int func_ov006_020e7fe8(char* c);
int LoadFile(int handle);
unsigned func_02054de8(void);
void DecompressLZ16(int src, void* dst);
void Deallocate(void* p);
void _ZN3GXS10LoadBGPlttEPKvjj(const void* p, unsigned int a, unsigned int b);
void func_02056374(const void* src, unsigned int offset, unsigned int count);
void _ZN3GXS11LoadOBJPlttEPKvjj(const void* p, unsigned int a, unsigned int b);
void func_ov006_020e8aac(char* c);
void func_ov004_020b0cac(int c, int a1, int a2, int a3, int arg5, short arg6);

extern unsigned char data_0209d45c;
extern unsigned char data_0209d454;
extern void* data_ov006_02141e9c;
extern void* data_ov006_0213c844;
extern void* data_ov006_02141e74;
extern int data_ov004_020bc880;
extern int data_ov004_020bc884;
}

struct M48 { int w[12]; };
extern volatile M48 data_ov006_0213c88c;

struct Obj {
    virtual void v00(); virtual void v04(); virtual void v08(); virtual void v0c();
    virtual void v10(); virtual void v14(); virtual void v18(); virtual void v1c();
    virtual void v20(); virtual void v24(); virtual void v28(); virtual void v2c();
    virtual void v30(); virtual void v34(); virtual void v38(); virtual void v3c();
    virtual void v40(); virtual void v44();
    virtual void v48(int arg);
};

extern "C" int func_ov006_020e9e70(void* arg0)
{
    char* c = (char*)arg0;
    M48 tmp;
    int zero;
    int f;
    int r5v;
    int r4v;

    data_0209d45c = 0x11;
    data_0209d454 = 0x10;
    _ZN3G3X6SetFogEbiii(0, 0, 2, 0x1000);
    InitialiseVramGlobals();

    *(volatile unsigned short*)0x4000008 = (*(volatile unsigned short*)0x4000008 & ~3) | 1;
    func_ov004_020b04d0(0x10);

    *(int*)(c + 0x4000 + 0x700) = 0;
    *(int*)(c + 0x4000 + 0x704) = 0xd0000;
    *(int*)(c + 0x4000 + 0x708) = 0x40000;
    *(int*)(c + 0x4000 + 0x70c) = 0xffed3000;
    *(int*)(c + 0x4000 + 0x710) = 0xe0000;
    *(int*)(c + 0x4000 + 0x714) = 0x40000;
    *(volatile unsigned short*)(c + 0x4700 + 0x18) = 0xc00;
    func_ov006_020c0134((void*)(c + 0x4660));

    if (func_020179b4(&data_ov006_02141e9c, (void*)(c + 0x4f38), 1) == 0) return 0;

    _ZN18TextureTransformer7PrepareER8BMD_FileR8BTA_File(*((void**)&data_ov006_02141e9c + 1), &data_ov006_0213c844);
    _ZN18TextureTransformer7SetFileER8BTA_Filei5Fix12IiEj(c + 0x51f4, &data_ov006_0213c844, 0, 0x1000, 0);

    if (func_ov006_020e7fe8((char*)(c + 0x4fd8)) == 0) return 0;

    if (func_020179b4(&data_ov006_02141e74, (void*)(c + 0x4f88), 1) == 0) return 0;

    tmp = (M48&)data_ov006_0213c88c;
    *(M48*)(c + 0x4f54) = (M48&)data_ov006_0213c88c;
    *(M48*)(c + 0x4fa4) = tmp;

    data_0209d454 |= 4;
    *(volatile unsigned short*)0x400100c &= ~3;
    *(volatile unsigned short*)0x400100c &= ~0x40;
    zero = 0;
    *(volatile unsigned int*)0x4001018 = zero;
    *(volatile unsigned short*)0x400100c = (*(volatile unsigned short*)0x400100c & 0x43) | 0x210;

    f = LoadFile(0x12);
    DecompressLZ16(f, (void*)(func_02054de8() + 0x4000));
    Deallocate((void*)f);

    f = LoadFile(0x13);
    _ZN3GXS10LoadBGPlttEPKvjj((const void*)f, 0x1e0, 0x20);
    Deallocate((void*)f);

    f = LoadFile(0x14);
    func_02056374((const void*)f, zero, 0x800);
    Deallocate((void*)f);

    r5v = LoadFile(8);
    r4v = LoadFile(9);
    DecompressLZ16(r5v, (void*)0x6600000);
    _ZN3GXS11LoadOBJPlttEPKvjj((const void*)r4v, zero, 0x100);
    Deallocate((void*)r5v);
    Deallocate((void*)r4v);

    func_ov004_020b04d0(0x20);

    ((Obj*)c)->v48(-1);

    *(unsigned short*)(c + 0x5500 + 0x48) = 0x40;

    *(int*)(c + 0xa8) = 3;
    *(int*)(c + 0xac) = *(int*)(c + 0xa8);
    *(int*)(c + 0x5000 + 0x53c) = 1;

    func_ov006_020e8aac(c);

    *(int*)(c + 0xa4) = 1;

    func_ov004_020b0cac(0xd, 0x80, 0xa8, 1, -1, 0xd);

    data_ov004_020bc880 = 0x80;
    data_ov004_020bc884 = ~0x3f;

    *(int*)(c + 0xb4) = zero;

    return 1;
}
