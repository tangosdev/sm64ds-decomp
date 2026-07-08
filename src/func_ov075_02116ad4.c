typedef unsigned short u16;
typedef unsigned int u32;

extern int func_0203d9b4(void);
extern void func_0200f13c(void);
extern void *_ZN3G2S12GetBG2ScrPtrEv(void);
extern u32 LoadCompressedFileAt(int fileID, void *target);
extern int func_0200f0bc(void);
extern void DecompressLZ16(void *src, int dst);
extern void *LoadFile(int handle);
extern void _ZN2GX11LoadOBJPlttEPKvjj(const void *p, u32 a, u32 b);
extern void Deallocate(void *ptr);
extern void _ZN2GX10LoadBGPlttEPKvjj(const void *p, u32 a, u32 b);
extern int func_0203da3c(void);
extern void func_02030aa4(int arg);
extern void func_ov075_0211a194(void *p, const void *data);
extern void func_ov075_02116e00(void *p);
extern void *_ZN12ActorDerived5SpawnEjP9ActorBaseii(u32 a, void *b, int c, int d);

extern void *data_0208a0e4[];
extern int data_ov075_0211c9a4[];
extern const unsigned char data_ov075_0211d888[];
extern const unsigned char data_ov075_0211d768[];
extern const unsigned char data_ov075_0211d780[];
extern void *ROOT_ACTOR_BASE;

int func_ov075_02116ad4(void *arg) {
    void *tmp;
    int mode;

    if (*(int *)((char *)arg + 8) != 0 || func_0203d9b4() != 0) {
        func_0200f13c();
        if (func_0203d9b4() != 0) {
            LoadCompressedFileAt(0x25e, _ZN3G2S12GetBG2ScrPtrEv());
        }
        DecompressLZ16(data_0208a0e4[func_0200f0bc()], 0x6600000);
    }
    LoadCompressedFileAt(data_ov075_0211c9a4[func_0200f0bc()], (void *)0x6400000);

    tmp = LoadFile(0x8412);
    _ZN2GX11LoadOBJPlttEPKvjj(tmp, 0, 0xc0);
    Deallocate(tmp);

    tmp = LoadFile(0x980b);
    _ZN2GX10LoadBGPlttEPKvjj(tmp, 0, 0xc0);
    Deallocate(tmp);

    *(int *)((char *)arg + 0x264) = 0;
    *(unsigned char *)((char *)arg + 0x280) = 0;

    if (*(int *)((char *)arg + 8) == 0) {
        if (func_0203da3c() == 0) {
            func_02030aa4(0);
            func_ov075_0211a194(arg, data_ov075_0211d888);
        } else {
            func_ov075_0211a194(arg, data_ov075_0211d768);
        }
        func_ov075_02116e00(arg);
        mode = 0;
    } else {
        func_ov075_0211a194(arg, data_ov075_0211d780);
        mode = 2;
    }

    *(void **)((char *)arg + 0x50) = _ZN12ActorDerived5SpawnEjP9ActorBaseii(0x15a, ROOT_ACTOR_BASE, mode, 0);
    return 1;
}
