typedef unsigned int u32;
typedef unsigned short u16;
typedef unsigned char u8;

extern int LoadFile(int handle);
extern void _ZN2GX11LoadOBJPlttEPKvjj(const void *p, u32 a, u32 b);
extern unsigned func_02054de8(void);
extern void Deallocate(void *ptr);
extern void _ZN3GXS10LoadBGPlttEPKvjj(const void *p, u32 a, u32 b);
extern void func_02056374(const void *src, u32 offset, u32 count);
extern void _ZN3GXS11LoadOBJPlttEPKvjj(const void *p, u32 a, u32 b);
extern void DecompressLZ16(int src, void *dst);
extern void func_ov006_020c225c(void *p);
extern int func_ov006_020c3050(void *p);
extern void func_ov006_0211c478(void *p);
extern void func_ov006_0211c080(void *p);
extern void func_ov004_020b6808(void);
extern void func_ov004_020b0cac(int c, int a1, int a2, int a3, int arg5, short arg6);

extern u8 TOP_SCREEN_RELATED;
extern u8 BOTTOM_SCREEN_RELATED;

int func_ov006_0211c984(void *arg0)
{
    u8 *r7 = (u8 *)arg0;
    int r6, r5, r4;

    TOP_SCREEN_RELATED |= 0x11;
    r6 = LoadFile(0xff);
    r5 = LoadFile(0x100);
    DecompressLZ16(r6, (void *)0x6400000);
    _ZN2GX11LoadOBJPlttEPKvjj((const void *)r5, 0, 0x100);

    BOTTOM_SCREEN_RELATED |= 4;
    r4 = LoadFile(0x98);
    *(volatile u16 *)0x400100c = (*(volatile u16 *)0x400100c & 0x43) | 0x214;
    DecompressLZ16(r4, (void *)func_02054de8());
    Deallocate((void *)r4);

    r4 = LoadFile(0x99);
    _ZN3GXS10LoadBGPlttEPKvjj((const void *)r4, 0x60, 0x1a0);
    Deallocate((void *)r4);

    r4 = LoadFile(0x9a);
    func_02056374((const void *)r4, 0, 0x800);
    Deallocate((void *)r4);

    *(volatile u16 *)0x400100c &= ~0x40;
    *(volatile u32 *)0x4001018 = 0;
    *(volatile u16 *)0x400100c = (*(volatile u16 *)0x400100c & ~3) | 1;
    DecompressLZ16(r6, (void *)0x6600000);
    _ZN3GXS11LoadOBJPlttEPKvjj((const void *)r5, 0, 0x100);
    Deallocate((void *)r6);
    Deallocate((void *)r5);

    func_ov006_020c225c((void *)(r7 + 0x4660));
    if (func_ov006_020c3050((void *)(r7 + 0x4f38)) == 0)
        return 0;

    *(int *)(r7 + 0x5000 + 0xe0) = 1;
    func_ov006_0211c478(r7);
    func_ov006_0211c080(r7);
    *(u8 *)(r7 + 0x5000 + 0x626) = 3;
    *(int *)(r7 + 0x5000 + 0x608) = 1;
    *(u16 *)(r7 + 0x5600 + 0x18) = 0x20;
    func_ov004_020b6808();
    func_ov004_020b0cac(0xd, 0x80, 0xa8, 1, -1, 0xd);
    *(int *)(r7 + 0xb4) = 0;
    return 1;
}
