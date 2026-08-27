//cpp
// @symbol _ZN15dScMgHanachan_c13InitResourcesEv
/* dScMgHanachan_c::InitResources -- 0xa8/0xac are inherited from
   dScMgBase_c (0xac is inside its own pad_0ac, not a named field there
   either) so they stay raw offsets on a char* or Obj* cast of `this`. Slot 18
   stays an unmigrated raw extern "C" helper (see the class header), so
   calling it here still goes through the same local vtable-shim struct the
   pre-migration source already used, rather than a named method. */
#include "decl_common.h"
#include "dScMgHanachan_c.h"

extern "C" void *LoadFile(int handle);
extern "C" void DecompressLZ16(void *src, void *dst);
extern "C" void _ZN3GXS11LoadOBJPlttEPKvjj(const void *p, u32 a, u32 b);
extern "C" void _ZN2GX11LoadOBJPlttEPKvjj(const void *p, u32 a, u32 b);

struct Obj {
    virtual void v00(); virtual void v01(); virtual void v02(); virtual void v03();
    virtual void v04(); virtual void v05(); virtual void v06(); virtual void v07();
    virtual void v08(); virtual void v09(); virtual void v10(); virtual void v11();
    virtual void v12(); virtual void v13(); virtual void v14(); virtual void v15();
    virtual void v16(); virtual void v17(); virtual void v18(int a);
    char pad[0xa4];
    int unkA8;
    int unkAC;
};

s32 dScMgHanachan_c::InitResources()
{
    void *a;
    void *b;
    Obj *obj = (Obj *)this;

    func_ov006_020edcb0();
    a = LoadFile(0xcb);
    b = LoadFile(0xcc);
    DecompressLZ16(a, (void *)0x6400000);
    DecompressLZ16(a, (void *)0x6600000);
    _ZN3GXS11LoadOBJPlttEPKvjj(b, 0, 0xc0);
    _ZN2GX11LoadOBJPlttEPKvjj(b, 0, 0xc0);
    Deallocate(a);
    Deallocate(b);
    obj->unkA8 = data_ov006_0213c954;
    obj->unkAC = obj->unkA8;
    func_ov004_020b04d0(0x20);
    obj->v18(-1);
    *data_ov006_02137560[0] = (*data_ov006_02137560[0] & 0xc1fffcff) | 0x100;
    *data_ov006_02137560[1] = (*data_ov006_02137560[1] & 0xc1fffcff) | 0x100;
    *data_ov006_02137560[2] = (*data_ov006_02137560[2] & 0xc1fffcff) | 0x100;
    *data_ov006_02137560[3] = (*data_ov006_02137560[3] & 0xc1fffcff) | 0x100;
    *data_ov006_02137560[4] = (*data_ov006_02137560[4] & 0xc1fffcff) | 0x100;
    *data_ov006_02137560[5] = (*data_ov006_02137560[5] & 0xc1fffcff) | 0x100;
    *data_ov006_02137560[6] = (*data_ov006_02137560[6] & 0xc1fffcff) | 0x100;
    *data_ov006_02137560[7] = (*data_ov006_02137560[7] & 0xc1fffcff) | 0x100;
    *data_ov006_02137560[8] = (*data_ov006_02137560[8] & 0xc1fffcff) | 0x100;
    *data_ov006_02137560[9] = (*data_ov006_02137560[9] & 0xc1fffcff) | 0x100;
    *data_ov006_02137560[10] = (*data_ov006_02137560[10] & 0xc1fffcff) | 0x100;
    *data_ov006_02137560[11] = (*data_ov006_02137560[11] & 0xc1fffcff) | 0x100;
    return 1;
}
