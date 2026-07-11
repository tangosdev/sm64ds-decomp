typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;

extern void func_ov002_020be3b0(char* c);
extern u32 _ZNK6Player14GetBodyModelIDEjb(char* c, u32 a, char b);
extern void _ZN10ModelAnim24CopyERKS_Pcj(void* self, void* src, char* p, u32 n);
extern void func_ov002_020e444c(char* c);
extern int func_ov002_020e4990(char* c);
extern void func_ov002_020e4768(char* c);
extern u32 _ZN6Player8HasNoCapEv(char* c);

extern void* data_ov002_020ff480[];

void func_ov002_020e4bb8(char* self)
{
    u8 save_6fd;
    u8 save_6f9;
    u8 save_6fb;
    u8 save_6ff;

    func_ov002_020be3b0(self);

    save_6fd = *(u8*)(self + 0x6fd);
    save_6f9 = *(u8*)(self + 0x6f9);
    save_6fb = *(u8*)(self + 0x6fb);
    save_6ff = *(u8*)(self + 0x6ff);

    *(u8*)(self + 0x702) = 0;
    if (*(u16*)(self + 0x6ae) < 0x3f) {
        *(u8*)(self + 0x702) = (u8)(*(u16*)(self + 0x6ae) & 1);
        if (*(u8*)(self + 0x702) == 0) {
            if (*(u8*)(self + 0x6ff) == 1 || *(u8*)(self + 0x6fb) != 0) {
                *(u8*)(self + 0x6fb) = 0;
                *(u8*)(self + 0x6ff) = *(u8*)(self + 0x6fb);
            }
            if (*(u8*)(self + 0x6f9) != 0) {
                u32 v;
                u32 id;
                void* dst;
                void* src;
                void* poolEntry;
                char* p;

                *(u8*)(self + 0x6f9) = 0;
                v = *(u32*)(self + 8);
                id = _ZNK6Player14GetBodyModelIDEjb(self, (u8)v, 0);
                dst = *(void**)(self + id * 4 + 0xdc);
                src = *(void**)(self + 0xec);
                poolEntry = data_ov002_020ff480[*(u32*)(self + 0x63c) + v];
                p = *(char**)((char*)poolEntry + 4);
                _ZN10ModelAnim24CopyERKS_Pcj(dst, src, p, 0);
            }
        }
    }

    if (*(u16*)(self + 0x6c0) < 0x3f) {
        if (*(u8*)(self + 0x6fd) != 0) {
            *(u8*)(self + 0x6fd) = (u8)(*(u16*)(self + 0x6c0) & 1);
            *(u8*)(self + 0x717) = 0;
            if (*(u8*)(self + 0x6fd) == 0) {
                *(u8*)(self + 0x6f5) = 0;
            } else {
                *(u8*)(self + 0x6f5) = 0x1f;
            }
        }
    }

    func_ov002_020e444c(self);
    func_ov002_020e4990(self);
    func_ov002_020e4768(self);

    *(u8*)(self + 0x6db) = (u8)*(u32*)(self + 8);
    *(u8*)(self + 0x6fe) = *(u8*)(self + 0x6fd);
    *(u8*)(self + 0x6fa) = *(u8*)(self + 0x6f9);
    *(u8*)(self + 0x6fc) = *(u8*)(self + 0x6fb);
    *(u8*)(self + 0x700) = *(u8*)(self + 0x6ff);
    *(u8*)(self + 0x701) = (u8)_ZN6Player8HasNoCapEv(self);

    *(u8*)(self + 0x6fd) = save_6fd;
    *(u8*)(self + 0x6f9) = save_6f9;
    *(u8*)(self + 0x6fb) = save_6fb;
    *(u8*)(self + 0x6ff) = save_6ff;
}
