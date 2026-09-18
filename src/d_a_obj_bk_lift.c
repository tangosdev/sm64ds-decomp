// @symbol daObjBk_Lift_c_classInit
/* recovered: vtable identified, globals resolved, declarations from a shared header */
#include "decl_ActorBase.h"
#include "decl_Platform.h"
#include "decl_ShadowModel.h"
#include "decl_common.h"
/* recovered: vtable identified, globals resolved */
/* resolved: VT0 = _ZTV14daObjBk_Lift_c */
/* SM64DS proves daObjBk_Lift_c through RTTI, allocation size, vtable identity
 * and the BK_LIFT01 registry profile; later EAD lineage supplies classInit.
 * The implementation side carries the same name now. It used to be spelled with
 * the coined `TowerStep`, so this vtable address carried both spellings at once;
 * that alias has been collapsed onto the cartridge name.
 * Historical aliases: TowerStep, TowerStep_Spawn. */
int *daObjBk_Lift_c_classInit(void)
{
    int *p = (int *)_ZN7fBase_cnwEj(916);
    if (p) {
        _ZN10dBgActor_cC2Ev(p);
        p[0] = (int)_ZTV14daObjBk_Lift_c;
        _ZN11ShadowModelC1Ev((char *)p + 0x320);
    }
    return p;
}
