/* HOST COPY of src/_ZN10MrBlizzard8BehaviorEv.cpp -- the SAME PMF-on-an-
 * incomplete-class seam port/unmatched/MrBlizzard_StateDispatch.cpp fixes
 * for func_ov081_02125488, applied to MrBlizzard's own Behavior tick
 * dispatch.
 *
 * THE PMF CALL. The matched src reads `PMF* p = pp + 1; (this->**p)();` --
 * `pp` points at one of the ten state cells (a PortMrBlizzardPair:
 * {enter_fn, enter_delta, tick_fn, tick_delta}); `pp + 1` (PMF-width, 8
 * bytes) lands on the cell's SECOND half, i.e. tick_fn/tick_delta. MSVC's
 * PMF representation for the matched src's deliberately-incomplete `struct
 * C` is not a plain function pointer, so this dispatch does not reproduce
 * the ROM's ABI on the host (see MrBlizzard_StateDispatch.cpp's header for
 * the full derivation) -- confirmed by crash: id 0xdf (MR_BLIZZARD) faults
 * at a near-null address the very first tick after gate 192 boots. This
 * host copy is the matched src translated field for field with that one
 * dispatch replaced by a plain function-pointer call through the seat's
 * own struct layout (hal/actor_classes_ov081.cpp's PortMrBlizzardPair).
 * The `pp+8 != 0` guard (matched src's `*(void**)((char*)pp + 8)`) becomes
 * `cell->tick_fn != 0` -- the identical byte offset, read through the real
 * type instead of a raw pointer.
 */
#include <cstddef>   /* size_t (pointer-width casts) */
#include "decl_SaveData.h"
#include "decl_common.h"
#include "MrBlizzard.h"
extern "C" {
extern int _ZN5Enemy26UpdateKillByInvincibleCharER12WithMeshClsnR9ModelAnimj(void* self, void* wm, void* anim, unsigned n);
extern int _ZN5Enemy11UpdateDeathER12WithMeshClsn(void* self, void* wm);
extern int _ZN5Actor5SpawnEjjRK7Vector3PK10Vector3_16as(unsigned a, unsigned b, const Vector3* v, const void* p, int e, int f);
extern void func_02012694(int a, void* p);
extern void _ZN5Actor9SetRangesE5Fix12IiES1_S1_S1_(void* self, int a, int b, int c, int d);
extern void* _ZN5Actor13ClosestPlayerEv(void* self);
extern unsigned short DecIfAbove0_Short(unsigned short* p);
extern void _ZN5Actor9UpdatePosEP12CylinderClsn(void* self, void* cyl);
extern void _ZN5Enemy12UpdateWMClsnER12WithMeshClsnj(void* self, void* wm, unsigned n);
extern void _ZN12CylinderClsn5ClearEv(void* self);
extern void _ZN12CylinderClsn6UpdateEv(void* self);
extern void _ZN9Animation7AdvanceEv(void* self);
extern char data_ov081_02128e24;
extern char data_ov081_02128e84;
extern char data_ov081_02128e64;
extern char data_ov081_02128e94;
extern void func_ov081_021254d8(void *c);
extern void func_ov081_021243cc(void *c);

struct PortMrBlizzardPair { unsigned enter_fn, enter_delta, tick_fn, tick_delta; };
typedef int (*PortMbFn)(void *);
}

