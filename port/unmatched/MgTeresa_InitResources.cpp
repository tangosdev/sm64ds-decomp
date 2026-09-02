// PORT_HOST_ABI. dScMgTeresa_c::InitResources, vtable slot 0, host-copied.
// Run mg9, lane BOO. actor id 0x183 = scene 387, "Hide and Boo Seek".
//
// ---- THIS IS NOT WALL WORK, IT IS A COMPILE DEFECT -----------------------
//
// src/func_ov006_021203fc.c is a MATCHED body and nothing about it is a
// pointer-to-member problem. MSVC refuses one statement in it:
//
//     src\func_ov006_021203fc.c(69): error C2036: 'void *': unknown size
//
// which is `f2 += 0;` on a `void *f2`. The ROM contains no such operation --
// the instructions either side of that point are the LoadFile(0x102) return
// and the call to func_ov004_020af2f8 -- so the statement is a codegen nudge
// mwcc needed and the host does not.
//
// WHY NOT hostgen. port/tools/hostgen.py exists for exactly this class of
// problem and its VOIDPP_ARITH rewrite is the closest rule it has, but that
// rule retypes a `*(void**)(x) + n` DEREF and does not cover a `void*`
// compound assignment. Widening a generator that six other gates route their
// sources through, from inside a fan-out lane, would change files no reviewer
// of this lane is reading. The lane-local remedy is the precedented one:
// port/mg_fanout_costs.txt section 13 records three host copies for
// dScMgMemory2_c that were "not wall work" either, each found by a different
// instrument. This one was found by the closure probe's compiler.
//
// WHAT DIVERGES FROM src/, EXACTLY: the `f2 += 0;` statement is dropped. That
// is the whole of it.
//
// AND ONE src-SIDE NAMING DEFECT IS FIXED HERE RATHER THAN CARRIED. The src
// writes its last two words through `func_020bc880` and `func_020bc884`,
// declared in include/decl_common.h as `extern int`. NO CONFIG DEFINES EITHER
// NAME. The ROM's own literal pool at 0x021207a0 and 0x021207a4 holds
// 0x020bc880 and 0x020bc884, which config/arm9/overlays/ov004/symbols.txt
// names data_ov004_020bc880 and data_ov004_020bc884 -- ov004 DATA, not arm9
// functions. The addresses are right and only the spelling is wrong, so this
// copy writes the mount's own symbols and no alias is involved. The ov004
// mount's initial values are {128,0,0,0} and {96,0,0,0}, i.e. 0x80 and 0x60,
// which is why the first of the two writes is a no-op on frame 0 and the
// second is not. src/func_ov006_021203fc.c is the only file in the whole
// decomp that uses either name; a decomp-side rename is routed, not taken.
//
// ---- THE TRANSCRIPTION IS CHECKED AGAINST THE ROM ------------------------
//
// The body is 0x3ac bytes = 235 words = 217 instructions + 18 literal-pool
// words, and every pool word was read out of
// extracted/overlays/overlay_0006.bin at base 0x020bfec0:
//
//   0x02120760 0400000c   0x0212076c 0209d45c   0x02120778 00009310
//   0x02120764 04000018   0x02120770 0400000e   0x0212077c 00000101
//   0x02120768 00001210   0x02120774 0400001c   0x02120780 00000102
//   0x02120784 04001008   0x02120790 0209d454   0x0212079c 00000408
//   0x02120788 04001010   0x02120794 0400100c   0x021207a0 020bc880
//   0x0212078c 00001111   0x02120798 04001018   0x021207a4 020bc884
//
// so every register address, every mask and every file id below is the ROM's
// own word rather than a reading of the src. The file ids arrive in ROM order
// -- 0x9b, 0x9c, 0x9e, 0x9d, 0x101, 0x102, 0x9f, 0xa0, 0xa1 -- and the two
// DecompressLZ16 destinations 0x6400000 and 0x6600000 are immediates
// (mov r1,#0x6400000 at 0x0212054c, mov r1,#0x6600000 at 0x021206a0).
//
// THE THREE VOLATILE u16 LOCALS ARE LOAD-BEARING AND ARE KEPT. The ROM stores
// each MultiStore16 fill value to the stack and loads it straight back
// (strh r1,[sp,#8] / ldrh r0,[sp,#8] at 0x0212044c, and the same pair at
// sp+0xa and sp+0xc), which is what a `volatile u16` local compiles to. They
// are not decoration and dropping them would change the frame.
//
// This body carries the "recovered from vtable slot identity" marker and was
// ruled REAL_DECOMP against the ROM before it was seated; the ruling is in
// port/tools/inferred_stub_adjudicated.txt and the arithmetic is in
// port/slice_boo.txt.

typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;

extern "C" {

void *_ZN2G212GetBG2ScrPtrEv(void);
void *_ZN2G213GetBG2CharPtrEv(void);
void *_ZN3G2S12GetBG0ScrPtrEv(void);
void *_ZN3G2S13GetBG0CharPtrEv(void);
unsigned _ZN3G2S13GetBG2CharPtrEv(void);
void MultiStore16(u16 val, void *dst, int nbytes);
void *LoadFile(int handle);
void DecompressLZ16(void *src, void *dst);
void Deallocate(void *ptr);
void _ZN2GX10LoadBGPlttEPKvjj(const void *p, u32 a, u32 b);
void _ZN2GX11LoadOBJPlttEPKvjj(const void *p, u32 a, u32 b);
void _ZN3GXS10LoadBGPlttEPKvjj(const void *p, u32 a, u32 b);
void _ZN3GXS11LoadOBJPlttEPKvjj(const void *p, u32 a, u32 b);
void func_02056314(void *src, int a, int b);
void func_02056374(void *src, int a, int b);
void func_ov004_020af2f8(void *self, int a, int b, int c);
void func_ov004_020b04d0(int a);
void func_ov004_020b0cac(int c, int a1, int a2, int a3, int arg5, short arg6);
void func_ov006_0211fbf8(char *self);
void func_ov006_0211d7b4(char *self);
void func_ov006_0211dd6c(char *self);
void func_ov006_0211f77c(char *self);

extern u8 data_0209d45c;
extern u8 data_0209d454;
/* ov004 DATA, and the two words the src misnames func_020bc880 / _884. */
extern u8 data_ov004_020bc880[];
extern u8 data_ov004_020bc884[];

int func_ov006_021203fc(char *self);

}  /* extern "C" */

