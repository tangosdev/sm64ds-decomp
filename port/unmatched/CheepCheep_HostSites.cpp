/* PORT_HOST_ABI. CHEEP_CHEEP (227, 12daPukupuku_c), ov090. Run rel0215 wave 2,
 * lane cast-ov090. Three sites; see unmatched/Skeeter_HostSites.cpp for the
 * full derivation of the ModelAnim and PMF shapes.
 *
 * (1) src/_ZN10CheepCheep6RenderEv.cpp -- the ModelAnim slot-5 collision (T1),
 *     guarded by unk_0b0 & 0x40000 the way Skeeter's is, null scale.
 * (2) src/func_ov090_021332e8.cpp -- the state setter, record 0 (ENTER).
 * (3) src/_ZN10CheepCheep8BehaviorEv.cpp -- the record-1 (TICK)
 *     pointer-to-member call. This one is spelled as a REAL C++ METHOD in the
 *     source (`int CheepCheep::Behavior()`, out of include/CheepCheep.h) with
 *     the PMF taken through a forward-declared `struct Enemy;` -- incomplete
 *     at the typedef, so MSVC's four-word unknown-inheritance form again.
 *     No PathPtr in this one.
 */
#include "ModelAnim.h"

extern "C" {

/* ---- (1) Render -------------------------------------------------------- */
/* PORT_HOST_ABI: ROM-order ModelAnim slot-5 dispatch. */
int _ZN10CheepCheep6RenderEv(void *selfv)
{
    char *c = (char *)selfv;
    if ((*(unsigned int *)(c + 0xb0) & 0x40000) != 0)
        return 1;
    ((ModelAnim *)(c + 0x30c))->ModelAnim::Render(0);
    return 1;
}

/* ---- (2) and (3) ------------------------------------------------------- */
struct PortOv090Pmf { unsigned int fn; int delta; };
typedef int (*PortOv090StateFn)(void *);

/* PORT_HOST_ABI: mwcc pointer-to-member through an incomplete class. */
int func_ov090_021332e8(void *cv, void *pv)   /* record 0, the ENTER half */
{
    *(void **)((char *)cv + 0x370) = pv;
    PortOv090Pmf *q = (PortOv090Pmf *)pv;
    if (q[0].fn == 0)
        return 1;
    return ((PortOv090StateFn)(size_t)q[0].fn)(cv);
}

int  _ZN5Enemy14UpdateYoshiEatER12WithMeshClsn(void *self, void *wm);
void _ZN12CylinderClsn5ClearEv(void *c);
void _ZN12CylinderClsn6UpdateEv(void *c);
unsigned short DecIfAbove0_Short(unsigned short *p);
void _ZN5Actor9UpdatePosEP12CylinderClsn(void *thiz, void *clsn);
void _ZN9Animation7AdvanceEv(void *thiz);
char *_ZN5Actor13ClosestPlayerEv(void *thiz);
void func_ov090_02133338(void *c);
void func_ov090_021330c8(void *c);

/* PORT_HOST_ABI: mwcc pointer-to-member through an incomplete class; the rest
 * is the matched source statement for statement. */
int _ZN10CheepCheep8BehaviorEv(void *selfv)
{
    char *c = (char *)selfv;
    if (_ZN5Enemy14UpdateYoshiEatER12WithMeshClsn(c, c + 0x150) != 0) {
        _ZN12CylinderClsn5ClearEv(c + 0x110);
        if (*(unsigned char *)(c + 0x107) != 0) {
            if (*(unsigned short *)(c + 0x104) == 0) {
                _ZN12CylinderClsn6UpdateEv(c + 0x110);
            }
        }
        func_ov090_02133338(c);
        return 1;
    }

    DecIfAbove0_Short((unsigned short *)(c + 0x100));
    _ZN5Actor9UpdatePosEP12CylinderClsn(c, (void *)(c + 0x110));
    {
        PortOv090Pmf *q = *(PortOv090Pmf **)(c + 0x370);
        if (q[1].fn != 0)
            ((PortOv090StateFn)(size_t)q[1].fn)(c);
    }
    *(short *)(c + 0x8e) = *(short *)(c + 0x94);
    *(int *)(c + 0x368) = 0x1000;
    _ZN9Animation7AdvanceEv(c + 0x35c);
    func_ov090_02133338(c);
    func_ov090_021330c8(c);
    _ZN12CylinderClsn5ClearEv(c + 0x110);
    {
        char *p = _ZN5Actor13ClosestPlayerEv(c);
        if (p != 0 && *(unsigned char *)(p + 0x6fb) == 0) {
            _ZN12CylinderClsn6UpdateEv(c + 0x110);
        }
    }
    return 1;
}

}  /* extern "C" */