int MrBlizzard::Behavior()
{
    char* c = (char*)this;
    void* r5;
    char* p;
    if (*(int*)(c + 0x41c) == 3) {
        switch (*(unsigned char*)(c + 0x469)) {
        case 0:
            if (_ZN8SaveData16HasPlayerLostCapEv()) *(unsigned char*)(c + 0x469) = 1;
            else *(unsigned char*)(c + 0x469) = 2;
            break;
        case 1:
            if (!_ZN8SaveData16HasPlayerLostCapEv()) *(unsigned char*)(c + 0x469) = 2;
            break;
        case 2:
            if (_ZN8SaveData16HasPlayerLostCapEv()) *(int*)(c + 0x41c) = 2;
            break;
        }
        return 1;
    }
    if (_ZN5Enemy26UpdateKillByInvincibleCharER12WithMeshClsnR9ModelAnimj(c, c + 0x150, c + 0x30c, 3)) return 1;
    if (*(int*)(c + 0x10c) != 0) {
        if (_ZN5Enemy11UpdateDeathER12WithMeshClsn(c, c + 0x150) && *(int*)(c + 0x41c) == 2) {
            _ZN5Actor5SpawnEjjRK7Vector3PK10Vector3_16as(0xdf, 0x300, (Vector3*)(c + 0x44c), 0, *(signed char*)(c + 0xcc), -1);
            *(int*)(c + 0x41c) = 0;
        }
        if (*(int*)(c + 0x10c) == 0 && *(unsigned char*)(c + 0x468) != 0) {
            func_02012694(0x166, c + 0x74);
            *(unsigned char*)(c + 0x468) = 0;
        }
        func_ov081_021254d8(c);
        return 1;
    }
    if (*(int*)(c + 0x41c) == 2
        && *(void**)(c + 0x3f8) != (void*)&data_ov081_02128e94
        && *(void**)(c + 0x3f8) != (void*)&data_ov081_02128e24
        && _ZN8SaveData16HasPlayerLostCapEv()
        && *(int*)(c + 0x400) == 0) {

        p = (char*)_ZN5Actor13ClosestPlayerEv(c);
        if (p != 0) {
            int param = 0xc;
            param = param | (*(unsigned char*)(p + 0x6d9) << 8);
            r5 = (void*)_ZN5Actor5SpawnEjjRK7Vector3PK10Vector3_16as(
                0x10d, param, (Vector3*)(c + 0x5c), 0, *(signed char*)(c + 0xcc), -1);
            if (r5 != 0) {
                _ZN5Actor9SetRangesE5Fix12IiES1_S1_S1_(r5, 0x64000, 0xc8000, 0x1000000, 0x1000000);
                *(int*)(c + 0x400) = *(int*)((char*)r5 + 4);
            }
        }

    }
    *(short*)(c + 0x8c) = *(short*)(c + 0x92);
    *(short*)(c + 0x8e) = *(short*)(c + 0x94);
    *(short*)(c + 0x90) = *(short*)(c + 0x96);
    DecIfAbove0_Short((unsigned short*)(c + 0x100));
    {
        PortMrBlizzardPair *cell = *(PortMrBlizzardPair **)(c + 0x3f8);
        if (cell->tick_fn != 0)
            ((PortMbFn)(size_t)cell->tick_fn)(c);
    }
    _ZN5Actor9UpdatePosEP12CylinderClsn(c, c + 0x110);
    if (*(int*)(c + 0x41c) == 0)
        _ZN5Enemy12UpdateWMClsnER12WithMeshClsnj(c, c + 0x150, 0);
    func_ov081_021254d8(c);
    if (*(void**)(c + 0x3f8) != (void*)&data_ov081_02128e84
        && *(void**)(c + 0x3f8) != (void*)&data_ov081_02128e64
        && *(void**)(c + 0x3f8) != (void*)&data_ov081_02128e94
        && *(void**)(c + 0x3f8) != (void*)&data_ov081_02128e24)
        func_ov081_021243cc(c);
    _ZN12CylinderClsn5ClearEv(c + 0x110);
    {
        p = (char*)_ZN5Actor13ClosestPlayerEv(c);
        if (p != 0 && *(unsigned char*)(p + 0x6fb) == 0)
            _ZN12CylinderClsn6UpdateEv(c + 0x110);
    }
    *(int*)(c + 0x368) = 0x1000;
    _ZN9Animation7AdvanceEv(c + 0x35c);
    return 1;
}
