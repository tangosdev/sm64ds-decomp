//cpp
// @symbol _ZN15dScMgSnowball_c13InitResourcesEv
#include "types.h"
#include "decl_common.h"
#include "dScMgSnowball_c.h"
/* dScMgSnowball_c::InitResources -- vtable slot 0, ov006 0x02129268.
 *
 * Named from the table: 0x02129268 is the word slot 0 of
 * _ZTV15dScMgSnowball_c holds where its base's table holds something else, so
 * it is this class's own override of the virtual fBase_c declares.
 *
 * Sets up both screens' BG2/BG3 layers, loads the minigame's palettes and
 * tiles, then loads the snowball BMD into unk_abf4 and hands it to mModel --
 * which is the pointer this class's own CleanupResources (slot 3, 0x021291f8)
 * frees again. Returning 0 when ModelBase::SetFile fails is what makes the
 * scene abort.
 *
 * EVERY MANGLED CALLEE IS NOW `extern "C"`, not a bare `extern`. In a C
 * translation unit -- which this file was -- the identifier is emitted
 * verbatim; compiled as C++ a bare `extern` mangles it a SECOND time and the
 * reference resolves to nothing (see the note at the top of
 * include/SharedFilePtr.h). The names themselves are unchanged.
 *
 * The receiver stays a `void *`/`u8 *` pair the way the pre-migration file
 * spelled it: 0xb9f8 and 0xabf4 are named fields, but the three ov006 helpers
 * at the end take the scene as an opaque pointer, and only unk_abf4 is
 * reached as a member below. */

extern "C" {
void *_ZN2G213GetBG2CharPtrEv(void);
unsigned _ZN3G2S13GetBG2CharPtrEv(void);
void *_ZN2G212GetBG3ScrPtrEv(void);
void *_ZN3G2S12GetBG3ScrPtrEv(void);
u32 LoadCompressedFileAt(u16 fileID, void *target);

void *func_ov004_020adc74(void *p);
void Ov004_Deallocate(void *p);
void _ZN4CP1527FlushAndInvalidateDataCacheEjj(void *p, u32 sz);
void _ZN2GX10LoadBGPlttEPKvjj(const void *p, u32 a, u32 b);
void _ZN3GXS10LoadBGPlttEPKvjj(const void *p, u32 a, u32 b);
void DecompressLZ16(void *src, void *dst);
void _ZN2GX11LoadOBJPlttEPKvjj(const void *p, u32 a, u32 b);
void _ZN3GXS11LoadOBJPlttEPKvjj(const void *p, u32 a, u32 b);
void _ZN5Model17UpdateFileOffsetsER8BMD_File(void *file);
int _ZN9ModelBase7SetFileEP8BMD_Fileii(void *thisPtr, void *file, int a, int b);
void func_ov006_02126a98(void *p);

extern int data_0208ee44;
extern u8 data_0209d45c;
extern u8 data_0209d454;
}

s32 dScMgSnowball_c::InitResources()
{
    void *arg0 = this;
    u8 *r4 = (u8 *)arg0;
    void *buf;

    data_0208ee44 = 1;
    *(vu16 *)0x4000304 &= ~0x8000;
    *(r4 + 0xb9f8) = 1;

    *(vu16 *)0x400000c = (*(vu16 *)0x400000c & 0x43) | 0x5214;
    *(vu16 *)0x400000c &= ~0x40;
    *(vu32 *)0x4000018 = 0;
    *(vu16 *)0x400000c = (*(vu16 *)0x400000c & ~3) | 2;

    *(vu16 *)0x400100c = (*(vu16 *)0x400100c & 0x43) | 0x5214;
    *(vu16 *)0x400100c &= ~0x40;
    *(vu32 *)0x4001018 = 0;
    *(vu16 *)0x400100c = (*(vu16 *)0x400100c & ~3) | 2;

    *(vu16 *)0x400000e = (*(vu16 *)0x400000e & 0x43) | 0x1614;
    *(vu16 *)0x400000e &= ~0x40;
    *(vu32 *)0x400001c = 0;
    *(vu16 *)0x400000e = (*(vu16 *)0x400000e & ~3) | 3;

    *(vu16 *)0x400100e = (*(vu16 *)0x400100e & 0x43) | 0x1614;
    *(vu16 *)0x400100e &= ~0x40;
    *(vu32 *)0x400101c = 0;
    *(vu16 *)0x400100e = (*(vu16 *)0x400100e & ~3) | 3;

    LoadCompressedFileAt(0xad, (u8 *)_ZN2G213GetBG2CharPtrEv() + 0x4000);
    LoadCompressedFileAt(0xad, (void *)(_ZN3G2S13GetBG2CharPtrEv() + 0x4000));
    LoadCompressedFileAt(0xac, _ZN2G212GetBG3ScrPtrEv());
    LoadCompressedFileAt(0xac, _ZN3G2S12GetBG3ScrPtrEv());

    data_0209d45c |= 0xc;
    data_0209d454 |= 0xc;

    buf = func_ov004_020adc74(&data_ov006_0214009c);
    _ZN4CP1527FlushAndInvalidateDataCacheEjj(buf, 0x100);
    _ZN2GX10LoadBGPlttEPKvjj(buf, 0x100, 0x100);
    _ZN3GXS10LoadBGPlttEPKvjj(buf, 0x100, 0x100);
    Ov004_Deallocate(buf);

    buf = func_ov004_020adc74(&data_ov006_021400bc);
    DecompressLZ16(buf, (void *)0x6400000);
    DecompressLZ16(buf, (void *)0x6600000);
    Ov004_Deallocate(buf);

    buf = func_ov004_020adc74(&data_ov006_021400dc);
    _ZN4CP1527FlushAndInvalidateDataCacheEjj(buf, 0x100);
    _ZN2GX11LoadOBJPlttEPKvjj(buf, 0, 0x100);
    _ZN3GXS11LoadOBJPlttEPKvjj(buf, 0, 0x100);
    Ov004_Deallocate(buf);

    *(vu16 *)0x4000008 = (*(vu16 *)0x4000008 & ~3) | 1;
    _ZN3G3X6SetFogEbiii(0, 0, 2, 0x1000);
    *(vu16 *)0x4000060 = (*(vu16 *)0x4000060 & ~0x3000) | 8;
    InitialiseVramGlobals();

    *(void **)(r4 + 0xabf4) = func_ov004_020adc74(&data_ov006_021400fc);
    _ZN5Model17UpdateFileOffsetsER8BMD_File(*(void **)(r4 + 0xabf4));

    if (_ZN9ModelBase7SetFileEP8BMD_Fileii(r4 + 0xaba4, *(void **)(r4 + 0xabf4), 1, -1) == 0)
        return 0;

    *(vu16 *)0x4000008 = (*(vu16 *)0x4000008 & ~3) | 1;
    data_0209d45c |= 1;

    func_ov006_021279b0(arg0);
    func_ov006_02126ee4(arg0);
    func_ov006_02126a98(arg0);
    return 1;
}