// PORT_HOST_ABI: slot 0 InitResources host copy; src carries a mwcc codegen-nudge f2 += 0 on a void* that MSVC rejects as C2036 and the ROM never performs, so the host drops that one no-op statement.
extern "C" int func_ov006_021203fc(char *self)
{
    void *f1;
    void *f2;
    void *t;
    volatile u16 fa;
    volatile u16 fb;
    volatile u16 fc;

    *((volatile u16 *) 0x400000c) &= ~3;
    *((volatile u16 *) 0x400000c) &= ~0x40;
    *((volatile u32 *) 0x4000018) = 0;
    *((volatile u16 *) 0x400000c) =
        ((*((volatile u16 *) 0x400000c)) & 0x43) | 0x1210;
    {
        void *p = _ZN2G212GetBG2ScrPtrEv();
        fa = 0x5300;
        MultiStore16(fa, p, 0x800);
    }
    data_0209d45c |= 8;
    *((volatile u16 *) 0x400000e) = (*((volatile u16 *) 0x400000e)) & (~3);
    *((volatile u16 *) 0x400000e) &= ~0x40;
    *((volatile u32 *) 0x400001c) = 0;
    *((volatile u16 *) 0x400000e) =
        ((*((volatile u16 *) 0x400000e)) & 0x43) | 0x9310;
    t = LoadFile(0x9b);
    DecompressLZ16(t, _ZN2G213GetBG2CharPtrEv());
    Deallocate(t);
    t = LoadFile(0x9c);
    _ZN2GX10LoadBGPlttEPKvjj(t, 0x60, 0x1a0);
    Deallocate(t);
    t = LoadFile(0x9e);
    func_02056314(t, 0, 0x800);
    Deallocate(t);
    t = LoadFile(0x9d);
    func_02056314(t, 0x800, 0x800);
    Deallocate(t);
    f1 = LoadFile(0x101);
    f2 = LoadFile(0x102);
    DecompressLZ16(f1, (void *) 0x6400000);
    _ZN2GX11LoadOBJPlttEPKvjj(f2, 0, 0x100);
    *((volatile u16 *) 0x4001008) =
        ((*((volatile u16 *) 0x4001008)) & 0x43) | 0x218;
    *((volatile u16 *) 0x4001008) &= ~0x40;
    *((volatile u32 *) 0x4001010) = 0;
    *((volatile u16 *) 0x4001008) &= ~3;
    {
        void *p = _ZN3G2S12GetBG0ScrPtrEv();
        fb = 0;
        MultiStore16(fb, p, 0x800);
    }
    /* src/func_ov006_021203fc.c has `f2 += 0;` here. THE ROM HAS NO SUCH
       OPERATION and MSVC rejects it as C2036; see this file's header. */
    func_ov004_020af2f8(self, 0, 0, 0);
    {
        void *p = _ZN3G2S13GetBG0CharPtrEv();
        fc = 0x1111;
        MultiStore16(fc, p, 0x6000);
    }
    data_0209d454 |= 4;
    *((volatile u16 *) 0x400100c) =
        ((*((volatile u16 *) 0x400100c)) & (~3)) | 1;
    *((volatile u16 *) 0x400100c) &= ~0x40;
    *((volatile u32 *) 0x4001018) = 0;
    *((volatile u16 *) 0x400100c) =
        ((*((volatile u16 *) 0x400100c)) & 0x43) | 0x408;
    t = LoadFile(0x9f);
    DecompressLZ16(t, (void *) _ZN3G2S13GetBG2CharPtrEv());
    Deallocate(t);
    t = LoadFile(0xa0);
    _ZN3GXS10LoadBGPlttEPKvjj(t, 0x60, 0x1a0);
    Deallocate(t);
    t = LoadFile(0xa1);
    func_02056374(t, 0, 0x800);
    Deallocate(t);
    DecompressLZ16(f1, (void *) 0x6600000);
    _ZN3GXS11LoadOBJPlttEPKvjj(f2, 0, 0x100);
    Deallocate(f1);
    Deallocate(f2);
    func_ov004_020b04d0(0x20);
    func_ov006_0211fbf8(self);
    *((u8 *) (self + 0x4c23)) = 0xff;
    func_ov006_0211d7b4(self);
    func_ov006_0211dd6c(self);
    func_ov006_0211f77c(self);
    *((int *) (self + 0x4be8)) = 1;
    *((u16 *) (self + 0x4c16)) = 0x20;
    func_ov004_020b0cac(0xd, 0x80, 0xa8, 1, -1, 0xd);
    *((int *) data_ov004_020bc880) = 0x80;
    *((int *) data_ov004_020bc884) = -128;
    *((int *) (self + 0xb4)) = 0;
    return 1;
}
