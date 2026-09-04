// @symbol daObjCvShutter_c_classInit
/* recovered: vtable identified, declarations from a shared header */
#include "decl_ActorBase.h"
#include "decl_Platform.h"
#include "decl_common.h"
extern int _ZTV13daObjSwdoor_c[];
extern int _ZTV16daObjCvShutter_c[];
/* recovered: vtable identified */
/* vtable identified: VT0 = _ZTV13daObjSwdoor_c */
/* Reconstructed source-style name: SM64DS proves daObjCvShutter_c through RTTI,
 * allocation size, most-derived vtable identity, and the CV_SHUTTER registry
 * profile; later EAD lineage supplies classInit. Exact original spelling is
 * not preserved. The project's daObjCvShutter_c implementation aliases remain
 * unchanged. Historical alias: ShutterHmc_Spawn. */
int *daObjCvShutter_c_classInit(void)
{
    int *p = (int *)_ZN7fBase_cnwEj(804);
    if (p) {
        _ZN10dBgActor_cC2Ev(p);
        p[0] = (int)_ZTV13daObjSwdoor_c;
        p[0] = (int)_ZTV16daObjCvShutter_c;
    }
    return p;
}
